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
	{ op_store_dword,	OPST_2Regs,	"svd" }
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
