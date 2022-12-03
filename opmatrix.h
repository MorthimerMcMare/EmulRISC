//
// General opcode data matrix.
//

#ifndef __RISCEMUL_OPCODES_MATRIX_H
#define __RISCEMUL_OPCODES_MATRIX_H

#include "defines.h"
#include <stdint.h>


// Forward opcodes declaration:
OPCODE( int );

OPCODE( mov_const );
OPCODE( mov_const_lo );
OPCODE( load_byte );
OPCODE( load_word );
OPCODE( load_dword );
OPCODE( load_dw_addr );
OPCODE( store_lbyte );
OPCODE( store_hbyte );
OPCODE( store_word );
OPCODE( store_dword );
OPCODE( store_dw_addr );

OPCODE( add );
OPCODE( add_const );
OPCODE( sub );
OPCODE( sub_const );
OPCODE( mul );
OPCODE( mul_u );
OPCODE( div );
OPCODE( div_u );

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

OPCODE( jmp_near );
OPCODE( jmp_far );
OPCODE( jf_near );
OPCODE( jf_far );
OPCODE( jnf_near );
OPCODE( jnf_far );

OPCODE( beq_near );
OPCODE( bne_near );
OPCODE( beq_far );
OPCODE( bne_far );

OPCODE( blt_near );
OPCODE( bge_near );
OPCODE( blt_far );
OPCODE( bge_far );

OPCODE( bltu_near );
OPCODE( bgeu_near );
OPCODE( bltu_far );
OPCODE( bgeu_far );

OPCODE( setflag );
OPCODE( clearflag );
OPCODE( int_ret );
OPCODE( nop );
OPCODE( brkp );

OPCODE( fadd );
OPCODE( transfer_int2float );
OPCODE( transfer_float2int );
OPCODE( freg_precision );




typedef struct {
	void ( *address )( void );
	EOpcodeStructureType structType;
	char name[ 6 ];
} opcode_data;

