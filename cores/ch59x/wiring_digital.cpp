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

#include "Arduino.h"

#ifdef __cplusplus
extern "C" {


#endif
// CH592 GPIO definitions
#define PB 0x80000000  // Bit flag for GPIO B
#define OFFSET_FOR_GPIOB(pin) (((pin & PB) >> 31) * (&R32_PB_PIN - &R32_PA_PIN))

// funPinMode implementation
void funPinMode(uint32_t pin, GPIOModeTypeDef mode) {
	switch (mode) {
		case GPIO_ModeIN_Floating:
			*(&R32_PA_PD_DRV + OFFSET_FOR_GPIOB(pin)) &= ~(pin & ~PB);
			*(&R32_PA_PU + OFFSET_FOR_GPIOB(pin))  &= ~(pin & ~PB);
			*(&R32_PA_DIR + OFFSET_FOR_GPIOB(pin)) &= ~(pin & ~PB);
			break;
		case GPIO_ModeOut_PP_5mA:
			*(&R32_PA_PD_DRV + OFFSET_FOR_GPIOB(pin)) &= ~(pin & ~PB);
			*(&R32_PA_DIR + OFFSET_FOR_GPIOB(pin)) |= (pin & ~PB);
			break;
		case GPIO_ModeOut_PP_20mA:
			*(&R32_PA_PD_DRV + OFFSET_FOR_GPIOB(pin)) |= (pin & ~PB);
			*(&R32_PA_DIR + OFFSET_FOR_GPIOB(pin)) |= (pin & ~PB);
			break;
	}
}

// funDigitalWrite implementation
#if defined(CH571_CH573) || defined(CH582_CH583)
#define GPIO_SetBits(pin)   (*(&R32_PA_OUT + OFFSET_FOR_GPIOB(pin)) |= (pin & ~PB))
#else
#define GPIO_SetBits(pin)   (*(&R32_PA_SET + OFFSET_FOR_GPIOB(pin)) = (pin & ~PB))
#endif
#define GPIO_ResetBits(pin) (*(&R32_PA_CLR + OFFSET_FOR_GPIOB(pin)) = (pin & ~PB))

#define funDigitalWrite(pin, value) \
do { \
if ((value) == FUN_HIGH) { GPIO_SetBits(pin); } \
else if ((value) == FUN_LOW) { GPIO_ResetBits(pin); } \
} while(0)

#define FUN_HIGH 0x1
#define FUN_LOW  0x0

void pinMode(uint32_t ulPin, uint32_t ulMode) {
	// Set pin mode according to chapter '22.6.3 I/O Pin Configuration'
	switch (ulMode) {
		case INPUT:
			// Set pin to input mode
			funPinMode(ulPin, GPIO_ModeIN_Floating);
			break ;

		case INPUT_PULLUP:
			// Set pin to input mode with pull-up resistor enabled
			funPinMode(ulPin, GPIO_ModeIN_PU);
			break ;

		case INPUT_PULLDOWN:
			// Set pin to input mode with pull-down resistor enabled
			funPinMode(ulPin, GPIO_ModeIN_PD);
			break ;

		case OUTPUT:
			// Set pin to output mode
			funPinMode(ulPin, GPIO_ModeOut_PP_20mA);
			break ;

		default:
			// do nothing
			break ;
	}
}

void digitalWrite(uint32_t ulPin, uint32_t ulVal) {
	funDigitalWrite(ulPin, ulVal ? FUN_HIGH : FUN_LOW);
}

int digitalRead(uint32_t ulPin) {
	// return funDigitalRead(ulPin);
}

void digitalToggle(uint32_t ulPin) {
	// funDigitalWrite(ulPin, FUN_HIGH); //TODO: implement toggle
}

#ifdef LED_STATE_ON
void ledOn(uint32_t pin) {
	digitalWrite(pin, LED_STATE_ON);
}

void ledOff(uint32_t pin) {
	digitalWrite(pin, 1 - LED_STATE_ON);
}
#endif

#ifdef __cplusplus
}
#endif
