//
// Exceptions and quasiBIOS functions.
//

#ifndef __RISCEMUL_QUASIBIOS_H
#define __RISCEMUL_QUASIBIOS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
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

// Prints a string to the stdout.
//   a0: index of the first memory char cell;
//   a1: maximal amount of symbols;
//   return t0: finish memory pointer (next from last '\x0');
//   return t1: real amount of symbols (excluding last '\x0').
BIOSOPCODE( printstr ) {
	char *curchar = &mem[ proc.a0 ];

	uint32 maxchars = proc.a1;
	if ( maxchars == 0 )
		maxchars = UINT32_MAX;

	//printf( "op_bios_printstr(). maxchars %u, curchar %i. Mem[0x%X..+10] == ", maxchars, *curchar, proc.a0 );
	//for ( uint32 i = 0; i < 10; i++ )
	//	printf( "%02X ", mem[ i + proc.a0 ] );
	//puts( "" );
	proc.t1 = 0;

	for ( ; *curchar != '\x0' && proc.t1 < maxchars; curchar++, proc.t1++ )
		putchar( *curchar );

	proc.t0 = proc.a0 + proc.t1 + 1;
}

// Prints a digit base X to the stdout.
//   a0: index of the first memory char cell;
//   a1: amount of bytes;
//   a2: destination base.
static const char *DIGITS = "0123456789ABCDEFGHIKLMNOPQRSTVXYZ";

BIOSOPCODE( printdigit ) {
	int32 digit = * (int64 *) &mem[ proc.a0 ] & ( ( 1 << proc.a1 ) - 1 );

	unsigned char fastprinted = 1;

	switch ( proc.a2 ) {
		case 0: case 10: printf( "%i", digit ); break;
		case 8: printf( "%o", (uint32) digit ); break;
		case 16: printf( "%X", (uint32) digit ); break;
		default: fastprinted = 0; break;
	}

	if ( fastprinted || ( proc.a2 >= 36 ) )
		return;

	proc.t1 = 0;

	char to[ 65 ];
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

	printf( "%s", to + ( 65 - len ) );
}

BIOSOPCODE( printnewline ) {
	putchar( '\n' );
}

BIOSOPCODE( videomemory ) {}

BIOSOPCODE( getkey ) {
	proc.a0 = getch();
}

BIOSOPCODE( getstring ) {
	fgets( &mem[ proc.a0 ], MAX_MEM - proc.a0 - 1, stdin );
}


#endif // of #ifndef __RISCEMUL_QUASIBIOS_H
