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

// Maps a GPIO number to its PinInfo; indices not listed stay NULL.
PinInfo *lt_arduino_pin_gpio_map[PINS_GPIO_MAX + 1] = {
	[7] = &(lt_arduino_pin_info_list[0]), // D0
	[8] = &(lt_arduino_pin_info_list[1]), // D1
	[9] = &(lt_arduino_pin_info_list[2]), // D2
	[12] = &(lt_arduino_pin_info_list[3]), // D3
	[13] = &(lt_arduino_pin_info_list[4]), // D4
	[14] = &(lt_arduino_pin_info_list[5]), // D5
	[25] = &(lt_arduino_pin_info_list[6]), // D6
	[26] = &(lt_arduino_pin_info_list[7]), // D7
	[27] = &(lt_arduino_pin_info_list[8]), // D8
	[30] = &(lt_arduino_pin_info_list[9]), // D9
	[33] = &(lt_arduino_pin_info_list[10]), // D10
	[34] = &(lt_arduino_pin_info_list[11]), // D11
	[35] = &(lt_arduino_pin_info_list[12]), // D12
};

void lt_init_variant() {
	// no board-specific init on BW16 (no PSRAM, no external peripherals)
}
