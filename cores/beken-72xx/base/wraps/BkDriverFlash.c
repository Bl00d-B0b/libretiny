/* Copyright (c) Kuba Szczodrzyński 2022-07-07. */

#include "BkDriverFlash.h"
#include "drv_model_pub.h"
#include "uart_pub.h"

// BK_PARTITION_BLE_BONDING_FLASH exists since BDK 3.0.76. The placement
// invariants (sector alignment, minimum size, no overlap) are enforced at
// build time in builder/utils/flash.py.
#if CFG_BDK_VERSION >= 30076
#ifndef FLASH_BLE_BONDING_OFFSET
#error "Board flash layout must declare a 'ble_bonding' partition (see boards/_base/beken-*.json)"
#endif
#endif

static const bk_logic_partition_t bk7231_partitions[BK_PARTITION_MAX] = {
	[BK_PARTITION_BOOTLOADER] =
		/**/
	{
		.partition_owner	   = BK_FLASH_EMBEDDED,
		.partition_description = "Bootloader",
		.partition_start_addr  = FLASH_BOOTLOADER_OFFSET,
		.partition_length	   = FLASH_BOOTLOADER_LENGTH,
		.partition_options	   = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
	 },
	[BK_PARTITION_APPLICATION] =
		/**/
	{
		.partition_owner	   = BK_FLASH_EMBEDDED,
		.partition_description = "Application",
		.partition_start_addr  = FLASH_APP_OFFSET,
		.partition_length	   = FLASH_APP_LENGTH,
		.partition_options	   = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
	 },
	[BK_PARTITION_OTA] =
		/**/
	{
		.partition_owner	   = BK_FLASH_EMBEDDED,
		.partition_description = "ota",
		.partition_start_addr  = FLASH_DOWNLOAD_OFFSET,
		.partition_length	   = FLASH_DOWNLOAD_LENGTH,
		.partition_options	   = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
	 },
	[BK_PARTITION_RF_FIRMWARE] =
		/**/
	{
		.partition_owner	   = BK_FLASH_EMBEDDED,
		.partition_description = "RF Firmware",
		.partition_start_addr  = FLASH_CALIBRATION_OFFSET,
		.partition_length	   = FLASH_CALIBRATION_LENGTH,
		.partition_options	   = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
	 },
	[BK_PARTITION_NET_PARAM] =
		/**/
	{
		.partition_owner	   = BK_FLASH_EMBEDDED,
		.partition_description = "NET info",
		.partition_start_addr  = 0x1FF000,
		.partition_length	   = 0,
		.partition_options	   = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
	 },
#if CFG_BDK_VERSION >= 30076
	// The BDK bond store erases the sector at this partition's start address
	// on every BLE bring-up. Without this entry the zero-filled slot points
	// at flash sector 0 and the bootloader is erased (esphome/esphome#18646).
	// Claimed sectors per layout: 0x1E1000 (beken-72xx) and 0x1D1000
	// (7231n-tuya) are the former net/tlv partitions removed in 9598759;
	// 0x1E2000 (7238-tuya) is free between diff2ya and calibration; 0x3FF000
	// (7252) is the tail sector of its 4 MiB flash. Restoring net/tlv at
	// their stock addresses would collide with this partition.
	[BK_PARTITION_BLE_BONDING_FLASH] =
		/**/
	{
		.partition_owner	   = BK_FLASH_EMBEDDED,
		.partition_description = "BLE bonding",
		.partition_start_addr  = FLASH_BLE_BONDING_OFFSET,
		.partition_length	   = FLASH_BLE_BONDING_LENGTH,
		.partition_options	   = PAR_OPT_READ_EN | PAR_OPT_WRITE_DIS,
	 },
#endif
};

bk_logic_partition_t *__wrap_bk_flash_get_info(bk_partition_t partition) {
	bk_logic_partition_t *info;
	if ((partition >= BK_PARTITION_BOOTLOADER) && (partition < BK_PARTITION_MAX)) {
		info = (bk_logic_partition_t *)&bk7231_partitions[partition];
		// An unmapped enum value is a zero-filled slot pointing at flash
		// sector 0; report it as not-found instead. NET_PARAM stays visible
		// (zero-length entries fail the bounds checks below instead) so BDK
		// callers of get_info never see NULL for a populated slot.
		if (info->partition_description == NULL)
			return NULL;
		return info;
	}
	return NULL;
}

