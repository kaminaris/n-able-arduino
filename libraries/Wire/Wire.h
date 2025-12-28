/*
 * TWI/I2C library for mRF5x
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

#ifndef TwoWire_h
#define TwoWire_h

#include "Stream.h"
// #include "variant.h"

#include "RingBuffer.h"

// WIRE_HAS_END means Wire has end()
#define WIRE_HAS_END 1
#define I2C_DEFAULT_TIMEOUT 100000
#define SYSTEM_CLOCK_MHZ (FUNCONF_SYSTEM_CORE_CLOCK/1000000)

#define BUF_MAKE_U16(buff) ((buff[0] << 8) | buff[1])

class TwoWire: public Stream {
public:
	TwoWire();
	void begin(uint16_t i2c_speed_khz);
	void end();
	void setClock(uint32_t);

	void beginTransmission(uint8_t);
	uint8_t endTransmission(bool stopBit);
	uint8_t endTransmission(void);

	uint8_t requestFrom(uint8_t address, size_t quantity, bool stopBit);
	uint8_t requestFrom(uint8_t address, size_t quantity);

	size_t write(uint8_t data);
	size_t write(const uint8_t* data, size_t quantity);

	virtual int available(void);
	virtual int read(void);
	virtual int peek(void);
	virtual void flush(void);
	void onReceive(void (* )(int));
	void onRequest(void (* )(void));
	void onService(void);

	using Print::write;

private:

	uint8_t _uc_pinSDA;
	uint8_t _uc_pinSCL;

	bool master;
	bool receiving;
	bool transmissionBegun;
	bool suspended;

	// RX Buffer
	RingBuffer rxBuffer;

	// TX buffer
	RingBuffer txBuffer;
	uint8_t txAddress;

	// Callback user functions
	void (* onRequestCallback)(void);
	void (* onReceiveCallback)(int);

	// TWI clock frequency
	static const uint32_t TWI_CLOCK = 100000;
};

extern TwoWire Wire;

#endif
