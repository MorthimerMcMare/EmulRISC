#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

//#define USE_NCURSES

#include "defines.h"	// Constants, defines, ...
#include "internal.h"	// Core and memory data.
#include "opmatrix.h"	// Opcodes matrix.
#include "bios.h"		// QuasiBIOS functions.

void opcodeCall( opcode_pointer );
void printFlags( void );


// ====================
//       Opcodes
// ====================


/* Main memory control: */

OPCODE( mov_const ) {
	REGARG( 0 ) = curopc.args[ 1 ];
}
OPCODE( load_byte ) {
	REGARG( 0 ) = (unsigned char) ( mem[ curopc.args[ 1 ] ] + curopc.args[ 2 ] );
}
OPCODE( load_word ) {
	REGARG( 0 ) = ( * (uint16 *) &mem[ curopc.args[ 1 ] ] ) + curopc.args[ 2 ];
}
OPCODE( load_dword ) {
	REGARG( 0 ) = ( * (uint32 *) &mem[ curopc.args[ 1 ] ] ) + curopc.args[ 2 ];
}


OPCODE( store_lbyte ) {
	mem[ curopc.args[ 0 ] ] = REGARG( 1 ) & 0xFF;
	//printf( "mem[ 0x%X ] = 0x%02X (really saved 0x%02X '%c')\n", curopc.args[ 0 ], REGARG( 1 ), (char) mem[ curopc.args[ 0 ] ], (char) mem[ curopc.args[ 0 ] ] );
}
OPCODE( store_hbyte ) {
	mem[ curopc.args[ 0 ] ] = ( proc.regs[ curopc.args[ 1 ] ] >> 8 ) & 0xFF;
}
OPCODE( store_word ) {
	* (uint16 *) &mem[ curopc.args[ 0 ] ] = proc.regs[ curopc.args[ 1 ] ] & 0xFFFF;
}
OPCODE( store_dword ) {
	* (uint32 *) &mem[ curopc.args[ 0 ] ] = proc.regs[ curopc.args[ 1 ] ];

	/*printf( "mem[ 0x%X ] = 0x%02X (really saved 0x%02X '%c')\n", curopc.args[ 0 ], REGARG( 1 ), * (uint32 *) &mem[ curopc.args[ 0 ] ], (unsigned char) mem[ curopc.args[ 0 ] ] );
	for ( uint32 i = curopc.args[ 0 ]; i < curopc.args[ 0 ] + 4; i++ )
		printf( "0x%02X ", (unsigned char) mem[ i ] );
	puts( "" );*/
}


/* Math and logical opcodes: */

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

OPCODE( shl_const ) {
	proc.flags = ( proc.flags & ~OF ) | ( OF * !!( (uint32) curopc.args[ 2 ] > 32 ) ); // Too far shift.
	proc.flags = ( proc.flags & ~CF ) | ( CF * !!( REGARG( 1 ) & ( 1 << (UINT32_MAX - curopc.args[ 2 ] - 1) ) ) ); // Save last shifted bit.
	REGARG( 0 ) = REGARG( 1 ) << curopc.args[ 2 ];
	checkZeroFlag( REGARG( 0 ) );
}
OPCODE( shl ) {
	curopc.args[ 2 ] = REGARG( 2 );
	op_shl_const();
}

OPCODE( shr_const ) {
	proc.flags = ( proc.flags & ~OF ) | ( OF * !!( (uint32) curopc.args[ 2 ] > 32 ) ); // Too far shift.
	proc.flags = ( proc.flags & ~CF ) | ( CF * !!( REGARG( 1 ) & ( 1 << (curopc.args[ 2 ] - 1) ) ) ); // Save last shifted bit.
	REGARG( 0 ) = REGARG( 1 ) >> curopc.args[ 2 ];
	checkZeroFlag( REGARG( 0 ) );
}
OPCODE( shr ) {
	curopc.args[ 2 ] = REGARG( 2 );
	op_shr_const();
}

OPCODE( neg ) {
	REGARG( 0 ) = ~REGARG( 0 );
}


OPCODE( nop ) {}



/* Jumps: */

