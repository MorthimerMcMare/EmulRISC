#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

/*  An option that only makes sense for the Linux; more precisely, switching 
  between direct terminal or ncurses output: */
//#define USE_NCURSES


// CMake will be here... Someday...

#include "defines.h"	// Constants, defines, ...
#include "internal.h"	// Core and memory data.
#include "opmatrix.h"	// Opcodes matrix.

#include "bios.c"		// QuasiBIOS functions.
#include "memory_op.c"	// Memory opcodes.
#include "arithm_op.c"	// Math opcodes (both for integers and float-point digits).
#include "logic_op.c"	// Logical opcodes.
#include "jumps_op.c"	// Jump, call and branch opcodes.
#include "other_op.c"	// Other opcodes: int, reti, nop, brkp, stf, clf, ...


// ====================
//     Emulator/VM
// ====================

void opcodeCall( opcode_pointer opcode ) {
#if 0
	// Debug tracing (seems to be temporal):
	printf( " [Trace 0x%04X: \"%5s ", proc.instructionptr, opcodes_matrix[ (int) curopc.id ].name );

	//for ( int i = 0; i < argLengths.argsAmount - 1; i++ )
	//	printf( "%5Xh, ", curopc.args[ i ] );

	//if ( argLengths.argsAmount > 0 )
	//	printf( "%5Xh", curopc.args[ argLengths.argsAmount - 1 ] );

	puts( "\"]" );

	opcode();

	printf( " [\\Post regs: ra 0x%X; a0..a2 {0x%X, 0x%X, 0x%X}; t0..t2 {0x%X, 0x%X, 0x%X}; sf 0x%X]", proc.ra, proc.a0, proc.a1, proc.a2, proc.t0, proc.t1, proc.t2, proc.sf );
	printFlags();

	//if ( getch() == 'Q' )
	//	proc.flags |= EndEmulF;
#else
	opcode();
#endif
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
					getch();
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
			getch();
		}
	} else {
		puts( "queueInstruction(). Warning: tried to add an opcode during VM execution." );
	}
} // of void queueInstruction( opcode_pointer opcode, const uint32 opcargs[ static const 4 ] ) {}

void setMemoryArray( uint32 address, char *source, uint16 amount ) {
	memcpy( &mem[ address ], source, amount );
}

void setMemoryString( uint32 address, char *source ) {
	strcpy( &mem[ address ], source );
}


