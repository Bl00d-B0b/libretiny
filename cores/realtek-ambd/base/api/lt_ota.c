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
 * Check which OTA image is active, by the OTA2 slot signature.
 */
uint8_t lt_ota_dual_get_stored_by_flag() {
	uint32_t sig[2];
	if (lt_flash_read(FLASH_OTA2_OFFSET, (void *)sig, 8) != 8)
		return 1;
	return (sig[0] == AMBD_OTA_SIGN_0 && sig[1] == AMBD_OTA_SIGN_1) ? 2 : 1;
}

// The vendor tree defines this in rtl8721d_ota.c, which stays out of the
// build (SD-card/HTTP OTA app code); app_start references it.
void ota_platform_reset(void) {
	extern void sys_reset(void);
	sys_reset();
	while (1) {}
}

// FLASH_WriteStream/FLASH_EreaseDwordsXIP/FLASH_TxData12BXIP come from
// rtl8721d_flash.h via ameba_soc.h (sdk_private.h).

/**
 * Switch the active OTA slot by moving the image signature
 * (the sequence OTA_Change() uses, with this board's slot offsets).
 */
bool lt_ota_dual_switch_flag() {
	u32 sig[2]	 = {AMBD_OTA_SIGN_0, AMBD_OTA_SIGN_1};
	u8 blank[8]	 = {0};
	uint32_t cur = FLASH_OTA1_OFFSET, tgt = FLASH_OTA2_OFFSET;
	if (lt_ota_dual_get_stored_by_flag() == 2) {
		cur = FLASH_OTA2_OFFSET;
		tgt = FLASH_OTA1_OFFSET;
	}
	// Zero the current slot's signature (NOR 1->0, no erase needed), then
	// dword-erase and rewrite the target slot's signature.
	FLASH_WriteStream(cur, 8, blank);
	FLASH_EreaseDwordsXIP(tgt, 2);
	FLASH_TxData12BXIP(tgt, 8, (u8 *)sig);
	return lt_ota_dual_get_stored_by_flag() != (cur == FLASH_OTA1_OFFSET ? 1 : 2);
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
	// that booted is the same one lt_ota_is_valid() reports for OTA2.
	return lt_ota_is_valid(2) ? 2 : 1;
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
