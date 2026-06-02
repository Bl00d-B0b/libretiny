/* Copyright (c) Etienne Le Cousin 2024-02-24. */

#include <libretiny.h>
#include <sdk_private.h>

// Declared in components/utils/ln_misc.h; compiled in ln882h_net.
// Not thread-safe — safe here since lt_init_family() runs before the RTOS scheduler.
extern int ln_generate_random_mac(uint8_t *addr);

// hal_flash_read: declared in mcu/driver_ln882h/hal/hal_flash.h; compiled in
// framework-lightning-ln882h.  Forward-declared to avoid the hexdump/ln_hexdump
// macro conflict triggered by hal_flash.h -> hal_common.h -> proj_config.h.
// Only needed when FLASH_TUYA_KV_OFFSET is defined (Tuya board variant).
#ifdef FLASH_TUYA_KV_OFFSET
extern uint8_t hal_flash_read(uint32_t offset, uint32_t length, uint8_t *buffer);
#endif

extern uint8_t uart_print_port;
extern Serial_t m_LogSerial;

static void lt_init_log(void) {
	// default LT print port
	uart_print_port = LT_UART_DEFAULT_LOGGER;
	// default SDK print port
	serial_init(&m_LogSerial, LT_UART_DEFAULT_PORT, CFG_UART_BAUDRATE_LOG, NULL);
}

// Scan the Tuya BLK_V1.0 KV store for the last valid "6_sta_mac" entry.
// Only compiled on Tuya board variants that define FLASH_TUYA_KV_OFFSET in
// their board JSON (e.g. boards/_base/lightning-ln882hki-tuya.json).
//
// BLK_V1.0 entry layout (no null terminator between key and value):
//   it_magic[8] | crc16[2] | val_size_u16_le[2] | prev_ptr_u32_le[4] | key[N] | value[val_size]
//
// The factory sentinel "6_sta_mac" #1 is always 00:50:C2:5E:10:88 (from
// sysparam_factory_setting.h).  The real unique MAC is written as a second
// "6_sta_mac" entry during Tuya manufacturing.  BLK_V1.0 is an append-only log,
// so the last matching entry wins — we return that.
//
// Returns true and fills mac_out[6] if a valid non-sentinel MAC is found.
#ifdef FLASH_TUYA_KV_OFFSET
static bool lt_tuya_kv_read_sta_mac(uint8_t *mac_out) {
	static const uint8_t IT_MAGIC[8]	= "it_magic";
	static const uint8_t STA_MAC_KEY[9] = "6_sta_mac";
	static const uint8_t SENTINEL[6]	= {0x00, 0x50, 0xC2, 0x5E, 0x10, 0x88};

	// Tuya KV offset and size from board JSON
	// (generated as FLASH_TUYA_KV_OFFSET / FLASH_TUYA_KV_LENGTH by the board generator)
	const uint32_t kv_base = FLASH_TUYA_KV_OFFSET;
	const uint32_t kv_size = FLASH_TUYA_KV_LENGTH;

	// Entry buffer: it_magic(8) + header(8) + key(9) + value(6) = 31 bytes
	uint8_t entry[31];
	bool found = false;
	uint32_t off;

	for (off = 0; off + sizeof(entry) <= kv_size; off++) {
		hal_flash_read(kv_base + off, 8, entry);
		if (memcmp(entry, IT_MAGIC, 8) != 0)
			continue;

		// Read header + key + value
		hal_flash_read(kv_base + off + 8, 23, entry + 8);

		uint16_t val_size = entry[10] | ((uint16_t)entry[11] << 8);
		if (val_size != 6)
			goto next;
		if (memcmp(entry + 16, STA_MAC_KEY, 9) != 0)
			goto next;

		// entry[25..30] = 6-byte MAC value
		if (memcmp(entry + 25, SENTINEL, 6) == 0)
			goto next;
		{
			bool all_ff = true;
			for (int i = 0; i < 6; i++)
				if (entry[25 + i] != 0xFF) {
					all_ff = false;
					break;
				}
			if (all_ff)
				goto next;
		}

		memcpy(mac_out, entry + 25, 6);
		found = true; // keep scanning -- last entry wins in BLK_V1.0 log

	next:
		off += 7; // skip past magic header, outer off++ gives total advance of 8
	}
	return found;
}
#endif // FLASH_TUYA_KV_OFFSET

