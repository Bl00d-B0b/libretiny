/* Copyright (c) Bl00d-B0b 2026-08-06. */

#pragma once

#include <lt_pins.h>

// AmebaD (RTL8720D) KM4 application core.
// The log console lives on LOGUART (KM0-owned at boot, handed to KM4), wired
// to PA7/PA8 on BW16 — the same pins the boot ROM uses for UART download.
// Port 2 in the printf port's mapping; verified on hardware 2026-08-07.
#ifndef LT_UART_DEFAULT_PORT
#define LT_UART_DEFAULT_PORT 2
#endif

// Auto-download-reboot detection: the AmebaD boot ROM handshake byte
// sequence. TODO(bench): capture the actual ROM prompt from BW16 hardware;
// the AmebaZ "ping" pattern does not apply to this ROM.
// #define LT_UART_ADR_PATTERN <pending bench capture>
