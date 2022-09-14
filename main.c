#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

#include "defines.h"	// Constants, defines, ...
#include "internal.h"	// Core and memory data.
#include "opmatrix.h"	// Opcodes matrix.

void opcodeCall( opcode_pointer );
void printFlags( void );


// ====================
//       Opcodes
// ====================


/* Main memory control: */

OPCODE( mov_const ) {
	proc.regs[ curopc.args[ 0 ] ] = curopc.args[ 1 ];
}
OPCODE( load_byte ) {
	proc.regs[ curopc.args[ 0 ] ] = mem[ curopc.args[ 1 ] ] + curopc.args[ 2 ];
}
OPCODE( load_word ) {
	proc.regs[ curopc.args[ 0 ] ] = ( * (uint16 *) &mem[ curopc.args[ 1 ] ] ) + curopc.args[ 2 ];
}
OPCODE( load_dword ) {
	proc.regs[ curopc.args[ 0 ] ] = ( * (uint32 *) &mem[ curopc.args[ 1 ] ] ) + curopc.args[ 2 ];
}


OPCODE( store_lbyte ) {
	mem[ curopc.args[ 0 ] ] = ( proc.regs[ curopc.args[ 1 ] ] & 0xFF ) + curopc.args[ 2 ];
}
OPCODE( store_hbyte ) {
	mem[ curopc.args[ 0 ] ] = ( ( proc.regs[ curopc.args[ 1 ] ] >> 8 ) & 0xFF ) + curopc.args[ 2 ];
}
OPCODE( store_word ) {
	* (uint16 *) &mem[ curopc.args[ 0 ] ] = ( proc.regs[ curopc.args[ 1 ] ] & 0xFFFF ) + curopc.args[ 2 ];
}
OPCODE( store_dword ) {
	* (uint32 *) &mem[ curopc.args[ 0 ] ] = proc.regs[ curopc.args[ 1 ] ] + curopc.args[ 2 ];
}


/* Registers control: */

uint64 setAddOverflow( uint32 val1, uint32 val2 ) {
	uint64 resultadd = ( val1 + val2 );

	if ( resultadd > UINT32_MAX )
		proc.flags |= OF;

	return resultadd;
}

void regaddsub( uint32 value ) {
	uint32 *destreg = &REGARG( 0 );
	*destreg = value;

	if ( *destreg == 0 )
		proc.flags |= ZF;
	else if ( (int32) *destreg < 0 )
		proc.flags |= SF;
}

OPCODE( add_const ) {
	//regaddsub( curopc.args[ 1 ] );
	proc.flags &= ~( ZF | SF | OF );
	regaddsub( setAddOverflow( REGARG( 1 ), curopc.args[ 2 ] ) );
}
OPCODE( add ) {
	proc.flags &= ~( ZF | SF | OF );
	regaddsub( setAddOverflow( setAddOverflow( REGARG( 1 ), REGARG( 2 ) ), curopc.args[ 3 ] ) );
}

OPCODE( sub_const ) {
	proc.flags &= ~( ZF | SF | OF );
	regaddsub( setAddOverflow( REGARG( 1 ), UINT32_MAX - curopc.args[ 1 ] + 1 ) );
}
OPCODE( sub ) {
	proc.flags &= ~( ZF | SF | OF );
	regaddsub( setAddOverflow( setAddOverflow( REGARG( 1 ), UINT32_MAX - REGARG( 2 ) + 1 ), curopc.args[ 3 ] ) );
	//regaddsub( UINT32_MAX - proc.regs[ curopc.args[ 1 ] ] + 1 );
}

OPCODE( mul ) {
	proc.flags &= ~( ZF | SF );

	int64 result = (int32) REGARG( 0 ) * (int32) REGARG( 1 );

	* (int32 *) &REGARG( 2 ) = result & 0xFFFFFFFF;
	* (int32 *) &REGARG( 3 ) = ( result >> 32 ) & 0xFFFFFFFF;

	if ( result == 0 )
		proc.flags |= ZF;
	else if ( result < 0 )
		proc.flags |= SF;
}

OPCODE( mul_u ) {
	uint64 result = REGARG( 0 ) * REGARG( 1 );

	REGARG( 2 ) = result & 0xFFFFFFFF;
	REGARG( 3 ) = ( result >> 32 ) & 0xFFFFFFFF;

	checkZeroFlag( result );
}


OPCODE( and ) {
	REGARG( 0 ) = REGARG( 1 ) & REGARG( 2 );
	checkZeroFlag( REGARG( 0 ) );
}
OPCODE( and_const ) {
	REGARG( 0 ) = REGARG( 1 ) & curopc.args[ 2 ];
	checkZeroFlag( REGARG( 0 ) );
}