// Ensure the stored STA/SoftAP MACs are unique on first boot after flash.
//
// Priority:
//   1. LibreTiny sysparam KV already holds a non-sentinel MAC → nothing to do.
//      (fast path on every boot after first; also covers LibreTiny→LibreTiny OTA)
//   2. [Tuya boards only] BLK_V1.0 KV at FLASH_TUYA_KV_OFFSET has a valid
//      unique MAC → restore it.
//      (first boot after UART flash or Kickstart/cloudcutter OTA from Tuya stock)
//   3. TRNG fallback — Tuya KV absent/erased, or non-Tuya board.
//
// sysparam KV at 0x1E0000 is outside the OTA partition and is never erased by
// OTA, so the resolved MAC is stable across all future LibreTiny OTA updates.
static void lt_init_unique_mac(void) {
	static const uint8_t factory_mac[6] = {0x00, 0x50, 0xC2, 0x5E, 0x10, 0x88};
	uint8_t mac[6];

	if (SYSPARAM_ERR_NONE != sysparam_sta_mac_get(mac))
		return;
	if (memcmp(mac, factory_mac, 6) != 0)
		return; // step 1: already unique, nothing to do

#ifdef FLASH_TUYA_KV_OFFSET
	// Step 2 (Tuya boards): try to recover factory MAC from BLK_V1.0 KV
	if (lt_tuya_kv_read_sta_mac(mac)) {
		LT_I(
			"Restored Tuya factory MAC: %02X:%02X:%02X:%02X:%02X:%02X",
			mac[0],
			mac[1],
			mac[2],
			mac[3],
			mac[4],
			mac[5]
		);
		sysparam_sta_mac_update(mac);
		mac[0] |= 0x02; // locally-administered bit → distinct SoftAP MAC
		sysparam_softap_mac_update(mac);
		return;
	}
#endif // FLASH_TUYA_KV_OFFSET

	// Step 3: TRNG — non-Tuya board, or Tuya KV absent/erased
	if (ln_generate_random_mac(mac) != 0)
		return;
	LT_I("Generated random unique MAC: %02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	sysparam_sta_mac_update(mac);
	mac[0] |= 0x02;
	sysparam_softap_mac_update(mac);
}

void lt_init_family() {
	// 0. check reboot cause
	ln_chip_get_reboot_cause();

	// 1. sys clock,interrupt
	SetSysClock();
	set_interrupt_priority();
	switch_global_interrupt(HAL_ENABLE);
	ln_runtime_measure_init();

	// 2. register os heap mem
	OS_DefineHeapRegions();

	// 3. log init
	lt_init_log();

	cm_backtrace_init("LibreTiny - LN882H", "HW_V1.0", "SW_V1.0");

	if (NVDS_ERR_OK != ln_nvds_init(FLASH_NVDS_OFFSET)) {
		LT_E("NVDS init failed!");
	}

	if (KV_ERR_NONE != ln_kv_port_init(FLASH_KV_OFFSET, (FLASH_KV_OFFSET + FLASH_KV_LENGTH))) {
		LT_E("KV init failed!");
	}

	// init system parameter
	sysparam_integrity_check_all();

	// randomize MAC if still at factory default (first boot after flash)
	lt_init_unique_mac();

	ln_pm_sleep_mode_set(ACTIVE);
	// ln_pm_always_clk_disable_select(CLK_G_I2S | CLK_G_WS2811 | CLK_G_SDIO);
	/*ln_pm_always_clk_disable_select(CLK_G_I2S | CLK_G_WS2811 | CLK_G_SDIO | CLK_G_AES);
	ln_pm_lightsleep_clk_disable_select(CLK_G_GPIOA | CLK_G_GPIOB | CLK_G_SPI0 | CLK_G_SPI1 | CLK_G_I2C0 |
					  CLK_G_UART1 | CLK_G_UART2 | CLK_G_WDT | CLK_G_TIM1 | CLK_G_TIM2 | CLK_G_MAC |
	CLK_G_DMA | CLK_G_RF | CLK_G_ADV_TIMER| CLK_G_TRNG);*/
}

void lt_init_arduino() {
#if LT_AUTO_DOWNLOAD_REBOOT && LT_ARD_HAS_SERIAL && LT_HW_UART0
	// initialize auto-download-reboot parser
	Serial0.begin(115200);
#endif
}
