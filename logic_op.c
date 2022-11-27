#include "defines.h"
#include "internal.h"

/* Logical opcodes: */

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

