/* Copyright (c) Bl00d-B0b 2026-09-11. */

#include "include.h"
#include "rtos_pub.h"
#include "uart_pub.h"

// The BDK's wpa_ctrl_request() (libsupplicant) queues a command to the
// wpa_supplicant task and waits for completion with BEKEN_WAIT_FOREVER. Every
// STA start/stop/scan goes through it from the application task, so a stalled
// supplicant stops the caller for good: no watchdog is armed, and on ESPHome the
// blocked task is the one that serves buttons, the API, OTA and the WiFi reboot
// timeout - the device stays dead until a power cycle (esphome/esphome#18592).
//
// This wrap runs the real call on a helper thread and bounds the wait. On a
// timeout the caller gets an error and keeps running; the helper stays parked
// on the stalled request and later calls fail fast, which lets the application
// fall back to its own recovery (ESPHome reboots after wifi.reboot_timeout).

typedef int wpa_ctrl_cmd_t_;
extern int __real_wpa_ctrl_request(wpa_ctrl_cmd_t_ cmd, void *data);
extern beken_thread_t wpas_thread_handle;

#define WPA_CTRL_TIMEOUT_MS	  10000
#define WPA_CTRL_WORKER_STACK 3072

static beken_thread_t worker		  = NULL;
static beken_mutex_t lock			  = NULL;
static beken_semaphore_t start_sem	  = NULL;
static beken_semaphore_t done_sem	  = NULL;
static volatile wpa_ctrl_cmd_t_ q_cmd = 0;
static void *volatile q_data		  = NULL;
static volatile int q_result		  = 0;
static volatile int stalled			  = 0;

static void wpa_ctrl_worker(beken_thread_arg_t arg) {
	(void)arg;
	for (;;) {
		if (rtos_get_semaphore(&start_sem, BEKEN_WAIT_FOREVER) != kNoErr)
			continue;
		q_result = __real_wpa_ctrl_request(q_cmd, q_data);
		rtos_set_semaphore(&done_sem);
	}
}

static int wpa_ctrl_worker_init(void) {
	if (worker != NULL)
		return 0;
	if (rtos_init_mutex(&lock) != kNoErr || rtos_init_semaphore(&start_sem, 1) != kNoErr ||
		rtos_init_semaphore(&done_sem, 1) != kNoErr)
		return -1;
	return rtos_create_thread(
			   &worker,
			   THD_APPLICATION_PRIORITY,
			   "wpa_ctrl",
			   wpa_ctrl_worker,
			   WPA_CTRL_WORKER_STACK,
			   NULL
		   ) == kNoErr
			   ? 0
			   : -1;
}

int __wrap_wpa_ctrl_request(wpa_ctrl_cmd_t_ cmd, void *data) {
	int result;

	// Calls from the supplicant thread itself are handled inline by the BDK.
	if (wpas_thread_handle && rtos_is_current_thread(&wpas_thread_handle))
		return __real_wpa_ctrl_request(cmd, data);

	if (wpa_ctrl_worker_init() != 0) {
		os_printf("%s: worker init failed, calling directly\r\n", __FUNCTION__);
		return __real_wpa_ctrl_request(cmd, data);
	}

	rtos_lock_mutex(&lock);
	if (stalled) {
		// A previous request never completed; the worker is still inside it.
		os_printf("%s: cmd %d refused, supplicant stalled\r\n", __FUNCTION__, cmd);
		rtos_unlock_mutex(&lock);
		return -1;
	}

	q_cmd  = cmd;
	q_data = data;
	rtos_set_semaphore(&start_sem);
	if (rtos_get_semaphore(&done_sem, WPA_CTRL_TIMEOUT_MS) == kNoErr) {
		result = q_result;
	} else {
		os_printf("%s: cmd %d timed out after %d ms, supplicant stalled\r\n", __FUNCTION__, cmd, WPA_CTRL_TIMEOUT_MS);
		stalled = 1;
		result	= -1;
	}
	rtos_unlock_mutex(&lock);
	return result;
}
