/* Copyright (c) Bl00d-B0b 2026-08-07. */

#if LT_ARD_HAS_SERIAL || DOXYGEN

#include "SerialPrivate.h"

// AmebaD's KM4 UARTs: UART0 (PA), UART1 (BT), UART2 is the KM0-owned LOGUART.
// The IRQ table and register access mirror AmebaZ; the differences are the
// irq_register()/irq_enable() naming and the per-port clock enable.

static u32 callback(void *param) {
	SerialData *data   = (SerialData *)param;
	UART_TypeDef *uart = data->uart;

	u32 intcr		= uart->DLH_INTCR;
	uart->DLH_INTCR = 0;

	u8 c;
	while (UART_Readable(uart)) {
		UART_CharGet(uart, &c);
		data->buf->store_char(c);
	}

	uart->DLH_INTCR = intcr;
	return 0;
}

void SerialClass::beginPrivate(unsigned long baudrate, uint16_t config) {
	if (!this->data)
		return;
	this->data->buf	 = this->rxBuf;
	this->data->uart = UART_DEV_TABLE[this->port].UARTx;
	this->data->irq	 = UART_DEV_TABLE[this->port].IrqNum;

	switch (this->port) {
		case 0:
			RCC_PeriphClockCmd(APBPeriph_UART0, APBPeriph_UART0_CLOCK, ENABLE);
			break;
		case 1:
			RCC_PeriphClockCmd(APBPeriph_UART1, APBPeriph_UART1_CLOCK, ENABLE);
			break;
		default:
			// UART2 is the LOGUART, brought up by the ROM
			break;
	}

	if (this->tx != PIN_INVALID)
		Pinmux_Config(this->tx, PINMUX_FUNCTION_UART);
	if (this->rx != PIN_INVALID) {
		Pinmux_Config(this->rx, PINMUX_FUNCTION_UART);
		PAD_PullCtrl(this->rx, GPIO_PuPd_UP);

		UART_TypeDef *uart = this->data->uart;
		IRQn_Type irq	   = this->data->irq;
		InterruptUnRegister(irq);
		InterruptRegister((IRQ_FUN)callback, irq, (u32)this->data, 10);
		InterruptEn(irq, 10);
		UART_RxCmd(uart, ENABLE);
		UART_INTConfig(uart, RUART_IER_ERBI, ENABLE);
	}
}

void SerialClass::configure(unsigned long baudrate, uint16_t config) {
	if (!this->data)
		return;
	UART_TypeDef *uart = this->data->uart;

	UART_InitTypeDef cfg;
	UART_StructInit(&cfg);
	cfg.WordLen	   = (config & SERIAL_DATA_MASK) == SERIAL_DATA_8;
	cfg.Parity	   = (config & SERIAL_PARITY_MASK) != SERIAL_PARITY_NONE;
	cfg.ParityType = (config & SERIAL_PARITY_MASK) == SERIAL_PARITY_EVEN;
	cfg.StopBit	   = (config & SERIAL_STOP_BIT_MASK) == SERIAL_STOP_BIT_2;
	UART_Init(uart, &cfg);
	UART_SetBaud(uart, baudrate);

	this->baudrate = baudrate;
	this->config   = config;
}

void SerialClass::endPrivate() {
	if (!this->data || !this->data->uart)
		return;
	UART_TypeDef *uart = this->data->uart;

	UART_INTConfig(uart, RUART_IER_ERBI, DISABLE);
	UART_RxCmd(uart, DISABLE);
	InterruptDis(this->data->irq);
	InterruptUnRegister(this->data->irq);
	UART_DeInit(uart);
}

void SerialClass::flush() {
	if (!this->data)
		return;
	UART_WaitBusy(this->data->uart, 10);
}

size_t SerialClass::write(uint8_t c) {
	if (!this->data)
		return 0;
	while (UART_Writable(this->data->uart) == 0) {}
	UART_CharPut(this->data->uart, c);
	return 1;
}

#endif
