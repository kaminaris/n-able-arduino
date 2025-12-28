#pragma once
#include <stdint.h>

#if __GNUC__ > 10
#define ADD_ARCH_ZICSR ".option arch, +zicsr\n"
#else
#define ADD_ARCH_ZICSR
#endif

static inline uint32_t __get_MCAUSE(void) {
	uint32_t result;
	__asm__ volatile( ADD_ARCH_ZICSR "csrr %0," "mcause": "=r"(result));
	return (result);
}

static inline uint32_t __get_MEPC(void) {
	uint32_t result;
	__asm__ volatile( ADD_ARCH_ZICSR "csrr %0," "mepc" : "=r"(result));
	return (result);
}

static inline uint32_t __get_MTVAL(void) {
	uint32_t result;
	__asm__ volatile( ADD_ARCH_ZICSR "csrr %0," "mtval" : "=r" (result) );
	return (result);
}

static inline uint32_t __get_MSTATUS(void) {
	uint32_t result;
	__asm__ volatile( ADD_ARCH_ZICSR "csrr %0, mstatus": "=r"(result) );
	return (result);
}

static inline void PrintHex(uint32_t n) {
	// TODO: NOP
	// while ((*DMDATA0) & 0x80);
	// // Write out character.
	// *DMDATA0 = 0x78302088; //" 0x"
	// int shift;
	// for (shift = 28; shift >= 0; shift -= 4) {
	// 	while ((*DMDATA0) & 0x80);
	// 	int s    = (n >> shift) & 0xf;
	// 	s        += (s < 10) ? '0' : ('a' - 10);
	// 	*DMDATA0 = 0x85 | (s << 8); //" 0x"
	// }
}
