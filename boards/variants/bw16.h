/* BW16 (RTL8720DN) — hand-written first pass; regenerate with boardgen once
 * the ambd family lands in boardgen. Pin roles from the Ai-Thinker BW16
 * pinout; TODO(bench): verify PWM/I2C/SPI muxing per pin. */

#pragma once

// clang-format off

// Pins
// ----
#define PINS_COUNT         13 // Total GPIO count
#define NUM_DIGITAL_PINS   13 // Digital inputs/outputs
#define NUM_ANALOG_INPUTS  0  // ADC inputs
#define NUM_ANALOG_OUTPUTS 0  // PWM & DAC outputs
#define PINS_GPIO_MAX      42 // Last usable GPIO number (PB27-class numbering)

// Serial ports
// ------------
// LOG_UART on PA7 (RX) / PA8 (TX) — the console + download port.
// LOG UART (PA7/PA8) is Serial0; the user UART on PB1/PB2 is Serial2.
// UART1 is wired to the BT controller and is not exposed.
#define HAS_SERIAL0 1
#define HAS_SERIAL2 1

#define PIN_SERIAL0_RX 7u  // PIN_PA7
#define PIN_SERIAL0_TX 8u  // PIN_PA8
#define PINS_SERIAL0_RX {7u}
#define PINS_SERIAL0_TX {8u}
// UART2 (user, "RX0/TX0" silk) on PB1 (RX) / PB2 (TX).
#define PIN_SERIAL2_RX 33u // PIN_PB1
#define PIN_SERIAL2_TX 34u // PIN_PB2
#define PINS_SERIAL2_RX {33u}
#define PINS_SERIAL2_TX {34u}

#define SERIAL_INTERFACES_COUNT 2

// Pin function macros
// -------------------
#define PINS_GPIO {7u, 8u, 9u, 12u, 13u, 14u, 25u, 26u, 27u, 30u, 33u, 34u, 35u}

// LED
// ---
// Onboard RGB LED: R=PA12, G=PA11 (shared), B=PA14 per BW16 schematic.
// TODO(bench): confirm polarity and exact mapping.
#define PIN_LED   12u
#define LED_BUILTIN PIN_LED

// clang-format on
