/* Copyright (c) Bl00d-B0b 2026-08-06. */

#include <libretiny.h>
#include <sdk_private.h>

// AmebaD OTA slot selection (rtl8721dhp_ota_ram.c): each slot begins with the
// 8-byte image signature "81958711"; the bootloader boots OTA2 when its
// signature is valid, OTA1 otherwise. Switching = writing the signature to the
// target slot head and blanking the other. No system-data bit flag exists.
#define IMG2_SIGN		"81958711"
#define AMBD_OTA_SIGN_0 0x35393138 // "8195"
#define AMBD_OTA_SIGN_1 0x31313738 // "8711"

lt_ota_type_t lt_ota_get_type() {
	return OTA_TYPE_DUAL;
}

/**
 * Convert OTA index (1, 2) into partition offset.
 */
bool lt_ota_dual_get_offset(uint8_t index, uint32_t *offset) {
	switch (index) {
		case 1:
			*offset = FLASH_OTA1_OFFSET;
			break;
		case 2:
			*offset = FLASH_OTA2_OFFSET;
			break;
		default:
			return false;
	}
	return true;
}

/**
 * Check which OTA image the bootloader would pick, by the OTA2 slot signature.
 */
uint8_t lt_ota_dual_get_stored_by_flag() {
	uint32_t sig[2];
	if (lt_flash_read(FLASH_OTA2_OFFSET, (void *)sig, 8) != 8)
		return 1;
	return (sig[0] == AMBD_OTA_SIGN_0 && sig[1] == AMBD_OTA_SIGN_1) ? 2 : 1;
}

// The signature is part of the image, so writing an update into OTA2 flips
// what lt_ota_dual_get_stored_by_flag() reports before any explicit switch.
// The running slot therefore has to be latched from the signature state
// before the first flash write; the first lt_ota call happens in
// lt_ota_begin(), which precedes all writes.
static uint8_t s_boot_slot = 0;

static uint8_t lt_ota_boot_slot() {
	if (s_boot_slot == 0)
		s_boot_slot = lt_ota_dual_get_stored_by_flag();
	return s_boot_slot;
}

// The vendor tree defines this in rtl8721d_ota.c, which stays out of the
// build (SD-card/HTTP OTA app code); app_start references it. lt_reboot()
// reproduces its backup-register + watchdog full-chip reset.
void ota_platform_reset(void) {
	lt_reboot();
}

// FLASH_WriteStream/FLASH_EreaseDwordsXIP/FLASH_TxData12BXIP come from
// rtl8721d_flash.h via ameba_soc.h (sdk_private.h).

/**
 * Activate the slot the update was written to. The new image arrives with its
 * signature in place, so ordering is: make sure the target signature is valid
 * FIRST, and only then blank the old slot's — a crash in between leaves the
 * old image bootable instead of bricking both slots. The running slot's
 * signature is never erased before the target is proven valid.
 */
bool lt_ota_dual_switch_flag() {
	u32 sig[2]		 = {AMBD_OTA_SIGN_0, AMBD_OTA_SIGN_1};
	u8 blank[8]		 = {0};
	uint8_t target	 = lt_ota_boot_slot() ^ 0b11;
	uint32_t tgt_off = target == 2 ? FLASH_OTA2_OFFSET : FLASH_OTA1_OFFSET;
	uint32_t old_off = target == 2 ? FLASH_OTA1_OFFSET : FLASH_OTA2_OFFSET;
	if (!lt_ota_is_valid(target)) {
		// The update did not carry the signature: write it.
		FLASH_EreaseDwordsXIP(tgt_off, 2);
		FLASH_TxData12BXIP(tgt_off, 8, (u8 *)sig);
		if (!lt_ota_is_valid(target))
			return false;
	}
	if (target == 1) {
		// The bootloader prefers OTA2 whenever its signature is valid, so
		// activating OTA1 means invalidating OTA2's signature — the slot this
		// code is executing from. FLASH_EreaseDwordsXIP must not be used here:
		// it erases and rewrites the whole 4KB sector holding the active KM0
		// image header, which hard-faults mid-rewrite (bench 2026-08-09). The
		// SDK's OTA_Change never erases the active slot either — it programs
		// zeros over the signature (NOR 1->0, no erase). Mirror that with the
		// XIP-safe writer, then drop the stale cache line so the verify below
		// reads flash, not the cached signature.
		FLASH_TxData12BXIP(old_off, 8, blank);
		DCache_Invalidate(SPI_FLASH_BASE + old_off, 8);
	}
	return lt_ota_dual_get_stored_by_flag() == target;
}

bool lt_ota_is_valid(uint8_t index) {
	uint32_t offset;
	if (!lt_ota_dual_get_offset(index, &offset))
		return false;
	uint8_t head[8];
	if (lt_flash_read(offset, head, sizeof(head)) != sizeof(head))
		return false;
	return memcmp(head, IMG2_SIGN, 8) == 0;
}

uint8_t lt_ota_dual_get_current() {
	// KM4 image2 runs from the 0x0E000000 XIP window, which the bootloader
	// maps onto the active slot — the code address carries no flash offset.
	// The bootloader picks OTA2 whenever its signature is valid, so the slot
	// that booted is the signature state latched before the first OTA write.
	return lt_ota_boot_slot();
}

uint8_t lt_ota_dual_get_stored() {
	return lt_ota_dual_get_stored_by_flag();
}

bool lt_ota_switch(bool revert) {
	uint8_t current = lt_ota_dual_get_current();
	uint8_t stored	= lt_ota_dual_get_stored();
	if ((current == stored) == revert)
		return true;
	if (!lt_ota_is_valid(stored ^ 0b11))
		return false;
	return lt_ota_dual_switch_flag();
}
