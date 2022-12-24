#include <math.h>
#include "defines.h"
#include "internal.h"

/* Math opcodes (both for integers and float-point digits): */

void opcodeCall( opcode_pointer );


// Integers:

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

OPCODE( div ) {
	proc.flags &= ~( ZF | SF );

	if ( REGARG( 1 ) == 0 ) {
		proc.ra = proc.instructionptr;
		curopc.args[ 0 ] = findInterruptMatrixIndex( except_zero_division );
		opcodeCall( op_int );
	} else {
		int64 result = (int32) REGARG( 0 ) / (int32) REGARG( 1 );

		* (int32 *) &REGARG( 2 ) = result & 0xFFFFFFFF;
		* (int32 *) &REGARG( 3 ) = ( result >> 32 ) & 0xFFFFFFFF;

		if ( result == 0 )
			proc.flags |= ZF;
		else if ( result < 0 )
			proc.flags |= SF;
	}
}

OPCODE( div_u ) {
	if ( REGARG( 1 ) == 0 ) {
		proc.ra = proc.instructionptr;
		curopc.args[ 0 ] = findInterruptMatrixIndex( except_zero_division );
		opcodeCall( op_int );
	} else {
		uint64 result = (uint32) REGARG( 0 ) / (uint32) REGARG( 1 );

		REGARG( 2 ) = result & 0xFFFFFFFF;
		REGARG( 3 ) = ( result >> 32 ) & 0xFFFFFFFF;

		checkZeroFlag( result );
	}
}


// Floating-point:

#define FLOATPOINTOP_PART1 \
	proc.flags &= ~( ZF | SF | FINF | FXXF );\
	if ( proc.flags & FDDF ) {\
		if ( checkXDoubleReg( 0 ) || checkXDoubleReg( 1 ) || checkXDoubleReg( 2 ) ) {\
			curopc.args[ 0 ] = findInterruptMatrixIndex( except_invalid_opcode );\
			op_int();\
		} else {\
			F2DOUBLEREG( FREGARG( 0 ) ) = F2DOUBLEREG( FREGARG( 1 ) )

#define FLOATPOINTOP_PART2 \
			F2DOUBLEREG( FREGARG( 2 ) );\
			checkDoubleFlags( F2DOUBLEREG( FREGARG( 0 ) ) );\
		}\
	} else {\
		FREGARG( 0 ) = FREGARG( 1 )

#define FLOATPOINTOP_PART3 \
		FREGARG( 2 );\
		checkDoubleFlags( FREGARG( 0 ) );\
	}



OPCODE( fadd ) {
	FLOATPOINTOP_PART1 + FLOATPOINTOP_PART2 + FLOATPOINTOP_PART3;
}

OPCODE( fsub ) {
	FLOATPOINTOP_PART1 - FLOATPOINTOP_PART2 - FLOATPOINTOP_PART3;
}

OPCODE( fmul ) {
	FLOATPOINTOP_PART1 * FLOATPOINTOP_PART2 * FLOATPOINTOP_PART3;
}

OPCODE( fdiv ) {
	if ( FREGARG( 2 ) == 0.0 ) {
		curopc.args[ 0 ] = findInterruptMatrixIndex( except_zero_division );
		op_int();
	} else {
		FLOATPOINTOP_PART1 / FLOATPOINTOP_PART2 / FLOATPOINTOP_PART3;
	}
}