int main( void ) {
	proc.protectedModeMemStart = MEM_PROG_KERNEL_START;
	proc.flags = IF | RlModeF;

#ifdef LINUX_NCURSES
	initscr();
	noecho();
#endif

	// Tracer initialization and code:
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 10, MEM_INTERRUPTS_START } );
	queueInstruction( op_store_dword, ( uint32[ 4 ] ){ MEM_INTERRUPT_VECTOR_TABLE_EXCEPTIONS_START + findInterruptMatrixIndex( except_trace ) * 4, 10 } );

	for ( int i = 0; i < opcodes_matrix_size; i++ )
		setMemoryArray( MEM_INTERRUPTS_MEM_OPCODENAMES_START + i * 4, getOpcodeData( i )->name, 4 );

	//for ( int i = MEM_INTERRUPTS_MEM_OPCODENAMES_START; i < MEM_INTERRUPTS_MEM_OPCODENAMES_END; i++ )
	//	putchar( mem[ i ] );
	//getch();

	setMemoryArray( MEM_INTERRUPTS_MEM_OPCODENAMES_START - 16, "[Trace] 0x\x0", 11 );
	setMemoryArray( MEM_INTERRUPTS_MEM_OPCODENAMES_START - 5, "    \x0", 5 );

	uint32 savedInstructionPos = proc.protectedModeMemStart;
	proc.protectedModeMemStart = MEM_INTERRUPTS_START + findInterruptMatrixIndex( except_trace ) * 4 - 8;

	// 256 bytes: 48 opcodes, 16 variables.
	uint32 interruptMemory = proc.protectedModeMemStart + 48 * 4;
	//queueInstruction( op_brkp, ( uint32[ 4 ] ){ 0 } );
	queueInstruction( op_store_dword, ( uint32[ 4 ] ){ interruptMemory, 1 } );
	queueInstruction( op_store_dword, ( uint32[ 4 ] ){ interruptMemory + 4, 10 } );
	queueInstruction( op_store_dword, ( uint32[ 4 ] ){ interruptMemory + 8, 11 } );
	queueInstruction( op_store_dword, ( uint32[ 4 ] ){ interruptMemory + 12, 12 } );
	queueInstruction( op_store_word, ( uint32[ 4 ] ){ interruptMemory + 16, 2 } );

	queueInstruction( op_load_dword, ( uint32[ 4 ] ){ 10, MEM_KERNELVARS_BIOS_SCREEN } );
	queueInstruction( op_store_dword, ( uint32[ 4 ] ){ interruptMemory + 20, 10 } );

	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 10, 4 } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 80 + 54 } );
	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( bios_videomemory ) } ); // Moves cursor (subfunc 4h).

	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 10, MEM_INTERRUPTS_MEM_OPCODENAMES_START - 16 } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 0 } );
	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( bios_print ) } ); // Prints a string ("[Trace] ").

	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 10, interruptMemory } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 4 } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 12, 16 } );
	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( bios_printdigit ) } ); // Prints a digit (return address).
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 10, MEM_INTERRUPTS_MEM_OPCODENAMES_START - 3 } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 0 } );
	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( bios_print ) } ); // Prints a space.

	queueInstruction( op_load_word, ( uint32[ 4 ] ){ 10, MEM_KERNELVARS_TRACE_CUROPCODE } );
	queueInstruction( op_shl_const, ( uint32[ 4 ] ){ 10, 10, 2 } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, MEM_INTERRUPTS_MEM_OPCODENAMES_START } );
	queueInstruction( op_add, ( uint32[ 4 ] ){ 10, 10, 11 } );

	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 0 } );
	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( bios_print ) } ); // Prints a string (opcode name).

	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 10, MEM_INTERRUPTS_MEM_OPCODENAMES_START - 5 } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 0 } );
	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( bios_print ) } ); // Prints a string (four spaces here).
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 2 } );
	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( bios_print ) } ); // Prints a newline.

	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( bios_getkey ) } ); // Waits for the keystroke.

	queueInstruction( op_load_dword, ( uint32[ 4 ] ){ 10, interruptMemory + 20 } );
	queueInstruction( op_store_dword, ( uint32[ 4 ] ){ MEM_KERNELVARS_BIOS_SCREEN, 10 } ); // Reverts X/Y screen coords back.

	queueInstruction( op_load_word, ( uint32[ 4 ] ){ 2, interruptMemory + 16 } );
	queueInstruction( op_load_dword, ( uint32[ 4 ] ){ 12, interruptMemory + 12 } );
	queueInstruction( op_load_dword, ( uint32[ 4 ] ){ 11, interruptMemory + 8 } );
	queueInstruction( op_load_dword, ( uint32[ 4 ] ){ 10, interruptMemory + 4 } );
	queueInstruction( op_load_dword, ( uint32[ 4 ] ){ 1, interruptMemory } );

	//queueInstruction( op_brkp, ( uint32[ 4 ] ){ 0 } );
	queueInstruction( op_int_ret, ( uint32[ 4 ] ){ 0 } );
	//queueInstruction( op_call_reg, ( uint32[ 4 ] ){ 0, 1, 0 } );


	proc.protectedModeMemStart = savedInstructionPos;

	//queueInstruction( op_clearflag, ( uint32[ 4 ] ){ RlModeF } );

	// Test program:
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 10, 100 } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 28 } );

	queueInstruction( op_sub, ( uint32[ 4 ] ){ 10, 10, 11 } );		// 'H'
	queueInstruction( op_add_const, ( uint32[ 4 ] ){ 11, 11, 77 } );	// 'i'

	queueInstruction( op_store_lbyte, ( uint32[ 4 ] ){ 0x60001, 11 } );
	queueInstruction( op_store_lbyte, ( uint32[ 4 ] ){ 0x60000, 10 } );

	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 10, '!' } );	// '!'
	queueInstruction( op_store_lbyte, ( uint32[ 4 ] ){ 0x60002, 10 } );

	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 10, 0 } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 2 } );
	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( bios_videomemory ) } ); // Videomemory output.

	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 10, 4 } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 999 } );
	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( bios_videomemory ) } ); // Moves cursor (subfunc 4h).
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 10, 0x60000 } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 0 } );

	//queueInstruction( op_setflag, ( uint32[ 4 ] ){ TF } );
	queueInstruction( op_jmp_const, ( uint32[ 4 ] ){ 2 } );
	queueInstruction( op_nop, ( uint32[ 4 ] ){ 0 } );				// Will be skipped;
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 1, 0xABCD } );	// Will be skipped.

	//queueInstruction( op_clearflag, ( uint32[ 4 ] ){ TF } );
	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( bios_print ) } ); // Prints a string (subfunc 0h).
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 2 } );
	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( bios_print ) } ); // A newline character (subfunc 2h).

	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 6, 1000 } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 5, 4 } );
	queueInstruction( op_div, ( uint32[ 4 ] ){ 6, 5, 5, 0 } );
	queueInstruction( op_store_dword, ( uint32[ 4 ] ){ 0x60000, 5 } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 10, 0x60000 } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 12, 10 } );
	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( bios_printdigit ) } );

	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 10, 4 } ); 	
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 80 * 24 } );
	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( bios_videomemory ) } ); // Moves cursor (subfunc 4h).

	setMemoryString( 0x60020, "(Press 'q' to exit...)" );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 10, 0x60020 } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 0 } );
	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( bios_print ) } );

	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( bios_getkey ) } ); // Waits for the keystroke.
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 'q' } );
	queueInstruction( op_beq_near, ( uint32[ 4 ] ){ 10, 11, 4 } );
	queueInstruction( op_mov_const, ( uint32[ 4 ] ){ 11, 'Q' } );
	queueInstruction( op_beq_near, ( uint32[ 4 ] ){ 10, 11, 2 } );
	queueInstruction( op_jmp_const, ( uint32[ 4 ] ){ 0xFFFF - 5 } );

	queueInstruction( op_int, ( uint32[ 4 ] ){ FINDINT( except_end_emulation ) } ); // Ends emulation (via quasiBIOS).


	puts( "================" );
	proc.protectedModeMemStart += ( 1024 - proc.protectedModeMemStart ) % 1024;
	proc.instructionptr = MEM_PROG_KERNEL_START;

	const int MAX_EXIT_DOWNCOUNTER = 386;
	int exit_countdown = MAX_EXIT_DOWNCOUNTER;
	//uint32 last_opcode_id_exitcheck = 0;

	uint32 traceExceptionIndex = FINDINT( except_trace );
	uint32 prevInstrutionPtr = proc.instructionptr;

	while ( !( proc.flags & EndEmulF ) ) {
		if ( proc.instructionptr >= MAX_MEM ) {
			printf( "main(). Instruction pointer is out-of-bounds (0x%08X, max mem 0x%08X).\n", proc.instructionptr, MAX_MEM );
			proc.flags |= EndEmulF;

		} else if ( !( proc.flags & RlModeF ) && proc.instructionptr <= MEM_KERNEL_END ) {
			int tmp = proc.a0;
			proc.a0 = proc.instructionptr;
			curopc.args[ 0 ] = findInterruptMatrixIndex( except_not_real_mode );
			op_int();
			proc.a0 = tmp;

		} else {
			// Parsing:
			curopc.srcmem.t32 = * (uint32 *) &mem[ proc.instructionptr ];
			curopc.id = ( curopc.srcmem.t32 >> 26 );

			opcode_data *opcode = getOpcodeData( curopc.id );

			opcode_structtype_length argLengths = opcode_structtype_lengths[ opcodes_matrix[ (int) curopc.id ].structType ];
			uint32 curSrcMem = ( curopc.srcmem.t32 << 6 );

			//printf( "OpcIdx. curSrcMem 0x%08X (at 0x%04X)\n", curSrcMem, proc.instructionptr );

			for ( int i = 0; i < argLengths.argsAmount; i++ ) {
				uint_fast8_t curLength = argLengths.lengths[ i ];

				curopc.args[ i ] = ( curSrcMem >> ( 32 - curLength ) );
				curSrcMem = ( curSrcMem << curLength );
				//printf( "arg %i: 0x%04X. curSrcMem 0x%08X >> %i\n", i, curopc.args[ i ], curSrcMem, 32 - curLength );
			}

			for ( int i = argLengths.argsAmount; i < 4; i++ )
				curopc.args[ i ] = 0;

			opcodeCall( opcode->address );

			// Trace interrupt call:
			if ( ( proc.flags & ( TF | EndEmulF ) ) == TF ) {
				mem[ MEM_KERNELVARS_TRACE_CUROPCODE ] = curopc.id;
				mem[ MEM_KERNELVARS_TRACE_CUROPCODE_ARGSAMOUNT ] = argLengths.argsAmount;
				mem[ MEM_KERNELVARS_TRACE_CUROPCODE_ARGSTYPE ] = opcodes_matrix[ (int) curopc.id ].structType;

				for ( int i = 0; i < 4; i++ )
					* (uint32 *) &mem[ MEM_KERNELVARS_TRACE_OPCARG_START + i * 4 ] = curopc.args[ i ];

				//printf( " [TF. ID %i, uint[ %i ] args type %i.\n", mem[ MEM_KERNELVARS_TRACE_CUROPCODE ], mem[ MEM_KERNELVARS_TRACE_CUROPCODE_ARGSAMOUNT ], mem[ MEM_KERNELVARS_TRACE_CUROPCODE_ARGSTYPE ] );

				int tmp = curopc.args[ 0 ];
				curopc.args[ 0 ] = traceExceptionIndex;
				opcodeCall( op_int );
				curopc.args[ 0 ] = tmp;
			}
		}

		if ( proc.flags & PostTF )
			proc.flags = ( proc.flags & ~PostTF ) | TF;


		// Videopage handling:
		if ( ( proc.flags & VUF ) && BIOS_OUTPUT_VIDEOPAGE == ( ( *screenvar & 0xFF0000 ) >> 16 ) ) {
			clrscr();

			char outline[ 82 ] = { 0 };

			for ( int i = 0; i < 25; i++ ) {
				memcpy( outline, &mem[ MEM_VIDEOPAGE_START + i * 80 ], 80 );
#ifdef LINUX_OS
				// It's strange, but "fwrite()" on Linux seems to output only non-null characters.
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

		if ( proc.instructionptr == prevInstrutionPtr )
			proc.instructionptr += RISC_INSTRUCTION_LENGTH;

		prevInstrutionPtr = proc.instructionptr;
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
