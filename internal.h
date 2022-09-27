//
// All internal stuff like memory and RISC core itself.
//

#ifndef __RISCEMUL_INTERNAL_H
#define __RISCEMUL_INTERNAL_H

#include "defines.h"

char mem[ MAX_MEM ] = { 0 };


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
	r10-11:	a0-1,	Arguments registers/return values (block 1);
	r12-17:	a2-7,	Arguments registers (block 2);
	r18-27:	s2-11,	Save-requiring registers (block 2);
	r28-31:	t3-6,	Temporal registers (block 2).
*/

struct _processor {
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

	EFlags flags;

	uint32 protectedModeMemStart;
	uint32 instructionptr;
	uint32 tlb; // uint32 *TranslateLookasideBuffer[ 64 ]
	uint32 bva; // Bad virtual address.
} proc;


typedef struct _float_coprocessor {
	float regs[ MAX_FLOAT_REGS ];
	EFlags flags;
} floatproc;


opcode_mem_struct curopc = { 0 };

#endif /* of #ifndef __RISCEMUL_INTERNAL_H */
