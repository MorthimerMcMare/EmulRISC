#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

#include "defines.h"	// Constants, defines, ...
#include "internal.h"	// Core and memory data.
#include "matrixop.h"	// Opcodes matrix.

// ====================
//       Opcodes
// ====================


/* Main memory control: */

// Stores a 32-bit register into memory.
//   arg0: memory cell to save to.
//   arg1: register index.
OPCODE( save_reg ) {
	mem[ opcode_args[ 0 ] ] = (uint32) proc.regs[ opcode_args[ 1 ] ];
}
// Loads a 32-bit value from memory into register.
//   arg0: register index to save to.
//   arg1: memory cell.
OPCODE( load_reg ) {
	proc.regs[ opcode_args[ 0 ] ] = (uint32) mem[ opcode_args[ 1 ] ];
}



/* Registers control: */

// Moves a constant value to the register.
//   arg0: register index to save to.
//   arg1: constant value.
OPCODE( mov_const ) {
	proc.regs[ opcode_args[ 0 ] ] = opcode_args[ 1 ];
}
// Moves a register value to other register.
//   arg0: register index to save to.
//   arg1: source register index.
OPCODE( mov_reg ) {
	proc.regs[ opcode_args[ 0 ] ] = proc.regs[ opcode_args[ 1 ] ];
}

void regaddsub( uint32 value ) {
	uint32 *reg = &proc.regs[ opcode_args[ 0 ] ];

	uint32 overflowLimit = UINT_MAX - *reg;

	if ( overflowLimit < value )
		proc.flags |= OF;

	*reg += value;

	if ( *reg == 0 )
		proc.flags |= ZF;
	else if ( (int32_t) *reg < 0 )
		proc.flags |= SF;
}

OPCODE( add_const ) {
	regaddsub( opcode_args[ 1 ] );
}
OPCODE( add_reg ) {
	regaddsub( proc.regs[ opcode_args[ 1 ] ] );
}
OPCODE( sub_const ) {
	regaddsub( UINT_MAX - opcode_args[ 1 ] + 1 );
}
OPCODE( sub_reg ) {
	regaddsub( UINT_MAX - proc.regs[ opcode_args[ 1 ] ] + 1 );
}


/* Stack data control: */

// Pushes a single 32-bit value from the register.
//   arg0: register index.
OPCODE( stack_pushreg ) {
	uint32 curStackPos = (uint32) mem[ MEM_BIOS_CURSTACKPOS ];

	memcpy( (void *) &mem[ MEM_STACK_START + curStackPos ], (void *) &proc.regs[ opcode_args[ 0 ] ], 4 );

	mem[ MEM_BIOS_CURSTACKPOS ] = curStackPos + 4;
}
// Pops a single 32-bit value to the register and fills stack node with zero.
//   arg0: register index.
OPCODE( stack_popreg ) {
	uint32 curStackPos = (uint32) mem[ MEM_BIOS_CURSTACKPOS ] - 4;

	proc.regs[ opcode_args[ 0 ] ] = (uint32) mem[ MEM_STACK_START + curStackPos ];
	memset( (void *) &mem[ MEM_STACK_START + curStackPos ], '\x0', 4 );

	mem[ MEM_BIOS_CURSTACKPOS ] = curStackPos;
}

// Pushes some register values to the stack. Close analogue of the "pusha".
//   arg0: amount of registers (from 0, including this value).
OPCODE( stack_pushsomeregs ) {
	uint32 regsamount = opcode_args[ 0 ] + 1;

	for ( uint32 i = 0; i < regsamount; i++ ) {
		opcode_args[ 0 ] = i;
		op_stack_pushreg();
	}
}
// Pops some values from the stack to registers. Close analogue of the "popa".
//   arg0: amount of registers (from 0, including this value).
OPCODE( stack_popsomeregs ) {
	uint32 regsamount = opcode_args[ 0 ] + 1;

	for ( uint32 i = 0; i < regsamount; i++ ) {
		opcode_args[ 0 ] = regsamount - i - 1; // Must be loaded out in reverse order.
		op_stack_popreg();
	}
}

