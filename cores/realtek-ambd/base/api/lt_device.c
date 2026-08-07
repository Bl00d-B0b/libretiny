/* Copyright (c) Bl00d-B0b 2026-08-06. */

#include <libretiny.h>
#include <sdk_private.h>

void lt_reboot(void) {
	// System reset through the SDK wrapper (resets both KM0 and KM4).
	extern void sys_reset(void);
	sys_reset();
	while (1) {}
}

lt_reboot_reason_t lt_get_reboot_reason(void) {
	// TODO(bench): map BOOT_Reason() / AON backup registers to
	// lt_reboot_reason_t values.
	return REBOOT_REASON_UNKNOWN;
}

// Factory MAC addresses live in the logical eFuse map (OTP), not flash:
// WiFi at 0x11A (wlan driver "read_mac"), BT at 0x190 (hci_board.c).
#define AMBD_EFUSE_WIFI_MAC 0x11A

void lt_get_device_mac(uint8_t *mac) {
	extern int efuse_logical_read(u16 addr, u16 size, u8 *pbuf);
	uint8_t buf[6];
	if (efuse_logical_read(AMBD_EFUSE_WIFI_MAC, 6, buf) >= 0) {
		// all-FF means the map slot was never programmed
		for (int i = 0; i < 6; i++) {
			if (buf[i] != 0xFF) {
				memcpy(mac, buf, 6);
				return;
			}
		}
	}
	// fall back to the WiFi driver's own view once it is up
	memset(mac, 0, 6);
}
