//
// General opcode data matrix.
//

#ifndef __RISCEMUL_OPCODES_MATRIX_H
#define __RISCEMUL_OPCODES_MATRIX_H

#include "defines.h"
#include <stdint.h>


// Forward opcodes declaration:
OPCODE( nop );

OPCODE( load_const );
OPCODE( load_byte );
OPCODE( load_word );
OPCODE( load_dword );
OPCODE( store_lbyte );
OPCODE( store_hbyte );
OPCODE( store_word );
OPCODE( store_dword );

OPCODE( add );
OPCODE( add_const );
OPCODE( sub );
OPCODE( mul );
OPCODE( and );
OPCODE( and_const );
OPCODE( or );
OPCODE( or_const );
OPCODE( xor );
OPCODE( xor_const );
OPCODE( shl );
OPCODE( shl_const );
OPCODE( shr );
OPCODE( shr_const );
OPCODE( neg );

typedef struct {
	void ( *address )( void );
	EOpcodeStructureType structType;
	char name[ 7 ];
} opcode_data;

opcode_data opcode_matrix[] = {
	{ op_nop,			OPST_None,	"nop" },

	{ op_load_const,	OPST_1Regs,	"mov" },
	{ op_load_byte,		OPST_2Regs,	"ldb" },
	{ op_load_word,		OPST_2Regs,	"ldw" },
	{ op_load_dword,	OPST_2Regs,	"ldd" },
	{ op_store_lbyte,	OPST_2Regs,	"svbl" },
	{ op_store_hbyte,	OPST_2Regs,	"svbh" },
	{ op_store_word,	OPST_2Regs,	"svw" },
	{ op_store_dword,	OPST_2Regs,	"svd" },

	{ op_add,			OPST_3Regs, "add" },
	{ op_add_const,		OPST_2Regs, "addv" },
	{ op_sub,			OPST_3Regs, "sub" },
	{ op_mul,			OPST_4Regs, "mul" },
	//{ op_mul_u,			OPST_4Regs, "mulu" },
	//{ op_div,			OPST_4Regs, "div" },
	{ op_and,			OPST_3Regs, "and" },
	{ op_and_const,		OPST_2Regs,	"andv" },
	{ op_or,			OPST_3Regs, "or" },
	{ op_or_const,		OPST_2Regs, "orv" },
	{ op_xor,			OPST_3Regs, "xor" },
	{ op_xor_const,		OPST_2Regs, "xorv" },
	{ op_shl,			OPST_3Regs, "shl" },
	{ op_shl_const,		OPST_2Regs, "shlv" },
	{ op_shr,			OPST_3Regs, "shr" },
	{ op_shr_const,		OPST_2Regs, "shrv" },
	{ op_neg,			OPST_1Regs, "neg" }
};



const int16_t matrixsize = sizeof( opcode_matrix ) / sizeof( opcode_matrix[ 0 ] );

opcode_data *getOpcodeData( unsigned char index ) {
	opcode_data *outptr = NULL;

	if ( index < matrixsize )
		outptr = &opcode_matrix[ (unsigned int) index ];

	return outptr;
}

int16_t findOpcodeMatrixIndex( opcode_pointer opcode ) {
	//static int16_t matrixsize = sizeof( opcode_matrix ) / sizeof( opcode_matrix[ 0 ] );

	for ( int16_t i = 0; i < matrixsize; i++ ) {
		if ( opcode_matrix[ i ].address == opcode )
			return i;
	}

	return -1;
}

#endif // of #ifndef __RISCEMUL_OPCODES_MATRIX_H
