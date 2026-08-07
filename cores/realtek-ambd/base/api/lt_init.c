/* Copyright (c) Bl00d-B0b 2026-08-07. */

#include <libretiny.h>
#include <sdk_private.h>

extern uint8_t lt_uart_port;

void lt_init_family() {
	// route LibreTiny's printf port to the family default (LOGUART on AmebaD)
	lt_uart_port = LT_UART_DEFAULT_PORT;
	// silence the SDK's own DBG_8195A chatter (diag.h globals)
	ConfigDebugClose  = 1;
	ConfigDebugBuffer = 0;
}
