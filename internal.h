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
} proc;


typedef struct _float_coprocessor {
	float regs[ MAX_FLOAT_REGS ];
	EFlags flags;
} floatproc;


uint32 opcode_args[ 2 ] = { 0, 0 };

#endif /* of #ifndef __RISCEMUL_INTERNAL_H */
