/* Petlibro PLWF116 (RTL8721DM). See petlibro-plwf116.h. */

#include <Arduino.h>

#ifdef LT_VARIANT_INCLUDE
#include LT_VARIANT_INCLUDE
#endif

// clang-format off
PinInfo lt_arduino_pin_info_list[PINS_COUNT] = {
	// D0: PA0
	{0u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D1: PA1
	{1u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D2: PA2
	{2u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D3: PA3
	{3u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D4: PA4
	{4u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D5: PA5
	{5u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D6: PA6
	{6u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D7: PA7, LOG_TX
	{7u, PIN_GPIO | PIN_IRQ | PIN_UART, PIN_NONE, 0},
	// D8: PA8, LOG_RX
	{8u, PIN_GPIO | PIN_IRQ | PIN_UART, PIN_NONE, 0},
	// D9: PA9
	{9u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D10: PA10
	{10u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D11: PA11
	{11u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D12: PA12
	{12u, PIN_GPIO | PIN_IRQ | PIN_PWM, PIN_NONE, 0},
	// D13: PA13
	{13u, PIN_GPIO | PIN_IRQ | PIN_PWM, PIN_NONE, 0},
	// D14: PA14, radar IRQ in vendor fw? (GPIO[14]); no PWM
	{14u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D15: PA15
	{15u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D16: PA16
	{16u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D17: PA17
	{17u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D18: PA18
	{18u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D19: PA19
	{19u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D20: PA20
	{20u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D21: PA21
	{21u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D22: PA22
	{22u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D23: PA23
	{23u, PIN_GPIO | PIN_IRQ | PIN_PWM, PIN_NONE, 0},
	// D24: PA24
	{24u, PIN_GPIO | PIN_IRQ | PIN_PWM, PIN_NONE, 0},
	// D25: PA25
	{25u, PIN_GPIO | PIN_IRQ | PIN_PWM, PIN_NONE, 0},
	// D26: PA26
	{26u, PIN_GPIO | PIN_IRQ | PIN_PWM, PIN_NONE, 0},
	// D27: PA27, SWD_DATA, boot strap (normal_mode_sel)
	{27u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D28: PA28
	{28u, PIN_GPIO | PIN_IRQ | PIN_PWM, PIN_NONE, 0},
	// D29: PA29
	{29u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D30: PA30
	{30u, PIN_GPIO | PIN_IRQ | PIN_PWM, PIN_NONE, 0},
	// D31: PA31
	{31u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D32: PB0
	{32u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D33: PB1, A0
	{33u, PIN_GPIO | PIN_IRQ | PIN_ADC, PIN_NONE, 0},
	// D34: PB2, A1, battery ADC in vendor fw (analogin_init [0x22:5])
	{34u, PIN_GPIO | PIN_IRQ | PIN_ADC, PIN_NONE, 0},
	// D35: PB3, A2, SWD_CLK
	{35u, PIN_GPIO | PIN_IRQ | PIN_ADC, PIN_NONE, 0},
	// D36: PB4
	{36u, PIN_GPIO | PIN_IRQ | PIN_PWM, PIN_NONE, 0},
	// D37: PB5
	{37u, PIN_GPIO | PIN_IRQ | PIN_PWM, PIN_NONE, 0},
	// D38: PB6
	{38u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D39: PB7
	{39u, PIN_GPIO | PIN_IRQ | PIN_PWM, PIN_NONE, 0},
	// D40: PB8
	{40u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D41: PB9
	{41u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D42: PB10
	{42u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D43: PB11
	{43u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D44: PB18
	{50u, PIN_GPIO | PIN_IRQ | PIN_PWM, PIN_NONE, 0},
	// D45: PB19
	{51u, PIN_GPIO | PIN_IRQ | PIN_PWM, PIN_NONE, 0},
	// D46: PB20
	{52u, PIN_GPIO | PIN_IRQ | PIN_PWM, PIN_NONE, 0},
	// D47: PB21
	{53u, PIN_GPIO | PIN_IRQ | PIN_PWM, PIN_NONE, 0},
	// D48: PB23
	{55u, PIN_GPIO | PIN_IRQ | PIN_PWM, PIN_NONE, 0},
	// D49: PB24
	{56u, PIN_GPIO | PIN_IRQ | PIN_PWM, PIN_NONE, 0},
	// D50: PB25
	{57u, PIN_GPIO | PIN_IRQ | PIN_PWM, PIN_NONE, 0},
	// D51: PB26
	{58u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D52: PB27
	{59u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D53: PB28
	{60u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D54: PB29
	{61u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D55: PB30
	{62u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
	// D56: PB31
	{63u, PIN_GPIO | PIN_IRQ, PIN_NONE, 0},
};
// clang-format on

// Maps a GPIO number to its PinInfo; indices not listed stay NULL.
PinInfo *lt_arduino_pin_gpio_map[PINS_GPIO_MAX + 1] = {
	[0] = &(lt_arduino_pin_info_list[0]), // PA0
	[1] = &(lt_arduino_pin_info_list[1]), // PA1
	[2] = &(lt_arduino_pin_info_list[2]), // PA2
	[3] = &(lt_arduino_pin_info_list[3]), // PA3
	[4] = &(lt_arduino_pin_info_list[4]), // PA4
	[5] = &(lt_arduino_pin_info_list[5]), // PA5
	[6] = &(lt_arduino_pin_info_list[6]), // PA6
	[7] = &(lt_arduino_pin_info_list[7]), // PA7
	[8] = &(lt_arduino_pin_info_list[8]), // PA8
	[9] = &(lt_arduino_pin_info_list[9]), // PA9
	[10] = &(lt_arduino_pin_info_list[10]), // PA10
	[11] = &(lt_arduino_pin_info_list[11]), // PA11
	[12] = &(lt_arduino_pin_info_list[12]), // PA12
	[13] = &(lt_arduino_pin_info_list[13]), // PA13
	[14] = &(lt_arduino_pin_info_list[14]), // PA14
	[15] = &(lt_arduino_pin_info_list[15]), // PA15
	[16] = &(lt_arduino_pin_info_list[16]), // PA16
	[17] = &(lt_arduino_pin_info_list[17]), // PA17
	[18] = &(lt_arduino_pin_info_list[18]), // PA18
	[19] = &(lt_arduino_pin_info_list[19]), // PA19
	[20] = &(lt_arduino_pin_info_list[20]), // PA20
	[21] = &(lt_arduino_pin_info_list[21]), // PA21
	[22] = &(lt_arduino_pin_info_list[22]), // PA22
	[23] = &(lt_arduino_pin_info_list[23]), // PA23
	[24] = &(lt_arduino_pin_info_list[24]), // PA24
	[25] = &(lt_arduino_pin_info_list[25]), // PA25
	[26] = &(lt_arduino_pin_info_list[26]), // PA26
	[27] = &(lt_arduino_pin_info_list[27]), // PA27
	[28] = &(lt_arduino_pin_info_list[28]), // PA28
	[29] = &(lt_arduino_pin_info_list[29]), // PA29
	[30] = &(lt_arduino_pin_info_list[30]), // PA30
	[31] = &(lt_arduino_pin_info_list[31]), // PA31
	[32] = &(lt_arduino_pin_info_list[32]), // PB0
	[33] = &(lt_arduino_pin_info_list[33]), // PB1
	[34] = &(lt_arduino_pin_info_list[34]), // PB2
	[35] = &(lt_arduino_pin_info_list[35]), // PB3
	[36] = &(lt_arduino_pin_info_list[36]), // PB4
	[37] = &(lt_arduino_pin_info_list[37]), // PB5
	[38] = &(lt_arduino_pin_info_list[38]), // PB6
	[39] = &(lt_arduino_pin_info_list[39]), // PB7
	[40] = &(lt_arduino_pin_info_list[40]), // PB8
	[41] = &(lt_arduino_pin_info_list[41]), // PB9
	[42] = &(lt_arduino_pin_info_list[42]), // PB10
	[43] = &(lt_arduino_pin_info_list[43]), // PB11
	[50] = &(lt_arduino_pin_info_list[44]), // PB18
	[51] = &(lt_arduino_pin_info_list[45]), // PB19
	[52] = &(lt_arduino_pin_info_list[46]), // PB20
	[53] = &(lt_arduino_pin_info_list[47]), // PB21
	[55] = &(lt_arduino_pin_info_list[48]), // PB23
	[56] = &(lt_arduino_pin_info_list[49]), // PB24
	[57] = &(lt_arduino_pin_info_list[50]), // PB25
	[58] = &(lt_arduino_pin_info_list[51]), // PB26
	[59] = &(lt_arduino_pin_info_list[52]), // PB27
	[60] = &(lt_arduino_pin_info_list[53]), // PB28
	[61] = &(lt_arduino_pin_info_list[54]), // PB29
	[62] = &(lt_arduino_pin_info_list[55]), // PB30
	[63] = &(lt_arduino_pin_info_list[56]), // PB31
};

void lt_init_variant() {
	// no board-specific init
}
