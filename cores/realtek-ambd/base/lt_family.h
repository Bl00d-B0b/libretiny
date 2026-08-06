/* Copyright (c) Bl00d-B0b 2026-08-06. */

#pragma once

#include <lt_pins.h>

// AmebaD (RTL8720D) KM4 application core.
// The log console lives on LOGUART (KM0-owned at boot, handed to KM4);
// exposed here as port 0 until the UART driver lands.
#ifndef LT_UART_DEFAULT_PORT
#define LT_UART_DEFAULT_PORT 0
#endif

// Auto-download-reboot detection: the AmebaD boot ROM handshake byte
// sequence. TODO(bench): capture the actual ROM prompt from BW16 hardware;
// the AmebaZ "ping" pattern does not apply to this ROM.
// #define LT_UART_ADR_PATTERN <pending bench capture>