OPCODE( call_const ) {	// "JAL r_saveto, const_offset".
	REGARG( 0 ) = proc.instructionptr + RISC_INSTRUCTION_LENGTH;
	proc.instructionptr = curopc.args[ 1 ];
}
OPCODE( call_reg ) {	// "JALR r_saveto, r_offset, const_offset".
	REGARG( 0 ) = proc.instructionptr + RISC_INSTRUCTION_LENGTH;
	proc.instructionptr = REGARG( 1 ) + curopc.args[ 2 ];
}

OPCODE( int ) {
	proc.sf = proc.flags;
	proc.flags &= ~TF;

	int interruptNumber = curopc.args[ 0 ];
	interrupt_data *intdata = getInterruptData( interruptNumber );

	if ( !intdata )
		interruptNumber = -1;

	if ( interruptNumber >= 0 ) {
		if ( ( !( intdata->type & INTT_Unmaskable ) && !( proc.flags & IF ) ) )
			return;

		uint32 interruptCodePtr = * (uint32 *) &mem[ MEM_INTERRUPT_VECTOR_TABLE_START + interruptNumber * sizeof( uint32 ) ];

		//printf( "op_int(). intCodeptr == mem[ 0x%04X ] == 0x%X.\n", interruptCodePtr, * (uint32 *) &mem[ interruptCodePtr ] );

		// Interrupts are overridden only if address value in the IVT is not equals to zero:
		if ( interruptCodePtr == 0 && intdata->defaddress ) {
			intdata->defaddress();

		} else if ( interruptCodePtr != 0 ) {
			// Save last instruction position:
			proc.ra = proc.instructionptr;
			proc.instructionptr = interruptCodePtr;

			//printf( "op_int(). Interrupt address 0x%04X, return address 0x%04X.\n", proc.instructionptr, proc.ra );

		} else {
			interruptNumber = -1;
		}
	} // of if ( interruptNumber > 0 ) {}

	if ( interruptNumber < 0 ) {
		proc.ra = proc.instructionptr;
		curopc.args[ 0 ] = findInterruptMatrixIndex( except_invalid_interrupt );
		opcodeCall( op_int );
	}

} // of OPCODE( int ) {}

OPCODE( int_ret ) {
	proc.flags = ( proc.sf & ( ~TF ) ) | PreTF;
	proc.instructionptr = proc.ra;
}


OPCODE( jmp_const ) {
	//printf( "jmp_const(). prev 0x%X, add %i, new 0x%X\n", proc.instructionptr, RISC_INSTRUCTION_LENGTH * (int32) curopc.args[ 0 ], proc.instructionptr + RISC_INSTRUCTION_LENGTH * (int32) curopc.args[ 0 ] );
	proc.instructionptr += RISC_INSTRUCTION_LENGTH * (int32) curopc.args[ 0 ];
}
OPCODE( jmp_reg ) {
	proc.instructionptr += RISC_INSTRUCTION_LENGTH * (int32) REGARG( 0 );
}

OPCODE( jz_const ) 	{ if ( proc.flags & ZF ) proc.instructionptr = curopc.args[ 0 ]; }
OPCODE( jz_reg ) 	{ if ( proc.flags & ZF ) proc.instructionptr = REGARG( 0 ); }
OPCODE( jnz_const )	{ if ( !( proc.flags & ZF ) ) proc.instructionptr = curopc.args[ 0 ]; }
OPCODE( jnz_reg ) 	{ if ( !( proc.flags & ZF ) ) proc.instructionptr = REGARG( 0 ); }

OPCODE( js_const ) 	{ if ( proc.flags & SF ) proc.instructionptr = curopc.args[ 0 ]; }
OPCODE( js_reg ) 	{ if ( proc.flags & SF ) proc.instructionptr = REGARG( 0 ); }
OPCODE( jns_const )	{ if ( !( proc.flags & SF ) ) proc.instructionptr = curopc.args[ 0 ]; }
OPCODE( jns_reg ) 	{ if ( !( proc.flags & SF ) ) proc.instructionptr = REGARG( 0 ); }

OPCODE( jo_const ) 	{ if ( proc.flags & OF ) proc.instructionptr = curopc.args[ 0 ]; }
OPCODE( jo_reg ) 	{ if ( proc.flags & OF ) proc.instructionptr = REGARG( 0 ); }
OPCODE( jno_const )	{ if ( !( proc.flags & OF ) ) proc.instructionptr = curopc.args[ 0 ]; }
OPCODE( jno_reg ) 	{ if ( !( proc.flags & OF ) ) proc.instructionptr = REGARG( 0 ); }

