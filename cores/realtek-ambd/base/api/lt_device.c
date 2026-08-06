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
