//
// Exceptions and quasiBIOS functions.
//
// (...Yes, I perfectly know that it's mauvais ton to write
//code in header files).
//

#ifndef __RISCEMUL_QUASIBIOS_H
#define __RISCEMUL_QUASIBIOS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include "defines.h"


EXCEPTIONOPCODE( zero_division ) {
	puts( "Division by zero." );
	proc.flags |= EndEmulF;
}

EXCEPTIONOPCODE( trace ) {
	if ( 'q' == getch() )
		proc.flags &= ~TF;
}

EXCEPTIONOPCODE( breakpoint ) {
	printf( "\nBrkp at 0x%04X.\nra 0x%04X; sf 0x%04X (flags ", proc.instructionptr, proc.ra, proc.sf );

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

	printf( "),\ntmp[0..6]: 0x%04X, 0x%04X, 0x%04X; 0x%04X, 0x%04X, 0x%04X, 0x%04X\n", proc.t0, proc.t1, proc.t2, proc.t3, proc.t4, proc.t5, proc.t6 );
	printf( "arg[0..7]: 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X, 0x%04X; 0x%04X, 0x%04X\n", proc.a0, proc.a1, proc.a2, proc.a3, proc.a4, proc.a5, proc.a6, proc.a7 );

	getch();
}

EXCEPTIONOPCODE( invalid_opcode ) {
	printf( "Invalid opcode at 0x%X.\n", proc.instructionptr );
	proc.flags |= EndEmulF;
}

EXCEPTIONOPCODE( invalid_interrupt ) {
	printf( "Invalid interrupt at 0x%X.\n", proc.ra );
	proc.flags |= EndEmulF;
}

EXCEPTIONOPCODE( not_real_mode ) {
	printf( "Memory cell 0x%08X access denied: not in real mode (pc 0x%08X; rlmem ends at 0x%06X).\n", proc.a0, proc.instructionptr, MEM_KERNEL_END );
	proc.flags |= EndEmulF;
}

EXCEPTIONOPCODE( printscreen ) {}
EXCEPTIONOPCODE( irq0 ) {}
EXCEPTIONOPCODE( irq1 ) {}
EXCEPTIONOPCODE( irq2 ) {}
EXCEPTIONOPCODE( irq3 ) {}
EXCEPTIONOPCODE( irq4 ) {}
EXCEPTIONOPCODE( irq5 ) {}
EXCEPTIONOPCODE( irq6 ) {}
EXCEPTIONOPCODE( irq7 ) {}

EXCEPTIONOPCODE( end_emulation ) {
	proc.flags |= EndEmulF;
}



// QuasiBIOS output.
//   0: console direct;
//   1: console with newlines;
//   2: to videopage;
//   3: freezed videopage.
#define BIOS_OUTPUT_CONSOLE 0
#define BIOS_OUTPUT_CONSOLE_NEWLINES 1
#define BIOS_OUTPUT_VIDEOPAGE 2
#define BIOS_OUTPUT_VIDEOPAGE_FREEZED 3

uint32 internalPrintString( char *outstring ) {
	uint32 outputMode = ( *screenvar & 0xFF0000 ) >> 16;
	uint32 cursorpos;
	uint32 outAmount = 0;

	switch ( outputMode ) {
		case BIOS_OUTPUT_CONSOLE:
			outAmount = printf( "%s", outstring );
			break;
		case BIOS_OUTPUT_CONSOLE_NEWLINES:
			outAmount = printf( "%s", outstring ) + 1;
			putchar( '\n' );
			break;
		case BIOS_OUTPUT_VIDEOPAGE:
		case BIOS_OUTPUT_VIDEOPAGE_FREEZED:
			cursorpos = ( *screenvar & 0xFFFF );

			char *curoutpos = &mem[ MEM_VIDEOPAGE_START + cursorpos ];
			char *curchar = outstring;

			for ( outAmount = 0; *curchar != '\x0' && outAmount < MEM_VIDEOPAGE_TEXTAMOUNT - cursorpos; curchar++, curoutpos++, outAmount++ )
				*curoutpos = *curchar;

			mem[ MEM_KERNELVARS_BIOS_SCREEN ] += outAmount; // Absolute position in a last two bytes in this uint32 variable.

			//strncpy( (char *) &mem[ MEM_VIDEOPAGE_START + cursorpos ], outstring, MIN( MEM_VIDEOPAGE_TEXTAMOUNT - cursorpos, strlen( outstring ) ) );

			if ( outputMode == BIOS_OUTPUT_VIDEOPAGE )
				proc.flags |= VUF;
			break;
		default:
			printf( "internalPrintString(). Warning: unknown output mode '%u'.\n", outputMode );
			break;
	}

	return outAmount;
}

void internalPrintChar( char outchar ) {
	uint32 outputMode = ( *screenvar & 0xFF0000 ) >> 16;

	switch ( outputMode ) {
		case BIOS_OUTPUT_CONSOLE:
		case BIOS_OUTPUT_CONSOLE_NEWLINES:
			putchar( outchar );
			break;
		case BIOS_OUTPUT_VIDEOPAGE:
		case BIOS_OUTPUT_VIDEOPAGE_FREEZED:
			if ( ( *screenvar & 0xFFFF ) < MEM_VIDEOPAGE_TEXTAMOUNT ) {
				if ( outchar != '\n' ) {
					mem[ MEM_VIDEOPAGE_START + ( *screenvar & 0xFFFF ) ] = outchar;
					(*screenvar)++;
				} else {
					* (uint16 *) screenvar = (uint16) ( ( *screenvar & 0xFFFF ) / 80 + 1 ) * 80;
				}

				if ( outputMode == BIOS_OUTPUT_VIDEOPAGE )
					proc.flags |= VUF;
			}
			break;
		default:
			printf( "internalPrintChar(). Warning: unknown output mode '%u'.\n", outputMode );
			break;
	}
}


