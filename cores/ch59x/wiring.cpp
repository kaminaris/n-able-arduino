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
#include <stdio.h>

#ifdef __cplusplus
extern "C" {


#endif

#ifndef CONFIG_WDT_TIMEOUT_SECONDS
#define CONFIG_WDT_TIMEOUT_SECONDS 0
#endif
#define sys_safe_access_enable()        do{volatile uint32_t mpie_mie;mpie_mie=__risc_v_disable_irq();SAFEOPERATE;\
                                        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;SAFEOPERATE;

#define sys_safe_access_disable()       R8_SAFE_ACCESS_SIG = 0;__risc_v_enable_irq(mpie_mie);SAFEOPERATE;}while(0)

static uint32_t _resetReason;

// SYS_CLKTypeDef sc = CLK_SOURCE_CH5XX;
// SYS_SAFE_ACCESS(
// 	R8_PLL_CONFIG &= ~(1 << 5);
// );
// if(sc & 0x20)  // HSE div
// {
// 	SYS_SAFE_ACCESS(
// 		R32_CLK_SYS_CFG = (0 << 6) | (sc & 0x1f) | RB_TX_32M_PWR_EN | RB_PLL_PWR_EN;
// 	);
// 	ADD_N_NOPS(4);
// 	SYS_SAFE_ACCESS(
// 		R8_FLASH_CFG = 0x51;
// 	);
// }
// else if(sc & 0x40) // PLL div
// {
// 	SYS_SAFE_ACCESS(
// 		R32_CLK_SYS_CFG = (1 << 6) | (sc & 0x1f) | RB_TX_32M_PWR_EN | RB_PLL_PWR_EN;
// 	);
// 	ADD_N_NOPS(4);
// 	SYS_SAFE_ACCESS(
// 		R8_FLASH_CFG = 0x52;
// 	);
// }
// else
// {
// 	SYS_SAFE_ACCESS(
// 		R32_CLK_SYS_CFG |= RB_CLK_SYS_MOD;
// 	);
// }
//
// SYS_SAFE_ACCESS(
// 	R8_PLL_CONFIG |= 1 << 7;
// );

__HIGH_CODE
void SetSysClock(SYS_CLKTypeDef sc) {
	SYS_SAFE_ACCESS(
		R8_PLL_CONFIG &= ~(1 << 5); //
	);
	if (sc & 0x20) // HSE div
	{
		SYS_SAFE_ACCESS(
			R32_CLK_SYS_CFG = (0 << 6) | (sc & 0x1f) | RB_TX_32M_PWR_EN | RB_PLL_PWR_EN;
			portNOP();
			portNOP();
			portNOP();
			portNOP();
		);
		SYS_SAFE_ACCESS(
			// SAFEOPERATE;
			R8_FLASH_CFG = 0X51;
		);
	}

	else if (sc & 0x40) // PLL div
	{
		SYS_SAFE_ACCESS(
			R32_CLK_SYS_CFG = (1 << 6) | (sc & 0x1f) | RB_TX_32M_PWR_EN | RB_PLL_PWR_EN;
			portNOP();
			portNOP();
			portNOP();
			portNOP();
		);
		SYS_SAFE_ACCESS(
			R8_FLASH_CFG = 0X52;
		);
	}
	else {
		SYS_SAFE_ACCESS(
			R32_CLK_SYS_CFG |= RB_CLK_SYS_MOD;
		);
	}
	//����FLASH clk����������
	SYS_SAFE_ACCESS(
		R8_PLL_CONFIG |= 1 << 7;
	);
}

void init(void) {
	// SystemInit();
#if(defined(DCDC_ENABLE)) && (DCDC_ENABLE == TRUE)
	DCDCEnable();
#endif
	SetSysClock(CLK_SOURCE_PLL_60MHz);
#if(defined(HAL_SLEEP)) && (HAL_SLEEP == TRUE)
	GPIOA_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU);
	GPIOB_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU);
#endif
#ifdef DEBUG
	GPIOA_SetBits(bTXD1);
	GPIOA_ModeCfg(bTXD1, GPIO_ModeOut_PP_5mA);
	UART1_DefInit();
#endif
}

uint32_t getResetReason(void) {
	return _resetReason;
}

void systemPowerOff(void) {
	// nrf_power_system_off(NRF_POWER);
}

void systemRestart(void) {
	// NVIC_SystemReset();
}

#ifdef __cplusplus
}
#endif
