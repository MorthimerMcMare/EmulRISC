//
// All internal stuff like memory and RISC core itself.
//

#ifndef __RISCEMUL_INTERNAL_H
#define __RISCEMUL_INTERNAL_H

#include "defines.h"

char mem[ MAX_MEM ] = { 0 };

uint32 *screenvar = (uint32 *) &mem[ MEM_KERNELVARS_BIOS_SCREEN ];


/* Registers (modified descriptions
		from https://www.cs.cornell.edu/courses/cs3410/2019sp/riscv/interpreter; and
		from https://en.wikichip.org/wiki/risc-v/registers):
	r0:		zero,	Always zero;
	r1:		ra,		Return Address;
	r2:		sf,		Saved flags;
	r3:		gp,		Global Pointer;
	r4:		tp,		Thread Pointer [unused in this emulator];
	r5-7:	t0-2,	Temporal registers (block 1);
	r8-9:	s0-1,	Save-requiring registers (block 1);
	r10-11:	a0-5,	Arguments registers (block 1);
	r12-17:	a6-7,	Arguments registers/return values (block 2);
	r18-27:	s2-11,	Save-requiring registers (block 2);
	r28-31:	t3-6,	Temporal registers (block 2).
*/

struct _processor {
	uint32 instructionptr; // Also known as program counter.

	union {
		uint32 regs[ MAX_REGS ];
		struct {
			uint32 zero;
			uint32 ra;
			uint32 sf;
			uint32 gp;
			uint32 tp;
			uint32 t0; uint32 t1; uint32 t2;
			uint32 s0; uint32 s1;
			uint32 a0; uint32 a1; uint32 a2; uint32 a3; uint32 a4; uint32 a5; uint32 a6; uint32 a7;
			uint32 s2; uint32 s3; uint32 s4; uint32 s5; uint32 s6; uint32 s7; uint32 s8; uint32 s9; uint32 s10; uint32 s11;
			uint32 t3; uint32 t4; uint32 t5; uint32 t6;
		};
	};

	float fregs[ MAX_FLOAT_REGS ];

	EFlags flags;

	uint32 protectedModeMemStart;

	uint32 tlb[ MAX_TLB ]; // Translation lookaside buffer;
	uint32 bva[ MAX_BVA ]; // Bad virtual address.
} proc;

enum ERegisterIndices {
	RgZ  = 0,
	RgZero = RgZ,
	RgRA = 1,
	RgRetAddr = RgRA,
	RgReturnAddress = RgRA,
	RgSF = 2,
	RgSavedFlags = RgSF,
	RgGP = 3,
	RgTP = 4,

	RgT0, RgT1, RgT2, 												// 5..7;
	RgS0, RgS1, 													// 8..9;
	RgA0, RgA1, RgA2, RgA3, RgA4, RgA5, RgA6, RgA7, 				// 10..16;
	RgS2, RgS3, RgS4, RgS5, RgS6, RgS7, RgS8, RgS9, RgS10, RgS11, 	// 17..27;
	RgT3, RgT4, RgT5, RgT6, 										// 28..31.

	RgQuantity
};


opcode_mem_struct curopc = { 0 };

#endif /* of #ifndef __RISCEMUL_INTERNAL_H */