opcode_data opcodes_matrix[ 64 ] = {
	{ op_int,			OPST_ByteConst, "int" },

	{ op_mov_const,		OPST_1RegC,	"mov" },
	{ op_mov_const_lo,	OPST_1RegC,	"movl" },
	{ op_load_byte,		OPST_1RegC,	"ldb" },
	{ op_load_word,		OPST_1RegC,	"ldw" },
	{ op_load_dword,	OPST_1RegC,	"ldd" },
	{ op_load_dw_addr,	OPST_1RegC,	"ldda" },
	{ op_store_lbyte,	OPST_C1Reg,	"svbl" },
	{ op_store_hbyte,	OPST_C1Reg,	"svbh" },
	{ op_store_word,	OPST_C1Reg,	"svw" },
	{ op_store_dword,	OPST_C1Reg,	"svd" },
	{ op_store_dw_addr,	OPST_C1Reg,	"svda" },

	{ op_add,			OPST_3RegC,	"add" },  // add [where], [op1], [op2], [const]	 // where = op1 + op2 + const11;
	{ op_add_const,		OPST_2RegC,	"addv" }, // addv [where], [op1], [const]		 // where = op1 + const16;
	{ op_sub,			OPST_3RegC,	"sub" },
	{ op_mul,			OPST_4Reg,	"mul" },
	{ op_mul_u,			OPST_4Reg,	"mulu" },
	{ op_div,			OPST_4Reg,	"div" },
	{ op_div_u,			OPST_4Reg,	"divu" },

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

	{ op_jmp_near,		OPST_WordConst, "jmp" },
	{ op_jmp_far,		OPST_1RegC, "jmpr" },
	{ op_jf_near,		OPST_1RegC, "jx" },
	{ op_jnf_near,		OPST_1RegC, "jnx" },
	{ op_jf_far,		OPST_2Reg,  "jxf" },
	{ op_jnf_far,		OPST_2Reg,  "jnxf" },

	{ op_beq_near,		OPST_2RegC, "beq" },
	{ op_bne_near,		OPST_2RegC, "bne" },
	{ op_beq_far,		OPST_3Reg,  "beqf" },
	{ op_bne_far,		OPST_3Reg,  "bnef" },
	{ op_blt_near,		OPST_2RegC, "blt" },
	{ op_bge_near,		OPST_2RegC, "bge" },
	{ op_blt_far,		OPST_3Reg,  "bltf" },
	{ op_bge_far,		OPST_3Reg,  "bgef" },
	{ op_bltu_near,		OPST_2RegC, "bltu" },
	{ op_bgeu_near,		OPST_2RegC, "bgeu" },
	{ op_bltu_far,		OPST_3Reg,  "bltuf" },
	{ op_bgeu_far,		OPST_3Reg,  "bgeuf" },

	{ op_setflag,		OPST_MaxConst, "stf" },
	{ op_clearflag,		OPST_MaxConst, "clf" },

	{ op_int_ret,		OPST_None, "iret" },
	{ op_nop,			OPST_None, "nop" },
	{ op_brkp,			OPST_None, "brkp" },

	{ op_fadd,			OPST_3Reg, "fadd" },
	{ op_transfer_int2float, OPST_3Reg,  "trif" },
	{ op_transfer_float2int, OPST_3Reg,  "trfi" },
	{ op_freg_precision,	 OPST_2RegC, "fprc" },
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

EXCEPTIONOPCODE( end_emulation );
EXCEPTIONOPCODE( zero_division );
EXCEPTIONOPCODE( trace );
EXCEPTIONOPCODE( breakpoint );
EXCEPTIONOPCODE( invalid_opcode );
EXCEPTIONOPCODE( invalid_interrupt );
EXCEPTIONOPCODE( not_real_mode );
EXCEPTIONOPCODE( printscreen );
EXCEPTIONOPCODE( irq0 );
EXCEPTIONOPCODE( irq1 );
EXCEPTIONOPCODE( irq2 );
EXCEPTIONOPCODE( irq3 );
EXCEPTIONOPCODE( irq4 );
EXCEPTIONOPCODE( irq5 );
EXCEPTIONOPCODE( irq6 );
EXCEPTIONOPCODE( irq7 );

BIOSOPCODE( print );
BIOSOPCODE( printdigit );
BIOSOPCODE( videomemory );
BIOSOPCODE( getkey );
BIOSOPCODE( getstring );

typedef struct {
	void ( *defaddress )( void );
	EInterruptType type;
} interrupt_data;

interrupt_data interrupts_matrix[ MEM_INTERRUPT_VECTOR_TABLE_EXCEPTIONS_AMOUNT + MEM_INTERRUPT_VECTOR_TABLE_BIOS_AMOUNT ] = {
	{ except_end_emulation,		INTT_Exception | INTT_Unmaskable },
	{ except_zero_division,		INTT_Exception | INTT_Unmaskable },
	{ except_trace,				INTT_Exception | INTT_Unmaskable },
	{ except_breakpoint,		INTT_Exception | INTT_Unmaskable },
	{ except_invalid_opcode,	INTT_Exception | INTT_Unmaskable },
	{ except_invalid_interrupt,	INTT_Exception | INTT_Unmaskable },
	{ except_not_real_mode,		INTT_Exception | INTT_Unmaskable },
	{ except_printscreen,		INTT_Exception },
	{ except_irq0,				INTT_IRQ }, // 0x08
	{ except_irq1, 				INTT_IRQ },
	{ except_irq2, 				INTT_IRQ },
	{ except_irq3, 				INTT_IRQ },
	{ except_irq4, 				INTT_IRQ },
	{ except_irq5, 				INTT_IRQ },
	{ except_irq6, 				INTT_IRQ },
	{ except_irq7, 				INTT_IRQ },
	{ bios_videomemory,			INTT_BIOS }, // 0x10
	{ bios_print,				INTT_BIOS },
	{ bios_printdigit,			INTT_BIOS },
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
