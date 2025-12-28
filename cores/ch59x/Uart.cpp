/*
  Copyright (c) 2015 Arduino LLC.  All right reserved.
  Copyright (c) 2016 Sandeep Mistry All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Uart.h"
#include "Arduino.h"
#include "wiring_private.h"
#include "ch32fun/ch5xxhw.h"

Uart::Uart(uint32_t uartPort) {
	this->uartPort = uartPort;
}

void Uart::begin(unsigned long baudrate) {
	begin(baudrate, (uint8_t)SERIAL_8N1);
}

void Uart::begin(unsigned long baudrate, uint16_t /*config*/) {
	// if (uartPort == R32_UART0_CTRL) {
	// 	funPinMode(PB4, GPIO_ModeIN_PU); // RX0 (PB4)
	// 	funPinMode(PB7, GPIO_ModeOut_PP_5mA); // TX0 (PB7)
	// 	printf("UART0 initialized\r\n");
	// }
	// else if (uartPort == R32_UART1_CTRL) {
	// 	funPinMode(PA8, GPIO_ModeIN_PU); // RX1 (PA8)
	// 	funPinMode(PA9, GPIO_ModeOut_PP_5mA); // TX1 (PA9)
	// 	printf("UART1 initialized\r\n");
	// }
	// else if (uartPort == R32_UART2_CTRL) {
	// 	funPinMode(PA6, GPIO_ModeIN_PU); // RX2 (PA6)
	// 	funPinMode(PA7, GPIO_ModeOut_PP_5mA); // TX2 (PA7)
	// 	printf("UART2 initialized\r\n");
	// }
	// else if (uartPort == R32_UART3_CTRL) {
	// 	funPinMode(PA4, GPIO_ModeIN_PU); // RX3 (PA4)
	// 	funPinMode(PA5, GPIO_ModeOut_PP_5mA); // TX3 (PA5)
	// 	printf("UART3 initialized\r\n");
	// }
	// else {
	// 	return; // Invalid UART
	// }

	//# TXD enabled
	GET_REG8(uartPort, UART_IER) = RB_IER_TXD_EN;

	//# FIFO Control register
	GET_REG8(uartPort, UART_FCR) = RB_FCR_FIFO_EN | RB_FCR_TX_FIFO_CLR | RB_FCR_RX_FIFO_CLR |
		// Trigger points select of receiving FIFO: 4 bytes
		(0b10 << 6);

	//# Line Control register
	GET_REG8(uartPort, UART_LCR) = RB_LCR_WORD_SZ; // word length: 8 bits

	//# Baud rate = Fsys * 2 / R8_UART0_DIV / 16 / R16_UART0_DL
	uint8_t divider                    = 1;
	GET_REG16(uartPort, UART_DLL) = FUNCONF_SYSTEM_CORE_CLOCK / (8 * baudrate * divider);

	//# Prescaler divisor
	GET_REG8(uartPort, UART_DIV) = divider;
}

void Uart::end() {
	// TODO: implement this
	rxBuffer.clear();
}

void Uart::flush() {
}

void Uart::IrqHandler() {
	// TODO: implement RX interrupt handling
	// if (nrfUart->EVENTS_RXDRDY) {
	// 	rxBuffer.store_char(nrfUart->RXD);
	//
	// 	nrfUart->EVENTS_RXDRDY = 0x0UL;
	// }
}

int Uart::available() {
	return rxBuffer.available();
}

int Uart::peek() {
	return rxBuffer.peek();
}

int Uart::read() {
	return rxBuffer.read_char();
}

size_t Uart::write(const uint8_t data) {
	while (!(GET_REG8(uartPort, UART_LSR) & RB_LSR_TX_ALL_EMP));
	GET_REG8(uartPort, UART_THR) = data;
	return 1;
}

// Uart Serial(&R32_UART0_CTRL);
// extern "C" {
// void UART1_IRQHandler() {
// void UART2_IRQHandler() {
// void UART3_IRQHandler() {
// void UART0_IRQHandler() {
	// SERIAL_PORT_HARDWARE.IrqHandler();
// }
// }
