#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "defines.h"	// Constants, defines, ...
#include "internal.h"	// Core and memory data.

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
OPCODE( stack_pushallregs ) {
	uint32 regsamount = opcode_args[ 0 ] + 1;

	for ( uint32 i = 0; i < regsamount; i++ ) {
		opcode_args[ 0 ] = i;
		op_stack_pushreg();
	}
}
// Pops some values from the stack to registers. Close analogue of the "popa".
//   arg0: amount of registers (from 0, including this value).
OPCODE( stack_popallregs ) {
	uint32 regsamount = opcode_args[ 0 ] + 1;

	for ( uint32 i = 0; i < regsamount; i++ ) {
		opcode_args[ 0 ] = regsamount - i - 1; // Must be loaded out in reverse order.
		op_stack_popreg();
	}
}

// Pushes a flags internal variable to the stack. Saves only ZF, SF, CF, OF, FDDF, FNF and FPF.
OPCODE( saveflags ) {
	opcode_args[ 0 ] = (uint32) ( proc.flags & FLAGS_Storable );
	op_stack_pushreg();
}
// Pops a flags internal variable from the stack. Restores only ZF, SF, CF, OF, FDDF, FNF and FPF.
OPCODE( loadflags ) {
	uint32 oldSpecialFlags = ( proc.flags & ~FLAGS_Storable );
	uint32 oldReg0 = proc.regs[ 0 ];

	opcode_args[ 0 ] = 0;
	op_stack_popreg();

	proc.regs[ 0 ] = oldReg0;
	proc.flags = ( ( proc.flags & FLAGS_Storable ) | oldSpecialFlags );
}



/* Complex and auxiliary opcodes: */

// Prints a string to the stdout.
//   arg0: index of the first memory char cell;
//   arg1: maximal amount of symbols.
OPCODE( printstr ) {
	char *curchar = &mem[ opcode_args[ 0 ] ];

	for ( int i = opcode_args[ 1 ]; *curchar != '\x0' && i > 0; curchar++, i-- )
		printf( "%c", *curchar );

	puts( "" );
}

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

int main( void ) {
	opcodeCall2( op_mov_const, 0, 'H' );
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
	printf( "[stack test] Reg1: %i\n", proc.regs[ 1 ] );
	opcodeCall2( op_mov_reg, 2, 1 );
	printf( "[\"mov r2, r1\" test] Reg2: %i\n", proc.regs[ 2 ] );

	opcodeCall2( op_mov_const, 1, 0x25 );
	opcodeCall2( op_mov_const, 3, 0x77 );

	printf( "[Bfr \"push all\" test] Reg[0..3]: %i %i %i %i\n", proc.regs[ 0 ], proc.regs[ 1 ], proc.regs[ 2 ], proc.regs[ 3 ] );
	opcodeCall1( op_stack_pushallregs, 3 );
	opcodeCall2( op_mov_const, 0, 3 );
	opcodeCall2( op_mov_const, 1, 6 );
	opcodeCall2( op_mov_const, 2, 9 );
	opcodeCall2( op_mov_const, 3, 12 );
	printf( "[Bfr  \"pop all\" test] Reg[0..3]: %i %i %i %i\n", proc.regs[ 0 ], proc.regs[ 1 ], proc.regs[ 2 ], proc.regs[ 3 ] );
	opcodeCall1( op_stack_popallregs, 3 );
	printf( "[Aft  \"pop all\" test] Reg[0..3]: %i %i %i %i\n", proc.regs[ 0 ], proc.regs[ 1 ], proc.regs[ 2 ], proc.regs[ 3 ] );

	return 0;
}
