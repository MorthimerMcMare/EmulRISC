//
// Matrix 
//

#ifndef __RISCEMUL_OPCODES_MATRIX_H
#define __RISCEMUL_OPCODES_MATRIX_H

#include "defines.h"
#include <stdint.h>

// 1 byte : opcode;
// 4 bytes: arg0;
// 4 bytes: arg1.


// Forward opcodes declaration:
OPCODE( stop );

OPCODE( mov_const );
OPCODE( mov_reg );
OPCODE( save_reg );
OPCODE( load_reg );

OPCODE( add_const );
OPCODE( add_reg );
OPCODE( sub_const );
OPCODE( sub_reg );

OPCODE( stack_pushreg );
OPCODE( stack_popreg );
OPCODE( stack_pushsomeregs );
OPCODE( stack_popsomeregs );
OPCODE( stack_saveflags );
OPCODE( stack_loadflags );

OPCODE( int );
OPCODE( nop );


// Opcodes matrix itself:
void ( *opcode_matrix[] )( void ) = {
	op_nop,

	op_mov_const,
	op_mov_reg,
	op_save_reg,
	op_load_reg,

	op_add_const,
	op_add_reg,
	op_sub_const,
	op_sub_reg,

	op_stack_pushreg,
	op_stack_popreg,
	op_stack_pushsomeregs,
	op_stack_popsomeregs,
	op_stack_saveflags,
	op_stack_loadflags,

	op_int
};

const char *opcode_names[] = {
	"nop",
	"movc", "mov", "save", "load",
	"addc", "add", "subc", "sub",
	"push", "pop", "pushsome", "popsome", "pushf", "popf",
	"int"
};


int16_t findOpcodeMatrixIndex( opcode_pointer opcode ) {
	static int16_t matrixsize = sizeof( opcode_matrix ) / sizeof( opcode_matrix[ 0 ] );

	for ( int16_t i = 0; i < matrixsize; i++ ) {
		if ( opcode_matrix[ i ] == opcode )
			return i;
	}

	return -1;
}


#endif // of #ifndef __RISCEMUL_OPCODES_MATRIX_H