OPCODE( jc_const ) 	{ if ( proc.flags & CF ) proc.instructionptr = curopc.args[ 0 ]; }
OPCODE( jc_reg ) 	{ if ( proc.flags & CF ) proc.instructionptr = REGARG( 0 ); }
OPCODE( jnc_const )	{ if ( !( proc.flags & CF ) ) proc.instructionptr = curopc.args[ 0 ]; }
OPCODE( jnc_reg ) 	{ if ( !( proc.flags & CF ) ) proc.instructionptr = REGARG( 0 ); }

OPCODE( beq ) { if ( REGARG( 0 ) == REGARG( 1 ) ) proc.instructionptr += curopc.args[ 2 ]; }
OPCODE( bne ) { if ( REGARG( 0 ) != REGARG( 1 ) ) proc.instructionptr += curopc.args[ 2 ]; }

OPCODE( blt ) { if ( (int32) REGARG( 0 ) <  (int32) REGARG( 1 ) ) proc.instructionptr += curopc.args[ 2 ]; }
OPCODE( bge ) { if ( (int32) REGARG( 0 ) >= (int32) REGARG( 1 ) ) proc.instructionptr += curopc.args[ 2 ]; }

OPCODE( bltu ) { if ( (uint32) REGARG( 0 ) <  (uint32) REGARG( 1 ) ) proc.instructionptr += curopc.args[ 2 ]; }
OPCODE( bgeu ) { if ( (uint32) REGARG( 0 ) >= (uint32) REGARG( 1 ) ) proc.instructionptr += curopc.args[ 2 ]; }

OPCODE( setflag )	{ proc.flags |= ( curopc.args[ 0 ] & FLAGS_Storable ); }
OPCODE( clearflag )	{ proc.flags &= ~( curopc.args[ 0 ] & FLAGS_Storable ); }




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
	if ( proc.flags & TF )
		putchar( 'T' );

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
			opcode_structtype_length argLengths = opcode_structtype_lengths[ opcodes_matrix[ opcodeIndex ].structType ];

			uint32 startMemCell = proc.protectedModeMemStart;
			uint32 newSrcMem = ( opcodeIndex << 26 );
			uint32 curBitOffset = 6;

			//printf( "OpcIdx. newSrcMem 0x%08X as ( 0x%x ) << 26\n", newSrcMem, opcodeIndex );

			for ( int i = 0; i < argLengths.argsAmount; i++ ) {
				uint16_t curLength = argLengths.lengths[ i ];
				curBitOffset += curLength;

				if ( opcargs[ i ] >= (uint32) ( 1 << curLength ) ) {
					printf( "queueInstruction(). Warning: in opcode \"%s\" (args[%i] == 0x%04X) >= (field max length 0x%04X). Skipping.\n", opcodes_matrix[ (int) opcodeIndex ].name, i, opcargs[ i ], ( 1 << curLength ) );
				} else {
					uint32 newBitMask = ( ( opcargs[ i ] & ( ( 1 << curLength ) - 1 ) ) << ( 32 - curBitOffset ) );
					newSrcMem |= newBitMask;

					//printf( "arg[%i]. newSrcMem 0x%08X as ( 0x%X & 0x%04X ) << %i == 0x%04X\n", i, newSrcMem, opcargs[ i ], ( ( 1 << curLength ) - 1 ), ( 32 - curBitOffset ), newBitMask );
				}
			}

			* (uint32 *) &mem[ startMemCell ] = newSrcMem;

			printf( " 0x%04X: ", startMemCell );

			for ( int i = 0; i < RISC_INSTRUCTION_LENGTH; i++ )
				printf( "%02X ", mem[ startMemCell + i ] & 0xFF );

			printf( "[Queue: opcode %-2u \"%s\", orig args %Xh %Xh %Xh %Xh)]\n", opcodeIndex, opcodes_matrix[ (int) opcodeIndex ].name, opcargs[ 0 ], opcargs[ 1 ], opcargs[ 2 ], opcargs[ 3 ] );

			proc.protectedModeMemStart += RISC_INSTRUCTION_LENGTH;
		} else {
			printf( "queueInstruction(). Warning: unknown opcode (%i). Args[] == { %u, %u, %u, %u }.", opcodeIndex, opcargs[ 0 ], opcargs[ 1 ], opcargs[ 2 ], opcargs[ 3 ] );
		}
	} else {
		puts( "queueInstruction(). Warning: tried to add an opcode during VM execution." );
	}
} // of void queueInstruction( opcode_pointer opcode, const uint32 opcargs[ static const 4 ] ) {}