OPCODE( or ) {
	REGARG( 0 ) = REGARG( 1 ) | REGARG( 2 );
	checkZeroFlag( REGARG( 0 ) );
}
OPCODE( or_const ) {
	REGARG( 0 ) = REGARG( 1 ) | curopc.args[ 2 ];
	checkZeroFlag( REGARG( 0 ) );
}

OPCODE( xor ) {
	REGARG( 0 ) = REGARG( 1 ) ^ REGARG( 2 );
	checkZeroFlag( REGARG( 0 ) );
}
OPCODE( xor_const ) {
	REGARG( 0 ) = REGARG( 1 ) ^ curopc.args[ 2 ];
	checkZeroFlag( REGARG( 0 ) );
}

OPCODE( shl ) {
	REGARG( 0 ) = REGARG( 1 ) << REGARG( 2 );
	checkZeroFlag( REGARG( 0 ) );
}
OPCODE( shl_const ) {
	REGARG( 0 ) = REGARG( 1 ) << curopc.args[ 2 ];
	checkZeroFlag( REGARG( 0 ) );
}

OPCODE( shr ) {
	REGARG( 0 ) = REGARG( 1 ) >> REGARG( 2 );
	checkZeroFlag( REGARG( 0 ) );
}
OPCODE( shr_const ) {
	REGARG( 0 ) = REGARG( 1 ) >> curopc.args[ 2 ];
	checkZeroFlag( REGARG( 0 ) );
}

OPCODE( neg ) {
	REGARG( 0 ) = ~REGARG( 0 );
}


/* Complex, auxiliary and other special opcodes: */

// Prints a string to the stdout (aux.).
//   Reg0: index of the first memory char cell;
//   Reg1: maximal amount of symbols.
OPCODE( bios_printstr ) {
	char *curchar = &mem[ proc.regs[ 0 ] ];

	uint32 maxchars = proc.regs[ 1 ];
	if ( maxchars == 0 )
		maxchars = UINT32_MAX;

	for ( uint32 i = maxchars; *curchar != '\x0' && i > 0; curchar++, i-- )
		putchar( *curchar );

	puts( "" );
}

OPCODE( nop ) {}


/*OPCODE( int ) {
	int interruptNumber = curopc.args[ 0 ];

	uint32 interruptCodePtr = MEM_INTERRUPT_VECTOR_TABLE_START + interruptNumber * sizeof( uint32 );

	// "BIOS" interrupts are overridden only if address value in the IVT is not equals to zero:
	if ( interruptNumber < MEM_INTERRUPT_VECTOR_TABLE_BIOS_AMOUNT && mem[ interruptCodePtr ] == 0 ) {
		// Temporal, nasty solution.
		switch ( interruptNumber ) {
			case 0:
				proc.flags |= EmulEndF;
				break;
			case 1:
				op_bios_printstr();
				break;
			default:
				break;
		}
	} else if ( mem[ interruptCodePtr ] != 0 ) {
		int prevreg0 = proc.regs[ 0 ];

		// Save last instruction position:
		proc.regs[ 0 ] = proc.instructionptr;
		curopc.args[ 0 ] = 0;
		op_stack_pushreg();

		proc.regs[ 0 ] = prevreg0;

		proc.instructionptr = mem[ interruptCodePtr ];
	} else {
		// An error call must be here.
		//opcodeCall1( op_int, MEM_INTERRUPT_VECTOR_TABLE_EXCEPTIONS_FIRST + 2 );
		printf( "op_int(). Null address in interrupt vector table for cell %i.\n", interruptNumber );
	}
}

OPCODE( ret ) {
	int prevreg0 = proc.regs[ 0 ];

	// Load last instruction position:
	curopc.args[ 0 ] = 0;
	op_stack_popreg();
	proc.instructionptr = proc.regs[ 0 ];

	proc.regs[ 0 ] = prevreg0;
}*/

OPCODE( jmp_const ) {
	int validInstruction = ( curopc.args[ 0 ] >= MEM_PROG_START && curopc.args[ 0 ] < proc.protectedModeMemStart && ( ( curopc.args[ 0 ] - MEM_PROG_START ) % RISC_INSTRUCTION_LENGTH ) == 0 );

	if ( validInstruction || ( proc.flags & RlModeF ) ) {
		proc.instructionptr = curopc.args[ 0 ];
	} else {
		// An error call must be here.
		//opcodeCall1( op_int, MEM_INTERRUPT_VECTOR_TABLE_EXCEPTIONS_FIRST + 1 );
		printf( "op_jmp(). Wrong adress %04X (checks: '>= %04X', '< %04X', 'modulo %i == %i' ).\n", curopc.args[ 0 ], MEM_PROG_START, proc.protectedModeMemStart, RISC_INSTRUCTION_LENGTH, ( curopc.args[ 0 ] - MEM_PROG_START ) % RISC_INSTRUCTION_LENGTH );
	}
}