// Pushes a flags internal variable to the stack. Saves only ZF, SF, CF, OF, FDDF, FNF and FPF.
OPCODE( stack_saveflags ) {
	opcode_args[ 0 ] = (uint32) ( proc.flags & FLAGS_Storable );
	op_stack_pushreg();
}
// Pops a flags internal variable from the stack. Restores only ZF, SF, CF, OF, FDDF, FNF and FPF.
OPCODE( stack_loadflags ) {
	uint32 oldSpecialFlags = ( proc.flags & ~FLAGS_Storable );
	uint32 oldReg0 = proc.regs[ 0 ];

	opcode_args[ 0 ] = 0;
	op_stack_popreg();

	proc.regs[ 0 ] = oldReg0;
	proc.flags = ( ( proc.flags & FLAGS_Storable ) | oldSpecialFlags );
}



/* Complex, auxiliary and other special opcodes: */

// Prints a string to the stdout.
//   arg0: index of the first memory char cell;
//   arg1: maximal amount of symbols.
OPCODE( printstr ) {
	char *curchar = &mem[ opcode_args[ 0 ] ];

	for ( int i = opcode_args[ 1 ]; *curchar != '\x0' && i > 0; curchar++, i-- )
		printf( "%c", *curchar );

	puts( "" );
}

OPCODE( stop ) {}

OPCODE( nop ) {}

//OPCODE( interrupt ) {}




// ====================
//     Emulator/VM
// ====================

void opcodeCall( opcode_pointer opcode ) {
	opcode();
}
void opcodeCall1( opcode_pointer opcode, uint32 arg1 ) {
	opcode_args[ 0 ] = arg1;
	opcode();
}
void opcodeCall2( opcode_pointer opcode, uint32 arg1, uint32 arg2 ) {
	opcode_args[ 0 ] = arg1;
	opcode_args[ 1 ] = arg2;
	opcode();
}

void printFlags( void ) {
	printf( "<Flags: " );

	if ( proc.flags & ZF )
		putchar( 'Z' );
	if ( proc.flags & SF )
		putchar( 'S' );
	if ( proc.flags & CF )
		putchar( 'C' );
	if ( proc.flags & OF )
		putchar( 'O' );
	if ( proc.flags & IF )
		putchar( 'I' );

	if ( proc.flags & FDDF )
		printf( "[fdd]" );
	if ( proc.flags & FNF )
		printf( "[fn]" );
	if ( proc.flags & FPF )
		printf( "[fp]" );

	if ( proc.flags & RlModeF )
		printf( "[REAL]" );

	puts( ">" );
}

void queueInstruction( opcode_pointer opcode, uint32 arg0, uint32 arg1 ) {
	if ( proc.instructionptr == 0 ) {
		int16_t opcodeIndex = findOpcodeMatrixIndex( opcode );

		opcode_arg curarg = { 0 };

		if ( opcodeIndex >= 0 ) {
			uint32 startMemCell = proc.protectedModeMemStart;

			mem[ startMemCell ] = opcodeIndex;

			curarg.t32 = arg0;
			memcpy( (void *) &mem[ startMemCell + 1 ], (void *) &curarg.c, 4 );
			//mem[ startMemCell + 1 ] = curarg.c;

			curarg.t32 = arg1;
			memcpy( (void *) &mem[ startMemCell + 5 ], (void *) &curarg.c, 4 );
			//mem[ startMemCell + 5 ] = curarg.c;

			//memcpy( (void *) &mem[ startMemCell + 4 ], (void *) uint32char.c, 4 );
			//memcpy( (void *) &mem[ startMemCell + 1 ], (void *) &arg0, 4 );
			//memcpy( (void *) &mem[ startMemCell + 5 ], (void *) &arg1, 4 );

			for ( int i = 0; i < RISC_INSTRUCTION_LENGHT; i++ )
				printf( "%02X ", mem[ startMemCell + i ] );

			printf( " [Queue: opcode %-2u \"%s\", %Xh:%Xh (orig %xh:%xh)]\n", opcodeIndex, opcode_names[ (int) mem[ startMemCell ] ], * (uint32*) &mem[ startMemCell + 1 ], * (uint32*) &mem[ startMemCell + 5 ], arg0, arg1 );

			proc.protectedModeMemStart += RISC_INSTRUCTION_LENGHT;
		} else {
			printf( "queueInstruction(). Warning: unknown opcode (%i). Args[ 2 ] == { %u, %u }.", opcodeIndex, arg0, arg1 );
		}
	} else {
		puts( "queueInstruction(). Warning: tried to queue an opcode during VM execution." );
	}
}


