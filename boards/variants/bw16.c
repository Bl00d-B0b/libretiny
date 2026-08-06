/* BW16 (RTL8720DN) — hand-written first pass; regenerate with boardgen. */

#include <Arduino.h>

#ifdef LT_VARIANT_INCLUDE
#include LT_VARIANT_INCLUDE
#endif

// clang-format off
PinInfo lt_arduino_pin_info_list[PINS_COUNT] = {
	// D0: PA7, LOG_RX
	{7u,  PIN_GPIO | PIN_IRQ | PIN_UART, PIN_NONE, 0},
	// D1: PA8, LOG_TX
	{8u,  PIN_GPIO | PIN_IRQ | PIN_UART, PIN_NONE, 0},
	// D2: PA9
	{9u,  PIN_GPIO | PIN_IRQ,            PIN_NONE, 0},
	// D3: PA12, LED_R, PWM
	{12u, PIN_GPIO | PIN_IRQ | PIN_PWM,  PIN_NONE, 0},
	// D4: PA13, PWM
	{13u, PIN_GPIO | PIN_IRQ | PIN_PWM,  PIN_NONE, 0},
	// D5: PA14, LED_B, SWD_CLK
	{14u, PIN_GPIO | PIN_IRQ | PIN_SWD,  PIN_NONE, 0},
	// D6: PA25, I2C
	{25u, PIN_GPIO | PIN_IRQ | PIN_I2C,  PIN_NONE, 0},
	// D7: PA26, I2C
	{26u, PIN_GPIO | PIN_IRQ | PIN_I2C,  PIN_NONE, 0},
	// D8: PA27, SWD_DATA
	{27u, PIN_GPIO | PIN_IRQ | PIN_SWD,  PIN_NONE, 0},
	// D9: PA30, PWM
	{30u, PIN_GPIO | PIN_IRQ | PIN_PWM,  PIN_NONE, 0},
	// D10: PB1, UART2_RX
	{33u, PIN_GPIO | PIN_IRQ | PIN_UART, PIN_NONE, 0},
	// D11: PB2, UART2_TX
	{34u, PIN_GPIO | PIN_IRQ | PIN_UART, PIN_NONE, 0},
	// D12: PB3, SPI/PWM
	{35u, PIN_GPIO | PIN_IRQ | PIN_SPI,  PIN_NONE, 0},
};
// clang-format on