// Prints a string to the stdout.
//   a0: index of the first memory char cell;
//   a1: print type. 1 == char, 2 == '\n' (ignoring a0), other == null-terminated string.
//   return a6: finish memory pointer (next from last '\x0');
//   return a7: real amount of symbols (excluding last '\x0').
BIOSOPCODE( print ) {
	switch ( proc.a1 ) {
		case 1:
			internalPrintChar( mem[ proc.a0 ] );
			break;
		case 2:
			internalPrintChar( '\n' );
			break;
		default:
			proc.a6 = internalPrintString( &mem[ proc.a0 ] );
			proc.a7 = proc.a0 + proc.a1 + 1;
			break;
	}
	/*uint32 maxchars = proc.a1;
	if ( maxchars == 0 )
		maxchars = UINT32_MAX;

	printf( "op_bios_printstr(). maxchars %u, curchar %i. Mem[0x%X..+10] == ", maxchars, *curchar, proc.a0 );
	for ( uint32 i = 0; i < 10; i++ )
		printf( "%02X ", mem[ i + proc.a0 ] );
	puts( "" );

	proc.t1 = 0;

	for ( ; *curchar != '\x0' && proc.t1 < maxchars; curchar++, proc.t1++ )
		putchar( *curchar );

	proc.t0 = proc.a0 + proc.t1 + 1;*/
}

// Prints a digit base X to the stdout.
//   a0: index of the first memory char cell;
//   a1: amount of bytes ([McM]: now unused and so may be deprecated);
//   a2: destination base.
static const char *DIGITS = "0123456789ABCDEFGHIKLMNOPQRSTVXYZ";

BIOSOPCODE( printdigit ) {
	int32 digit = ( * (int32 *) &mem[ proc.a0 ] );// & ( ( 1 << proc.a1 ) - 1 );

	//printf( "bios_printdigit(). %i\n", digit ); getch();

	unsigned char fastprinted = 1;
	char to[ 65 ] = { 0 };

	switch ( proc.a2 ) {
		case 0: case 10: sprintf( to, "%i", digit ); break;
		case 8: sprintf( to, "%o", (uint32) digit ); break;
		case 16: sprintf( to, "%X", (uint32) digit ); break;
		default: fastprinted = 0; break;
	}

	if ( fastprinted ) {
		internalPrintString( to );
		return;
	}

	if ( proc.a2 >= 36 ) {
		INVALID_INTERRUPT_ARG( bios_printdigit, 36 );
		return;
	}

	proc.t1 = 0;

	char *s = to + 65;
	char sign;
	unsigned len;

	if ( digit < 0 ) {
		sign = 1;
		len = 2;
	} else {
		sign = 0;
		len = 1;
	}

	*s = '\0';
	do {
		*( --s )= *( DIGITS + ( digit % proc.a2 ) );
		digit /= proc.a2;
		len++;
	} while ( digit != 0 );

	if ( sign )
		*( --s ) = '-';

	internalPrintString( to + ( 65 - len ) );
}

BIOSOPCODE( videomemory ) {
	uint32 action = proc.a0;
	uint32 arg1 = proc.a1;
	uint32 *screenvar = (uint32 *) &mem[ MEM_KERNELVARS_BIOS_SCREEN ];

	switch ( action ) {
		case 1: // Clear screen (videopage).
			memset( &mem[ MEM_VIDEOPAGE_START ], '\x0', MEM_VIDEOPAGE_TEXTAMOUNT );
			proc.flags |= VUF;
			break;
		case 2: // Set cursor X.
			if ( arg1 < 80 )
				*screenvar = (*screenvar & ~0xFFFF) + arg1 + (int) floor( (*screenvar & 0xFFFF) / 80.0 );
			break;
		case 3: // Set cursor Y.
			if ( arg1 < 25 )
				*screenvar = (*screenvar & ~0xFFFF) + ( (*screenvar & 0xFFFF) % 80 ) + arg1 * 80;
			break;
		case 4: // Set cursor absolute X/Y.
			if ( arg1 < MEM_VIDEOPAGE_TEXTAMOUNT )
				*screenvar = ( *screenvar & ~0xFFFF ) + arg1;
			break;
		case 5: // Add cursor X.
			if ( arg1 + (*screenvar) > MEM_VIDEOPAGE_TEXTAMOUNT - 1 )
				arg1 = MEM_VIDEOPAGE_TEXTAMOUNT - 1;

			*screenvar += arg1;
			break;
		case 0: // Switch output mode (0: console direct, 1: console with newlines, 2: to videopage, 3: freezed videopage).
			if ( arg1 < 3 )
				*screenvar = ( *screenvar & ~0xFF0000 ) | ( arg1 << 16 );

			if ( arg1 == BIOS_OUTPUT_VIDEOPAGE )
				proc.flags |= VUF;
		default:
			break;
	}
}

BIOSOPCODE( getkey ) {
	proc.a0 = getch();
}

BIOSOPCODE( getstring ) {
	uint32 maxAmount = ( proc.a1 != 0? MIN( proc.a1, MAX_MEM - proc.a0 - 1 ): MAX_MEM - proc.a0 - 1 );

	fgets( &mem[ proc.a0 ], maxAmount, stdin );
}


#endif // of #ifndef __RISCEMUL_QUASIBIOS_H
