//
// All internal stuff like memory and RISC core itself.
//

#ifndef __RISCEMUL_INTERNAL_H
#define __RISCEMUL_INTERNAL_H

#include "defines.h"

char mem[ MAX_MEM ] = { 0 };


struct _processor {
	uint32 regs[ MAX_REGS ];
	EFlags flags;

	uint32 protectedModeMemStart;
	uint32 instructionptr;
} proc;


typedef struct _float_coprocessor {
	float regs[ MAX_FLOAT_REGS ];
	EFlags flags;
} floatproc;


opcode_mem_struct curopc = { 0 };

#endif /* of #ifndef __RISCEMUL_INTERNAL_H */