OSStatus __wrap_bk_flash_erase(bk_partition_t partition, uint32_t off_set, uint32_t size) {
	uint32_t i;
	uint32_t param;
	UINT32 status;
	DD_HANDLE flash_hdl;
	uint32_t start_sector, end_sector;
	bk_logic_partition_t *partition_info;
	GLOBAL_INT_DECLARATION();
	partition_info = bk_flash_get_info(partition);
	// Mirror the write/read paths: fail on an unmapped partition or an
	// over-long range instead of erasing neighbouring sectors.
	if (NULL == partition_info) {
		os_printf("%s partiion not found\r\n", __FUNCTION__);
		return kNotFoundErr;
	}
	if (size == 0 || off_set > partition_info->partition_length || size > partition_info->partition_length - off_set) {
		os_printf("%s erase range out of bounds\r\n", __FUNCTION__);
		return kParamErr;
	}
	start_sector = off_set >> 12;
	end_sector	 = (off_set + size - 1) >> 12;
	flash_hdl	 = ddev_open(FLASH_DEV_NAME, &status, 0);
	ASSERT(DD_HANDLE_UNVALID != flash_hdl);
	for (i = start_sector; i <= end_sector; i++) {
		param = partition_info->partition_start_addr + (i << 12);
		GLOBAL_INT_DISABLE();
		ddev_control(flash_hdl, CMD_FLASH_ERASE_SECTOR, (void *)&param);
		GLOBAL_INT_RESTORE();
	}
	return kNoErr;
}

OSStatus __wrap_bk_flash_write(
	bk_partition_t partition,
	volatile uint32_t off_set,
	uint8_t *inBuffer,
	uint32_t inBufferLength
) {
	UINT32 status;
	DD_HANDLE flash_hdl;
	uint32_t start_addr;
	bk_logic_partition_t *partition_info;
	GLOBAL_INT_DECLARATION();
	if (NULL == inBuffer) {
		os_printf("%s inBuffer=NULL\r\n", __FUNCTION__);
		return kParamErr;
	}
	partition_info = bk_flash_get_info(partition);
	if (NULL == partition_info) {
		os_printf("%s partiion not found\r\n", __FUNCTION__);
		return kNotFoundErr;
	}
	if (off_set > partition_info->partition_length || inBufferLength > partition_info->partition_length - off_set) {
		os_printf("%s write range out of bounds\r\n", __FUNCTION__);
		return kParamErr;
	}
	start_addr = partition_info->partition_start_addr + off_set;
	flash_hdl  = ddev_open(FLASH_DEV_NAME, &status, 0);
	if (DD_HANDLE_UNVALID == flash_hdl) {
		os_printf("%s open failed\r\n", __FUNCTION__);
		return kOpenErr;
	}
	GLOBAL_INT_DISABLE();
	ddev_write(flash_hdl, (char *)inBuffer, inBufferLength, start_addr);
	GLOBAL_INT_RESTORE();
	return kNoErr;
}

OSStatus __wrap_bk_flash_read(
	bk_partition_t partition,
	volatile uint32_t off_set,
	uint8_t *outBuffer,
	uint32_t inBufferLength
) {
	UINT32 status;
	uint32_t start_addr;
	DD_HANDLE flash_hdl;
	bk_logic_partition_t *partition_info;
	GLOBAL_INT_DECLARATION();
	if (NULL == outBuffer) {
		os_printf("%s outBuffer=NULL\r\n", __FUNCTION__);
		return kParamErr;
	}
	partition_info = bk_flash_get_info(partition);
	if (NULL == partition_info) {
		os_printf("%s partiion not found\r\n", __FUNCTION__);
		return kNotFoundErr;
	}
	if (off_set > partition_info->partition_length || inBufferLength > partition_info->partition_length - off_set) {
		os_printf("%s read range out of bounds\r\n", __FUNCTION__);
		return kParamErr;
	}
	start_addr = partition_info->partition_start_addr + off_set;
	flash_hdl  = ddev_open(FLASH_DEV_NAME, &status, 0);
	if (DD_HANDLE_UNVALID == flash_hdl) {
		os_printf("%s open failed\r\n", __FUNCTION__);
		return kOpenErr;
	}
	GLOBAL_INT_DISABLE();
	ddev_read(flash_hdl, (char *)outBuffer, inBufferLength, start_addr);
	GLOBAL_INT_RESTORE();
	return kNoErr;
}
