//
// General opcode data matrix.
//

#ifndef __RISCEMUL_OPCODES_MATRIX_H
#define __RISCEMUL_OPCODES_MATRIX_H

#include "defines.h"
#include <stdint.h>


// Forward opcodes declaration:
OPCODE( nop );

OPCODE( mov_const );
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
OPCODE( sub_const );
OPCODE( mul );
OPCODE( mul_u );

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

opcode_data opcode_matrix[ 64 ] = {
	{ op_nop,			OPST_None,	"nop" },

	{ op_mov_const,		OPST_1RegC,	"mov" },
	{ op_load_byte,		OPST_2Reg,	"ldb" },
	{ op_load_word,		OPST_2Reg,	"ldw" },
	{ op_load_dword,	OPST_2Reg,	"ldd" },
	{ op_store_lbyte,	OPST_2Reg,	"svbl" },
	{ op_store_hbyte,	OPST_2Reg,	"svbh" },
	{ op_store_word,	OPST_2Reg,	"svw" },
	{ op_store_dword,	OPST_2Reg,	"svd" },

	{ op_add,			OPST_3RegC,	"add" },
	{ op_add_const,		OPST_2RegC,	"addv" },
	{ op_sub,			OPST_3RegC,	"sub" },
	{ op_mul,			OPST_4Reg,	"mul" },
	{ op_mul_u,			OPST_4Reg,	"mulu" },
	//{ op_div,			OPST_4Reg,	"div" },

	{ op_and,			OPST_3Reg,	"and" },
	{ op_and_const,		OPST_2RegC,	"andv" },
	{ op_or,			OPST_3Reg,	"or" },
	{ op_or_const,		OPST_2RegC,	"orv" },
	{ op_xor,			OPST_3Reg,	"xor" },
	{ op_xor_const,		OPST_2RegC,	"xorv" },
	{ op_shl,			OPST_3Reg,	"shl" },
	{ op_shl_const,		OPST_2RegC,	"shlv" },
	{ op_shr,			OPST_3Reg,	"shr" },
	{ op_shr_const,		OPST_2RegC,	"shrv" },
	{ op_neg,			OPST_1Reg,	"neg" }
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
