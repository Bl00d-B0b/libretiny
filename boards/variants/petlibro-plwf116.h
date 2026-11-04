/* Petlibro PLWF116 (RTL8721DM). PA_n = n, PB_n = 32 + n.
 * Superset of GPIOs for pin discovery; excludes SPI flash PB12-PB17 and flash power PB22. */

#pragma once

// clang-format off

// Pins
// ----
#define PINS_COUNT         57 // Total GPIO count
#define NUM_DIGITAL_PINS   57 // Digital inputs/outputs
#define NUM_ANALOG_INPUTS  3  // ADC inputs
#define NUM_ANALOG_OUTPUTS 0  // PWM & DAC outputs
#define PINS_GPIO_MAX      63 // Last usable GPIO number

// Analog pins
// -----------
#define PIN_A0	 33u // PB1
#define PIN_A1	 34u // PB2
#define PIN_A2	 35u // PB3
#define PIN_ADC0 33u // PB1
#define PIN_ADC1 34u // PB2
#define PIN_ADC2 35u // PB3
#define PINS_ADC {33u, 34u, 35u}

// Serial ports
// ------------
// LOG_UART on PA7 (TX) / PA8 (RX) is the console and download port.
#define HAS_SERIAL0 1
#define PIN_SERIAL0_TX 7u // PA7
#define PIN_SERIAL0_RX 8u // PA8
#define PINS_SERIAL0_TX {7u}
#define PINS_SERIAL0_RX {8u}
#define SERIAL_INTERFACES_COUNT 1

// Pin names
// ---------
#define PIN_PA0   0u
#define PIN_PA1   1u
#define PIN_PA2   2u
#define PIN_PA3   3u
#define PIN_PA4   4u
#define PIN_PA5   5u
#define PIN_PA6   6u
#define PIN_PA7   7u
#define PIN_PA8   8u
#define PIN_PA9   9u
#define PIN_PA10  10u
#define PIN_PA11  11u
#define PIN_PA12  12u
#define PIN_PA13  13u
#define PIN_PA14  14u
#define PIN_PA15  15u
#define PIN_PA16  16u
#define PIN_PA17  17u
#define PIN_PA18  18u
#define PIN_PA19  19u
#define PIN_PA20  20u
#define PIN_PA21  21u
#define PIN_PA22  22u
#define PIN_PA23  23u
#define PIN_PA24  24u
#define PIN_PA25  25u
#define PIN_PA26  26u
#define PIN_PA27  27u
#define PIN_PA28  28u
#define PIN_PA29  29u
#define PIN_PA30  30u
#define PIN_PA31  31u
#define PIN_PB0   32u
#define PIN_PB1   33u
#define PIN_PB2   34u
#define PIN_PB3   35u
#define PIN_PB4   36u
#define PIN_PB5   37u
#define PIN_PB6   38u
#define PIN_PB7   39u
#define PIN_PB8   40u
#define PIN_PB9   41u
#define PIN_PB10  42u
#define PIN_PB11  43u
#define PIN_PB18  50u
#define PIN_PB19  51u
#define PIN_PB20  52u
#define PIN_PB21  53u
#define PIN_PB23  55u
#define PIN_PB24  56u
#define PIN_PB25  57u
#define PIN_PB26  58u
#define PIN_PB27  59u
#define PIN_PB28  60u
#define PIN_PB29  61u
#define PIN_PB30  62u
#define PIN_PB31  63u

// Pin function macros
// -------------------
#define PINS_GPIO {0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u, 19u, 20u, 21u, 22u, 23u, 24u, 25u, 26u, 27u, 28u, 29u, 30u, 31u, 32u, 33u, 34u, 35u, 36u, 37u, 38u, 39u, 40u, 41u, 42u, 43u, 50u, 51u, 52u, 53u, 55u, 56u, 57u, 58u, 59u, 60u, 61u, 62u, 63u}

// clang-format on