OPCODE( jmp_reg ) {
	curopc.args[ 0 ] = proc.regs[ curopc.args[ 0 ] ];
	op_jmp_const();
}

OPCODE( jz_const ) {
	if ( proc.flags & ZF ) op_jmp_const();
}
OPCODE( jz_reg ) {
	if ( proc.flags & ZF ) op_jmp_reg();
}
OPCODE( jnz_const ) {
	if ( !( proc.flags & ZF ) ) op_jmp_const();
}
OPCODE( jnz_reg ) {
	if ( !( proc.flags & ZF ) ) op_jmp_reg();
}


// ====================
//     Emulator/VM
// ====================

void opcodeCall( opcode_pointer opcode ) {
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

void queueInstruction( opcode_pointer opcode, const uint32 opcargs[ static const 4 ] ) {
	if ( proc.instructionptr == 0 ) {
		int16_t opcodeIndex = findOpcodeMatrixIndex( opcode );

		if ( opcodeIndex >= 0 ) {
			opcode_structtype_length argLengths = opcode_structtype_lengths[ opcode_matrix[ opcodeIndex ].structType ];

			uint32 startMemCell = proc.protectedModeMemStart;
			uint32 newSrcMem = ( opcodeIndex << 26 );
			uint32 curBitOffset = 6;

			//printf( "OpcIdx. newSrcMem 0x%08X as ( 0x%x ) << 26\n", newSrcMem, opcodeIndex );

			for ( int i = 0; i < argLengths.argsAmount; i++ ) {
				uint16_t curLength = argLengths.lengths[ i ];
				curBitOffset += curLength;

				if ( opcargs[ i ] >= (uint32) ( 1 << curLength ) )
					printf( "queueInstruction(). Warning: in opcode \"%s\" (args[%i] == 0x%04X) >= (field max length 0x%04X). Skipping.\n", opcode_matrix[ (int) opcodeIndex ].name, i, opcargs[ i ], ( 1 << curLength ) );
				else {
					uint32 newBitMask = ( ( opcargs[ i ] & ( ( 1 << curLength ) - 1 ) ) << ( 32 - curBitOffset ) );
					newSrcMem |= newBitMask;

					//printf( "arg[%i]. newSrcMem 0x%08X as ( 0x%X & 0x%04X ) << %i == 0x%04X\n", i, newSrcMem, opcargs[ i ], ( ( 1 << curLength ) - 1 ), ( 32 - curBitOffset ), newBitMask );
				}
			}

			* (uint32 *) &mem[ startMemCell ] = newSrcMem;

			printf( " 0x%04X: ", startMemCell );

			for ( int i = 0; i < RISC_INSTRUCTION_LENGTH; i++ )
				printf( "%02X ", mem[ startMemCell + i ] & 0xFF );

			printf( "[Queue: opcode %-2u \"%s\", orig args %Xh %Xh %Xh %Xh)]\n", opcodeIndex, opcode_matrix[ (int) opcodeIndex ].name, opcargs[ 0 ], opcargs[ 1 ], opcargs[ 2 ], opcargs[ 3 ] );

			proc.protectedModeMemStart += RISC_INSTRUCTION_LENGTH;
		} else {
			printf( "queueInstruction(). Warning: unknown opcode (%i). Args[] == { %u, %u, %u, %u }.", opcodeIndex, opcargs[ 0 ], opcargs[ 1 ], opcargs[ 2 ], opcargs[ 3 ] );
		}
	} else {
		puts( "queueInstruction(). Warning: tried to add an opcode during VM execution." );
	}
} // of void queueInstruction( opcode_pointer opcode, const uint32 opcargs[ static const 4 ] ) {}


int main( void ) {
	proc.protectedModeMemStart = MEM_PROG_START;

	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 0, 0x1001 } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 1, 0x2AA0 } );
	queueInstruction( op_or, ( uint32[ 4 ] ){ 0, 0, 1 } );
	queueInstruction( op_and_const, ( uint32[ 4 ] ){ 1, 1, 0xF0FF } );
	queueInstruction( op_add_const, ( uint32[ 4 ] ){ 1, 1, 1 } );
	//queueInstruction( op_sub, ( uint32[ 4 ] ){ 0, 0, 1, 0 } );

