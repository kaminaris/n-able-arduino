/*
  Copyright (c) 2021 Ryan Powell.  All right reserved.

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

#include <Arduino.h>
#include <stdlib.h>
#include "debug_utils.h"

struct exception_frame {
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r12;
	uint32_t lr;
	uint32_t pc;
	uint32_t psr;
};



// extern "C" void NMI_Handler(void) {}
extern "C" void HardFault_Handler(void) {
	volatile uint32_t mcause  = __get_MCAUSE();
	volatile uint32_t mepc    = __get_MEPC();
	volatile uint32_t mtval   = __get_MTVAL();
	volatile uint32_t mstatus = __get_MSTATUS();
	volatile uint32_t x1      = (*(uint32_t*)0x26720);
	volatile uint32_t x2      = (*(uint32_t*)0x26724);

	// PrintHex(mepc);
	// PrintHex(mstatus);
	// PrintHex(mtval);
	// PrintHex(mcause);
	// PrintHex(x1);
	// PrintHex(x2);

	uint8_t* m;
	for (m = mepc - 8; m < mepc + 8; m++) {
		PrintHex(m);
		PrintHex(*m);
	}

	volatile uint32_t isInterrupt = (mcause & 0x80000000) >> 31;
	volatile uint32_t causeCode   = mcause & 0x7FFFFFFF;

	// Set breakpoint on next line, then inspect variables
	while (1) {
	}
}
