/* Copyright (c) Etienne Le Cousin 2024-02-24. */

#include <libretiny.h>
#include <sdk_private.h>

// Declared in components/utils/ln_misc.h; compiled in ln882h_net.
// Not thread-safe — safe here since lt_init_family() runs before the RTOS scheduler.
extern int ln_generate_random_mac(uint8_t *addr);

extern uint8_t uart_print_port;
extern Serial_t m_LogSerial;

static void lt_init_log(void) {
	// default LT print port
	uart_print_port = LT_UART_DEFAULT_LOGGER;
	// default SDK print port
	serial_init(&m_LogSerial, LT_UART_DEFAULT_PORT, CFG_UART_BAUDRATE_LOG, NULL);
}

// Randomize MAC addresses if they still hold the factory-default placeholder.
// Every LN882H ships with the same MAC (00:50:C2:5E:10:88) written to KV flash
// by sysparam_integrity_check_all(), causing all devices to share one address.
// The constant is STA_MAC_ADDR0..5 from sysparam_factory_setting.h.
static void lt_init_unique_mac(void) {
	// Factory-default STA MAC (sysparam_factory_setting.h: STA_MAC_ADDR0..5)
	static const uint8_t factory_mac[6] = {0x00, 0x50, 0xC2, 0x5E, 0x10, 0x88};
	uint8_t mac[6];

	if (SYSPARAM_ERR_NONE != sysparam_sta_mac_get(mac))
		return;
	if (memcmp(mac, factory_mac, 6) != 0)
		return; // already unique

	// Still the factory default — generate a unique address via TRNG
	if (ln_generate_random_mac(mac) != 0)
		return;
	sysparam_sta_mac_update(mac);

	// Derive SoftAP MAC: set the locally-administered bit on the first octet
	// so the AP address is distinct without consuming a second TRNG draw.
	mac[0] |= 0x02;
	sysparam_softap_mac_update(mac);

	LT_I("Generated unique WiFi MAC addresses");
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
