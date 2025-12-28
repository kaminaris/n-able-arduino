/********************************** (C) COPYRIGHT *******************************
 * File Name          : portmacro.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2023/10/25
 * Description        : WCH Qingke V4C FreeRTOS��ֲ���ӿ�
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef PORTMACRO_H
#define PORTMACRO_H
#include "ch32fun/ch5xxhw.h"

#ifdef __cplusplus
extern "C" {
#endif

// #include "core_riscv.h"
#include "ch32fun/ch5xxhw.h"

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the
 * given hardware and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portSTACK_TYPE	uint32_t
#define portBASE_TYPE	int32_t
#define portUBASE_TYPE	uint32_t
#define portMAX_DELAY ( TickType_t ) 0xffffffffUL


typedef portSTACK_TYPE StackType_t;
typedef portBASE_TYPE BaseType_t;
typedef portUBASE_TYPE UBaseType_t;
typedef portUBASE_TYPE TickType_t;

/* Legacy type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short

/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
not need to be guarded with a critical section. */
#define portTICK_TYPE_IS_ATOMIC 1
/*-----------------------------------------------------------*/

/* Architecture specifics. */
#define portSTACK_GROWTH			( -1 )
#define portTICK_PERIOD_MS			( ( TickType_t ) 1000 / configTICK_RATE_HZ )

#ifdef __riscv64
	#error This is the RV32 port that has not yet been adapted for 64.
	#define portBYTE_ALIGNMENT			16
#else
	#define portBYTE_ALIGNMENT			16
#endif
/*-----------------------------------------------------------*/

static  inline void PFIC_SetPendingIRQ(IRQn_Type IRQn)
{
	PFIC->IPSR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn)&0x1F));
}
static  inline void PFIC_SetPriority(IRQn_Type IRQn, uint8_t priority)
{
	PFIC->IPRIOR[(uint32_t)(IRQn)] = priority;
}
static  inline void PFIC_EnableIRQ(IRQn_Type IRQn)
{
	PFIC->IENR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn)&0x1F));
}
#define SysTick_LOAD_RELOAD_Msk    (0xFFFFFFFFFFFFFFFF)
#define SysTick_CTLR_SWIE          (1 << 31)
#define SysTick_CTLR_INIT          (1 << 5)
#define SysTick_CTLR_MODE          (1 << 4)
#define SysTick_CTLR_STRE          (1 << 3)
#define SysTick_CTLR_STCLK         (1 << 2)
#define SysTick_CTLR_STIE          (1 << 1)
#define SysTick_CTLR_STE           (1 << 0)

#define SysTick_SR_CNTIF           (1 << 0)
static  inline uint32_t SysTick_Config(uint64_t ticks)
{
	if((ticks - 1) > SysTick_LOAD_RELOAD_Msk)
		return (1); /* Reload value impossible */

	SysTick->CMP = ticks - 1; /* set reload register */
	PFIC_EnableIRQ(SysTick_IRQn);
	SysTick->CTLR = SysTick_CTLR_INIT |
					SysTick_CTLR_STRE |
					SysTick_CTLR_STCLK |
					SysTick_CTLR_STIE |
					SysTick_CTLR_STE; /* Enable SysTick IRQ and SysTick Timer */
	return (0);                       /* Function successful */
}
/* Scheduler utilities. */
extern void vTaskSwitchContext( void );
#define portYIELD()   PFIC_SetPendingIRQ(Software_IRQn)
#define portEND_SWITCHING_ISR( xSwitchRequired ) do { if( xSwitchRequired ) portYIELD(); } while( 0 )
#define portYIELD_FROM_ISR( x ) portEND_SWITCHING_ISR( x )
/*-----------------------------------------------------------*/


/* Critical section management. */
extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );
extern UBaseType_t uxGetCriticalNestingDepth(void);
extern portUBASE_TYPE xPortSetInterruptMask(void);
extern void vPortClearInterruptMask(portUBASE_TYPE uvalue);

#define portSET_INTERRUPT_MASK_FROM_ISR()  0
#define portCLEAR_INTERRUPT_MASK_FROM_ISR( uxSavedStatusValue )  ( void )(uxSavedStatusValue)
#define portDISABLE_INTERRUPTS()    __asm volatile( "csrc mstatus, 8" );__NOP();__NOP();__NOP()
#define portENABLE_INTERRUPTS()     __asm volatile( "csrs mstatus, 8" )
#define portENTER_CRITICAL()	vPortEnterCritical()
#define portEXIT_CRITICAL()		vPortExitCritical()

/*-------------------------------------------------------------*/

/* Architecture specific optimisations. */
#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
	#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#endif

#if( configUSE_PORT_OPTIMISED_TASK_SELECTION == 1 )

	/* Check the configuration. */
	#if( configMAX_PRIORITIES > 32 )
		#error configUSE_PORT_OPTIMISED_TASK_SELECTION can only be set to 1 when configMAX_PRIORITIES is less than or equal to 32.  It is very rare that a system requires more than 10 to 15 difference priorities as tasks that share a priority will time slice.
	#endif

	/* Store/clear the ready priorities in a bit map. */
	#define portRECORD_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) |= ( 1UL << ( uxPriority ) )
	#define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ) )

	/*-----------------------------------------------------------*/

	#define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities ) uxTopPriority = ( 31UL - __builtin_clz( uxReadyPriorities ) )

#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */


/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site.  These are
not necessary for to use this port.  They are defined so the common demo files
(which build with all the ports) will build. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters ) void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters ) void vFunction( void *pvParameters )

/*-----------------------------------------------------------*/

#define portNOP() __asm volatile 	( " nop " )

#define portINLINE	__inline

#ifndef portFORCE_INLINE
	#define portFORCE_INLINE inline __attribute__(( always_inline))
#endif

#define portMEMORY_BARRIER() __asm volatile( "" ::: "memory" )
/*-----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* PORTMACRO_H */

