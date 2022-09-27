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

OPCODE( call_const );
OPCODE( call_reg );

OPCODE( jmp_const );
OPCODE( jz_const );
OPCODE( jnz_const );
OPCODE( js_const );
OPCODE( jns_const );
OPCODE( jo_const );
OPCODE( jno_const );
OPCODE( jc_const );
OPCODE( jnc_const );

OPCODE( beq );
OPCODE( bne );
OPCODE( blt );
OPCODE( bge );
OPCODE( bltu );
OPCODE( bgeu );

OPCODE( setflag );
OPCODE( clearflag );
OPCODE( int );
OPCODE( int_ret );


typedef struct {
	void ( *address )( void );
	EOpcodeStructureType structType;
	char name[ 7 ];
} opcode_data;

opcode_data opcodes_matrix[ 64 ] = {
	{ op_nop,			OPST_None,	"nop" },

	{ op_mov_const,		OPST_1RegC,	"mov" },
	{ op_load_byte,		OPST_1RegC,	"ldb" },
	{ op_load_word,		OPST_1RegC,	"ldw" },
	{ op_load_dword,	OPST_1RegC,	"ldd" },
	{ op_store_lbyte,	OPST_C1Reg,	"svbl" },
	{ op_store_hbyte,	OPST_C1Reg,	"svbh" },
	{ op_store_word,	OPST_C1Reg,	"svw" },
	{ op_store_dword,	OPST_C1Reg,	"svd" },

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
	{ op_neg,			OPST_1Reg,	"neg" },

	{ op_call_const,	OPST_1RegC, "callv" },
	{ op_call_reg,		OPST_2RegC, "call" },

	{ op_jmp_const,		OPST_WordConst, "jmp" },
	{ op_jz_const,		OPST_MaxConst, "jz" },
	{ op_jnz_const,		OPST_MaxConst, "jnz" },
	{ op_js_const,		OPST_WordConst, "js" },
	{ op_jns_const,		OPST_WordConst, "jns" },
	{ op_jo_const,		OPST_WordConst, "jo" },
	{ op_jno_const,		OPST_WordConst, "jno" },
	{ op_jc_const,		OPST_WordConst, "jc" },
	{ op_jnc_const,		OPST_WordConst, "jnc" },

	{ op_beq,			OPST_2RegC, "beq" },
	{ op_bne,			OPST_2RegC, "bne" },
	{ op_blt,			OPST_2RegC, "blt" },
	{ op_bge,			OPST_2RegC, "bge" },
	{ op_bltu,			OPST_2RegC, "bltu" },
	{ op_bgeu,			OPST_2RegC, "bgeu" },

	{ op_setflag,		OPST_ByteConst, "stf" },
	{ op_clearflag,		OPST_ByteConst, "clf" },

	{ op_int,			OPST_ByteConst, "int" },
	{ op_int_ret,		OPST_None, "iret" },
};


const int16_t opcodes_matrix_size = sizeof( opcodes_matrix ) / sizeof( opcodes_matrix[ 0 ] );

opcode_data *getOpcodeData( unsigned char index ) {
	opcode_data *outptr = NULL;

	if ( index < opcodes_matrix_size )
		outptr = &opcodes_matrix[ (unsigned int) index ];

	return outptr;
}

int16_t findOpcodeMatrixIndex( opcode_pointer opcode ) {
	for ( int16_t i = 0; i < opcodes_matrix_size; i++ ) {
		if ( opcodes_matrix[ i ].address == opcode )
			return i;
	}

	return -1;
}



// Heavily modified exceptions list from https://www.cs.cmu.edu/~ralf/files.html, "OVERVIEW.LST".

EXCEPTIONOPCODE( zero_division );
EXCEPTIONOPCODE( trace );
EXCEPTIONOPCODE( breakpoint );
EXCEPTIONOPCODE( invalid_opcode );
EXCEPTIONOPCODE( invalid_interrupt );
EXCEPTIONOPCODE( printscreen );
EXCEPTIONOPCODE( irq0 );
EXCEPTIONOPCODE( irq1 );
EXCEPTIONOPCODE( irq2 );
EXCEPTIONOPCODE( irq3 );
EXCEPTIONOPCODE( irq4 );
EXCEPTIONOPCODE( irq5 );
EXCEPTIONOPCODE( irq6 );
EXCEPTIONOPCODE( irq7 );
EXCEPTIONOPCODE( end_emulation );

BIOSOPCODE( printstr );
BIOSOPCODE( printdigit );
BIOSOPCODE( printnewline );
BIOSOPCODE( videomemory );
BIOSOPCODE( getkey );
BIOSOPCODE( getstring );

typedef struct {
	void ( *defaddress )( void );
	EInterruptType type;
} interrupt_data;

interrupt_data interrupts_matrix[ 255 ] = {
	{ except_zero_division,		INTT_Exception | INTT_Unmaskable },
	{ except_trace,				INTT_Exception | INTT_Unmaskable },
	{ except_breakpoint,		INTT_Exception | INTT_Unmaskable },
	{ except_invalid_opcode,	INTT_Exception | INTT_Unmaskable },
	{ except_invalid_interrupt,	INTT_Exception | INTT_Unmaskable },
	{ except_printscreen,		INTT_Exception },
	{ except_irq0,				INTT_IRQ | INTT_Unmaskable }, // System timer.
	{ except_irq1, 				INTT_IRQ },
	{ except_irq2, 				INTT_IRQ }, // 0x08
	{ except_irq3, 				INTT_IRQ },
	{ except_irq4, 				INTT_IRQ },
	{ except_irq5, 				INTT_IRQ },
	{ except_irq6, 				INTT_IRQ },
	{ except_irq7, 				INTT_IRQ },
	{ NULL, 0 },
	{ except_end_emulation,		INTT_Exception | INTT_Unmaskable },
	{ bios_printstr,			INTT_BIOS }, // 0x10
	{ bios_printdigit,			INTT_BIOS },
	{ bios_printnewline,		INTT_BIOS },
	{ bios_videomemory,			INTT_BIOS },
	{ bios_getkey,				INTT_BIOS },
	{ bios_getstring,			INTT_BIOS },
};

const int16_t interrupts_matrix_size = sizeof( interrupts_matrix ) / sizeof( interrupts_matrix[ 0 ] );

interrupt_data *getInterruptData( unsigned char index ) {
	interrupt_data *outptr = NULL;

	if ( index < interrupts_matrix_size )
		outptr = &interrupts_matrix[ (unsigned int) index ];

	return outptr;
}

int16_t findInterruptMatrixIndex( opcode_pointer defaultaddress ) {
	for ( int16_t i = 0; i < interrupts_matrix_size; i++ ) {
		if ( interrupts_matrix[ i ].defaddress == defaultaddress )
			return i;
	}

	return -1;
}

#endif // of #ifndef __RISCEMUL_OPCODES_MATRIX_H