/*	queueInstruction( op_mov_const, 0, 100 );
	queueInstruction( op_mov_const, 1, 28 );
	queueInstruction( op_sub_reg, 0, 1 );		// 'H'
	queueInstruction( op_add_const, 1, 77 );	// 'i'

	queueInstruction( op_save_reg, 0x4001, 1 );
	queueInstruction( op_save_reg, 0x4000, 0 );

	queueInstruction( op_mov_const, 0, '!' );	// '!'
	queueInstruction( op_save_reg, 0x4002, 0 );

	queueInstruction( op_mov_const, 0, 0x4000 );
	queueInstruction( op_mov_const, 1, 0 );
	queueInstruction( op_jmp_const, proc.protectedModeMemStart + RISC_INSTRUCTION_LENGTH * 2, 0 );

	queueInstruction( op_nop, 0, 0 );				// Must be skipped;
	queueInstruction( op_mov_const, 0, 0xABCD );	// Must be skipped.

	queueInstruction( op_int, 1, 0 ); // Prints a string (via quasiBIOS).

	queueInstruction( op_mov_const, 0, 16 );
	queueInstruction( op_mov_const, 1, -16 );
	queueInstruction( op_add_reg, 0, 1 );

	queueInstruction( op_jnz_const, proc.protectedModeMemStart + RISC_INSTRUCTION_LENGTH * 4, 0 );
	queueInstruction( op_mov_const, 2, 'z' );
	queueInstruction( op_save_reg, 0x4400, 2 );
	queueInstruction( op_mov_const, 0, 0x4400 );
	queueInstruction( op_int, 1, 0 );

	queueInstruction( op_int, 0, 0 ); // Ends emulation (via quasiBIOS).
	*/

	puts( "================" );
	proc.protectedModeMemStart += ( 1024 - proc.protectedModeMemStart ) % 1024;
	proc.instructionptr = MEM_PROG_START;

	int exit_countdown = 8;
	unsigned char last_opcode_id_exitcheck = 0;

	while ( !( proc.flags & EmulEndF ) ) {
		// Parsing:
		uint32 startptr = proc.instructionptr;

		curopc.srcmem.t32 = * (uint32 *) &mem[ startptr ];
		curopc.id = ( curopc.srcmem.t32 >> 26 );

		opcode_data *opcode = getOpcodeData( curopc.id );

		opcode_structtype_length argLengths = opcode_structtype_lengths[ opcode_matrix[ (int) curopc.id ].structType ];
		uint32 curSrcMem = ( curopc.srcmem.t32 << 6 );
		uint_fast8_t curBitOffset = 0;

		//printf( "OpIdx. curSrcMem 0x%08X\n", curSrcMem );

		for ( int i = 0; i < argLengths.argsAmount; i++ ) {
			uint_fast8_t curLength = argLengths.lengths[ i ];
			curBitOffset += curLength;

			curopc.args[ i ] = ( curSrcMem >> ( 32 - curBitOffset ) );
			curSrcMem = ( curSrcMem << curLength ) >> curLength;
			//printf( "arg %i: 0x%04X. curSrcMem 0x%08X >> %i\n", i, curopc.args[ i ], curSrcMem, 32 - curBitOffset );
		}

		opcodeCall( opcode->address );

		// Debug tracing (seems to be temporal):
		printf( " [Trace 0x%04X: \"%8s", proc.instructionptr, opcode_matrix[ (int) curopc.id ].name );

		for ( int i = 0; i < argLengths.argsAmount - 1; i++ )
			printf( "%4Xh, ", curopc.args[ i ] );

		if ( argLengths.argsAmount > 0 )
			printf( "%4Xh", curopc.args[ argLengths.argsAmount - 1 ] );

		puts( "\"]" );

		//printf( " [\\Regs 0x%04X: [0]==0x%X, [1]==0x%X, [2]==0x%X]", proc.instructionptr, proc.regs[ 0 ], proc.regs[ 1 ], proc.regs[ 2 ] );
		//printFlags();
		//puts( "" );


		proc.instructionptr += RISC_INSTRUCTION_LENGTH;

		// Emergency exit check:
		if ( curopc.id == last_opcode_id_exitcheck ) {
			if ( --exit_countdown <= 0 ) {
				puts( "Exit downcounter reaches zero." );
				proc.flags |= EmulEndF;
			}
		} else {
			exit_countdown = 8;
		}

	} // of while ( !( proc.flags & EmulEndF ) ) {}

	return 0;
}