int main( void ) {
	proc.protectedModeMemStart = MEM_PROG_KERNEL_START;
	proc.flags = IF;

#ifdef LINUX_NCURSES
	initscr();
	noecho();
#endif

	// Tracer initialization and code:
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 10, MEM_INTERRUPTS_START } );
	queueInstruction( op_store_dword, ( uint32[ 4 ] ){ MEM_INTERRUPT_VECTOR_TABLE_EXCEPTIONS_START + findInterruptMatrixIndex( except_trace ) * 4, 10 } );

	uint32 savedInstructionPos = proc.protectedModeMemStart;
	proc.protectedModeMemStart = MEM_INTERRUPTS_START + findInterruptMatrixIndex( except_trace ) * 4;

	// 256 bytes: 48 opcodes, 16 variables.
	uint32 interruptMemory = proc.protectedModeMemStart + 48 * 4;
	queueInstruction( op_store_dword, ( uint32[ 4 ] ){ interruptMemory, 10 } );
	queueInstruction( op_store_dword, ( uint32[ 4 ] ){ interruptMemory + 4, 11 } );
	queueInstruction( op_store_dword, ( uint32[ 4 ] ){ interruptMemory + 8, 2 } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 0x123456 } );

	queueInstruction( op_load_dword, ( uint32[ 4 ] ){ 10, interruptMemory } );
	queueInstruction( op_load_dword, ( uint32[ 4 ] ){ 11, interruptMemory + 4 } );
	queueInstruction( op_load_dword, ( uint32[ 4 ] ){ 2, interruptMemory + 8 } );
	queueInstruction( op_int_ret, ( uint32[ 4 ] ){ 0 } );
	//queueInstruction( op_call_reg, ( uint32[ 4 ] ){ 0, 1, 0 } );


	proc.protectedModeMemStart = savedInstructionPos;

	// Test program:
	//queueInstruction( op_setflag, ( uint32[ 4 ] ){ TF } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 1, 100 } );
	//queueInstruction( op_clearflag, ( uint32[ 4 ] ){ TF } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 2, 28 } );

	queueInstruction( op_sub, ( uint32[ 4 ] ){ 1, 1, 2 } );		// 'H'
	queueInstruction( op_add_const, ( uint32[ 4 ] ){ 2, 2, 77 } );	// 'i'

	queueInstruction( op_store_lbyte, ( uint32[ 4 ] ){ 0x60001, 2 } );
	queueInstruction( op_store_lbyte, ( uint32[ 4 ] ){ 0x60000, 1 } );

	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 1, '!' } );	// '!'
	queueInstruction( op_store_lbyte, ( uint32[ 4 ] ){ 0x60002, 1 } );

	//queueInstruction( op_setflag, ( uint32[ 4 ] ){ TF } );

	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 10, 0 } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 2 } );
	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( bios_videomemory ) } ); // Videomemory output.

	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 10, 4 } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 999 } );				// (0x1FFF8A8 vs 0x1FFF830).
	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( bios_videomemory ) } ); // Moves cursor (subfunc 4h).
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 10, 0x60000 } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 0 } );

	queueInstruction( op_jmp_const, ( uint32[ 4 ] ){ 2 } );
	queueInstruction( op_nop, ( uint32[ 4 ] ){ 0 } );				// Will be skipped;
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 1, 0xABCD } );	// Will be skipped.

	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( bios_print ) } ); // Prints a string (subfunc 0h).
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 2 } );
	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( bios_print ) } ); // A newline character (subfunc 2h)

	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( bios_getkey ) } ); // Waits for the keystroke.
	/*queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 'q' } );
	queueInstruction( op_beq, ( uint32[ 4 ] ){ 10, 11, 3 } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 'Q' } );
	queueInstruction( op_beq, ( uint32[ 4 ] ){ 10, 11, 1 } );
	queueInstruction( op_jmp_const, ( uint32[ 4 ] ){ -5 } );*/

	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( except_end_emulation ) } ); // Ends emulation (via quasiBIOS).

	puts( "================" );
	proc.protectedModeMemStart += ( 1024 - proc.protectedModeMemStart ) % 1024;
	proc.instructionptr = MEM_PROG_KERNEL_START;

	const int MAX_EXIT_DOWNCOUNTER = 40;
	int exit_countdown = MAX_EXIT_DOWNCOUNTER;
	//uint32 last_opcode_id_exitcheck = 0;

	uint32 traceExceptionIndex = FINDINT( except_trace );

	while ( !( proc.flags & EndEmulF ) ) {
		// Parsing:
		uint32 startptr = proc.instructionptr;

		curopc.srcmem.t32 = * (uint32 *) &mem[ startptr ];
		curopc.id = ( curopc.srcmem.t32 >> 26 );

		opcode_data *opcode = getOpcodeData( curopc.id );

		opcode_structtype_length argLengths = opcode_structtype_lengths[ opcodes_matrix[ (int) curopc.id ].structType ];
		uint32 curSrcMem = ( curopc.srcmem.t32 << 6 );

		//printf( "OpIdx. curSrcMem 0x%08X\n", curSrcMem );

		for ( int i = 0; i < argLengths.argsAmount; i++ ) {
			uint_fast8_t curLength = argLengths.lengths[ i ];

			curopc.args[ i ] = ( curSrcMem >> ( 32 - curLength ) );
			curSrcMem = ( curSrcMem << curLength );
			//printf( "arg %i: 0x%04X. curSrcMem 0x%08X >> %i\n", i, curopc.args[ i ], curSrcMem, 32 - curLength );
		}

		// Debug tracing (seems to be temporal):
		/*printf( " [Trace 0x%04X: \"%5s ", proc.instructionptr, opcodes_matrix[ (int) curopc.id ].name );

		for ( int i = 0; i < argLengths.argsAmount - 1; i++ )
			printf( "%5Xh, ", curopc.args[ i ] );

		if ( argLengths.argsAmount > 0 )
			printf( "%5Xh", curopc.args[ argLengths.argsAmount - 1 ] );

		puts( "\"]" );*/

		opcodeCall( opcode->address );

		//printf( " [\\Post regs: ra 0x%X; a0..a2 {0x%X, 0x%X, 0x%X}; t0..t2 {0x%X, 0x%X, 0x%X}]", proc.ra, proc.a0, proc.a1, proc.a2, proc.t0, proc.t1, proc.t2 );
		//printFlags();

		if ( ( proc.flags & ( TF | EndEmulF ) ) == TF ) {
			curopc.args[ 0 ] = traceExceptionIndex;
			opcodeCall( op_int );
		} else if ( proc.flags & PreTF ) {
			proc.flags = ( proc.flags | TF ) & ~PreTF;
		}

		// Videopage handling:
		if ( ( proc.flags & VUF ) && BIOS_OUTPUT_VIDEOPAGE == ( ( *screenvar & 0xFF0000 ) >> 16 ) ) {
			clrscr();

			char outline[ 82 ] = { 0 };

			for ( int i = 0; i < 25; i++ ) {
				memcpy( outline, &mem[ MEM_VIDEOPAGE_START + i * 80 ], 80 );
#ifdef LINUX_OS
				for ( int j = 0; j < 80; j++ )
					if ( outline[ j ] < ' ' )
						outline[ j ] = ' ';

				printf( "%s", outline );
#else
				fwrite( outline, 80, sizeof( char ), stdout );
#endif
				puts( "|" );
			}

			// Lower screen border:
			memset( outline, '-', 80 );
			outline[ 80 ] = '/';
			puts( outline );

			proc.flags &= ~VUF;
		}

		proc.instructionptr += RISC_INSTRUCTION_LENGTH;
		proc.zero = 0;

		// Emergency exit check:
		if ( --exit_countdown <= 0 ) {
			printf( "main(). Exit downcounter (%i) reaches zero.\n", MAX_EXIT_DOWNCOUNTER );
			proc.flags |= EndEmulF;
		}
	} // of while ( !( proc.flags & EndEmulF ) ) {}

#ifdef LINUX_NCURSES
	printf( "\nPress any key to exit from the ncurses mode... " );
	getch();
	endwin();
#endif

	return 0;
}
