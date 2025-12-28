/*
 * TWI/I2C library for nRF5x
 * Copyright (c) 2015 Arduino LLC. All rights reserved.
 * Copyright (c) 2016 Sandeep Mistry All right reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */


extern "C" {
#include <string.h>
}
#include "ch32fun/ch5xxhw.h"
// #include <ch32fun/ch32fun.hrx>
#include <Arduino.h>
#include <wiring_private.h>

#include "Wire.h"

u8 i2c_get_error() {
	if (R16_I2C_STAR1 & RB_I2C_BERR)	{ R16_I2C_STAR1 &= ~RB_I2C_BERR;	return 1; }		// Bus Error
	if (R16_I2C_STAR1 & RB_I2C_ARLO)	{ R16_I2C_STAR1 &= ~RB_I2C_ARLO;	return 2; }		// Arbitration lost
	if (R16_I2C_STAR1 & RB_I2C_AF)		{ R16_I2C_STAR1 &= ~RB_I2C_AF;		return 3; }		// ACK Failure
	if (R16_I2C_STAR1 & RB_I2C_OVR)		{ R16_I2C_STAR1 &= ~RB_I2C_OVR;		return 4; }		// Overrun
	if (R16_I2C_STAR1 & RB_I2C_PECERR)	{ R16_I2C_STAR1 &= ~RB_I2C_PECERR;	return 5; }		// PEC Error
	return 0;
}

TwoWire::TwoWire() {
	this->transmissionBegun = false;
	this->suspended         = false;
}

void TwoWire::begin(uint16_t i2c_speed_khz) {
	//! REQUIRED: Software reset
	R16_I2C_CTRL1 |= RB_I2C_SWRST;
	R16_I2C_CTRL1 &= ~RB_I2C_SWRST;

	// Clear FREQ bits and set system clock frequency
	R16_I2C_CTRL2 &= ~RB_I2C_FREQ;
	R16_I2C_CTRL2 |= (SYSTEM_CLOCK_MHZ & RB_I2C_FREQ);

	// Clear pheriphera
	R16_I2C_CTRL1 &= ~RB_I2C_PE;
	u16 clock_config;

	if (i2c_speed_khz <= 100) {
		R16_I2C_RTR = (1 + SYSTEM_CLOCK_MHZ) > 0x3F ? 0x3F : (1 + SYSTEM_CLOCK_MHZ);

		clock_config = (SYSTEM_CLOCK_MHZ * 1000 / (i2c_speed_khz << 1)) & RB_I2C_CCR;
		if (clock_config < 4) clock_config = 4;
		printf("clock_config: %d\r\n", clock_config);
		printf("RTR: %d\r\n", R16_I2C_RTR);
	}
	else {
		R16_I2C_RTR = 1 + SYSTEM_CLOCK_MHZ * 300 / 1000;

		clock_config = (SYSTEM_CLOCK_MHZ * 1000 / (i2c_speed_khz * 3)) & RB_I2C_CCR;
		if (clock_config == 0) clock_config = 1;
		clock_config |= RB_I2C_F_S;
	}

	// Set CCR value
	R16_I2C_CKCFGR = clock_config;

	// Enable peripheral
	R16_I2C_CTRL1 = RB_I2C_PE;
	R16_I2C_CTRL1 |= RB_I2C_ACK;	// Enable ACK

	// return i2c_get_error();
}

void TwoWire::setClock(uint32_t baudrate) {
	// _p_twi->ENABLE = (TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos);
	//
	// uint32_t frequency;
	//
	// if (baudrate <= 100000) {
	// 	frequency = TWI_FREQUENCY_FREQUENCY_K100;
	// }
	// else if (baudrate <= 250000) {
	// 	frequency = TWI_FREQUENCY_FREQUENCY_K250;
	// }
	// else {
	// 	frequency = TWI_FREQUENCY_FREQUENCY_K400;
	// }
	//
	// _p_twi->FREQUENCY = (frequency << TWI_FREQUENCY_FREQUENCY_Pos);
	// _p_twi->ENABLE    = (TWI_ENABLE_ENABLE_Enabled << TWI_ENABLE_ENABLE_Pos);
}

void TwoWire::end() {
	// _p_twi->ENABLE = (TWI_ENABLE_ENABLE_Disabled << TWI_ENABLE_ENABLE_Pos);
}

uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity, bool stopBit) {
	// if (quantity == 0) {
	// 	return 0;
	// }
	// if (quantity > SERIAL_BUFFER_SIZE) {
	// 	quantity = SERIAL_BUFFER_SIZE;
	// }
	//
	size_t byteRead = 0;
	// rxBuffer.clear();
	//
	// _p_twi->ADDRESS = address;
	// _p_twi->SHORTS  = 0x1UL; // To trigger suspend task when a byte is received
	//
	// if (!this->suspended) {
	// 	_p_twi->TASKS_RESUME  = 0x1UL;
	// 	_p_twi->TASKS_STARTRX = 0x1UL;
	// }
	//
	// for (byteRead = 0; byteRead < quantity; byteRead++) {
	// 	if (byteRead == quantity - 1) {
	// 		// To trigger stop task when last byte is received, set before resume task.
	// 		if (stopBit) {
	// 			_p_twi->SHORTS = 0x2UL;
	// 		}
	// 	}
	//
	// 	_p_twi->TASKS_RESUME = 0x1UL;
	//
	// 	while (!_p_twi->EVENTS_RXDREADY && !_p_twi->EVENTS_ERROR);
	//
	// 	if (_p_twi->EVENTS_ERROR) {
	// 		break;
	// 	}
	//
	// 	_p_twi->EVENTS_RXDREADY = 0x0UL;
	//
	// 	rxBuffer.store_char(_p_twi->RXD);
	// }
	//
	// if (stopBit || _p_twi->EVENTS_ERROR) {
	// 	this->suspended    = false;
	// 	_p_twi->TASKS_STOP = 0x1UL;
	// 	while (!_p_twi->EVENTS_STOPPED);
	// 	_p_twi->EVENTS_STOPPED = 0x0UL;
	// }
	// else {
	// 	this->suspended       = true;
	// 	_p_twi->TASKS_SUSPEND = 0x1UL;
	// 	while (!_p_twi->EVENTS_SUSPENDED);
	// 	_p_twi->EVENTS_SUSPENDED = 0x0UL;
	// }
	//
	// if (_p_twi->EVENTS_ERROR) {
	// 	_p_twi->EVENTS_ERROR = 0x0UL;
	// }

	return byteRead;
}

uint8_t TwoWire::requestFrom(uint8_t address, size_t quantity) {
	return requestFrom(address, quantity, true);
}

void TwoWire::beginTransmission(uint8_t address) {
	// save address of target and clear buffer
	txAddress = address;
	txBuffer.clear();

	transmissionBegun = true;
}

// Errors:
//  0 : Success
//  1 : Data too long
//  2 : NACK on transmit of address
//  3 : NACK on transmit of data
//  4 : Other error
uint8_t TwoWire::endTransmission(bool stopBit) {
	transmissionBegun = false;

	// Start I2C transmission
	// _p_twi->ADDRESS       = txAddress;
	// _p_twi->SHORTS        = 0x0UL;
	// _p_twi->TASKS_RESUME  = 0x1UL;
	// _p_twi->TASKS_STARTTX = 0x1UL;
	//
	// while (txBuffer.available()) {
	// 	_p_twi->TXD = txBuffer.read_char();
	//
	// 	while (!_p_twi->EVENTS_TXDSENT && !_p_twi->EVENTS_ERROR);
	//
	// 	if (_p_twi->EVENTS_ERROR) {
	// 		break;
	// 	}
	//
	// 	_p_twi->EVENTS_TXDSENT = 0x0UL;
	// }
	//
	// if (stopBit || _p_twi->EVENTS_ERROR) {
	// 	_p_twi->TASKS_STOP = 0x1UL;
	// 	while (!_p_twi->EVENTS_STOPPED);
	// 	_p_twi->EVENTS_STOPPED = 0x0UL;
	// }
	// else {
	// 	_p_twi->TASKS_SUSPEND = 0x1UL;
	// 	while (!_p_twi->EVENTS_SUSPENDED);
	// 	_p_twi->EVENTS_SUSPENDED = 0x0UL;
	// }
	//
	// if (_p_twi->EVENTS_ERROR) {
	// 	_p_twi->EVENTS_ERROR = 0x0UL;
	//
	// 	uint32_t error = _p_twi->ERRORSRC;
	//
	// 	_p_twi->ERRORSRC = error;
	//
	// 	if (error == TWI_ERRORSRC_ANACK_Msk) {
	// 		return 2;
	// 	}
	// 	else if (error == TWI_ERRORSRC_DNACK_Msk) {
	// 		return 3;
	// 	}
	// 	else {
	// 		return 4;
	// 	}
	// }

	return 0;
}

uint8_t TwoWire::endTransmission() {
	return endTransmission(true);
}

size_t TwoWire::write(uint8_t ucData) {
	// No writing, without begun transmission or a full buffer
	if (!transmissionBegun || txBuffer.isFull()) {
		return 0;
	}

	txBuffer.store_char(ucData);

	return 1;
}

size_t TwoWire::write(const uint8_t* data, size_t quantity) {
	//Try to store all data
	for (size_t i = 0; i < quantity; ++i) {
		//Return the number of data stored, when the buffer is full (if write return 0)
		if (!write(data[i]))
			return i;
	}

	//All data stored
	return quantity;
}

int TwoWire::available(void) {
	return rxBuffer.available();
}

int TwoWire::read(void) {
	return rxBuffer.read_char();
}

int TwoWire::peek(void) {
	return rxBuffer.peek();
}

void TwoWire::flush(void) {
	// Do nothing, use endTransmission(..) to force
	// data transfer.
}

TwoWire Wire();