int main( void ) {
	proc.protectedModeMemStart = MEM_PROG_START;

	queueInstruction( op_mov_const, 0, 100 );
	queueInstruction( op_mov_const, 1, 28 );
	queueInstruction( op_sub_reg, 0, 1 );		// 'H'
	queueInstruction( op_add_const, 1, 77 );	// 'i'

	queueInstruction( op_save_reg, 0x4001, 1 );
	queueInstruction( op_save_reg, 0x4000, 0 );

	queueInstruction( op_mov_const, 2, '!' );	// '!'
	queueInstruction( op_stack_pushreg, 2, 0 );
	queueInstruction( op_stack_popreg, 0, 0 );
	queueInstruction( op_save_reg, 0x4002, 0 );

	queueInstruction( op_printstr, 0x4000, 50 );

	queueInstruction( op_stop, 0, 0 );

	puts( "===============" );
	proc.protectedModeMemStart += ( 1024 - proc.protectedModeMemStart ) % 1024;
	proc.instructionptr = MEM_PROG_START;
	opcode_struct curOpcode = { findOpcodeMatrixIndex( op_nop ), { 0 }, { 0 } };

	while ( curOpcode.id != 0 ) {
		uint32 startptr = proc.instructionptr;

		curOpcode.id = mem[ startptr ];
		memcpy( (void *) curOpcode.arg0.c, (void *) &mem[ startptr + 1 ], 4 );
		memcpy( (void *) curOpcode.arg1.c, (void *) &mem[ startptr + 5 ], 4 );

		printf( " [Trace: \"%8s  %4Xh, %Xh\"]\n", opcode_names[ (int) curOpcode.id ], curOpcode.arg0.t32, curOpcode.arg1.t32 );

		opcodeCall2( opcode_matrix[ (int) curOpcode.id ], curOpcode.arg0.t32, curOpcode.arg1.t32 );

		proc.instructionptr += RISC_INSTRUCTION_LENGHT;
	}

	/*opcodeCall2( op_mov_const, 0, 'H' );
	opcodeCall2( op_save_reg, 3, 0 );
	opcodeCall2( op_mov_const, 0, 'i' );
	opcodeCall2( op_save_reg, 4, 0 );
	opcodeCall2( op_mov_const, 0, '!' );
	opcodeCall2( op_save_reg, 5, 0 );
	opcodeCall2( op_printstr, 3, 17 );

	opcodeCall2( op_mov_const, 1, 0x10 );
	opcodeCall1( op_stack_pushreg, 1 );
	opcodeCall2( op_mov_const, 1, 0xFFFF );
	opcodeCall1( op_stack_popreg, 1 );
	printf( "\n[stack test] Reg1: %i\n\n", proc.regs[ 1 ] );
	opcodeCall2( op_mov_reg, 2, 1 );
	printf( "[\"mov r2, r1\" test] Reg2: %i\n\n", proc.regs[ 2 ] );

	opcodeCall2( op_mov_const, 1, 0x25 );
	opcodeCall2( op_mov_const, 3, 0x77 );

	printf( "[Bfr \"push all\" test] Reg[0..3]: %i %i %i %i\n", proc.regs[ 0 ], proc.regs[ 1 ], proc.regs[ 2 ], proc.regs[ 3 ] );
	opcodeCall1( op_stack_pushsomeregs, 3 );
	opcodeCall2( op_mov_const, 0, 3 );
	opcodeCall2( op_mov_const, 1, 6 );
	opcodeCall2( op_mov_const, 2, 9 );
	opcodeCall2( op_mov_const, 3, 12 );
	printf( "[Bfr  \"pop all\" test] Reg[0..3]: %i %i %i %i\n", proc.regs[ 0 ], proc.regs[ 1 ], proc.regs[ 2 ], proc.regs[ 3 ] );
	opcodeCall1( op_stack_popsomeregs, 3 );
	printf( "[Aft  \"pop all\" test] Reg[0..3]: %i %i %i %i\n\n", proc.regs[ 0 ], proc.regs[ 1 ], proc.regs[ 2 ], proc.regs[ 3 ] );

	opcodeCall2( op_sub_reg, 1, 3 );
	printf( "[\"add r1, r3\" test] Reg1: %i\n\n", proc.regs[ 1 ] );
	printFlags();*/

	return 0;
}
