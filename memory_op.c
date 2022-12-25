#include "defines.h"
#include "internal.h"

/* Memory control opcodes: */

#define INITMEMOPCODE(memaddr) uint32 addr = (memaddr);\
if ( !( proc.flags & RlModeF ) && (addr) <= MEM_KERNEL_END ) {\
	int tmp = proc.a0;\
	proc.a0 = addr;\
	curopc.args[ 0 ] = findInterruptMatrixIndex( except_not_real_mode );\
	op_int();\
	proc.a0 = tmp;\
} else


OPCODE( mov_const ) {
	REGARG( 0 ) = curopc.args[ 1 ];
}
OPCODE( mov_const_lo ) {
	REGARG( 0 ) = ( 0xFFFF0000 & REGARG( 0 ) ) | ( curopc.args[ 1 ] & 0xFFFF );
}

OPCODE( load_byte ) {
	INITMEMOPCODE( REGARG( 1 ) ) {
		REGARG( 0 ) = (unsigned char) ( mem[ addr ] + curopc.args[ 2 ] );
		REGARG( 1 )++;
	}
}
OPCODE( load_word ) {
	INITMEMOPCODE( REGARG( 1 ) ) {
		REGARG( 0 ) = ( * (uint16 *) &mem[ addr ] ) + curopc.args[ 2 ];
		REGARG( 1 ) += 2;
	}

	/*printf( "load_word(). mem[ 0x%X ] = 0x%02X. Add %i\n", curopc.args[ 1 ], * (uint16 *) &mem[ curopc.args[ 1 ] ], curopc.args[ 2 ] );
	for ( uint32 i = curopc.args[ 1 ]; i < curopc.args[ 1 ] + 2; i++ )
		printf( "0x%02X ", (unsigned char) mem[ i ] );
	puts( "" );*/
}
OPCODE( load_dword ) {
	INITMEMOPCODE( REGARG( 1 ) ) {
		REGARG( 0 ) = ( * (uint32 *) &mem[ addr ] ) + curopc.args[ 2 ];
		REGARG( 1 ) += 4;
	}

	/*printf( "load_dword(). mem[ 0x%X ] = 0x%02X. Add %i\n", curopc.args[ 1 ], * (uint32 *) &mem[ curopc.args[ 1 ] ], curopc.args[ 2 ] );
	for ( uint32 i = curopc.args[ 1 ]; i < curopc.args[ 1 ] + 4; i++ )
		printf( "0x%02X ", (unsigned char) mem[ i ] );
	puts( "" );*/
}
OPCODE( load_dw_addr ) {
	INITMEMOPCODE( curopc.args[ 1 ] )
		REGARG( 0 ) = ( * (uint32 *) &mem[ addr ] );
}


OPCODE( store_lbyte ) {
	INITMEMOPCODE( REGARG( 0 ) ) {
		mem[ addr ] = REGARG( 1 ) & 0xFF;
		REGARG( 0 )++;
	}
	//printf( "mem[ 0x%X ] = 0x%02X (really saved 0x%02X '%c')\n", curopc.args[ 0 ], REGARG( 1 ), (char) mem[ curopc.args[ 0 ] ], (char) mem[ curopc.args[ 0 ] ] );
}
OPCODE( store_hbyte ) {
	INITMEMOPCODE( REGARG( 0 ) ) {
		mem[ addr ] = ( REGARG( 1 ) >> 8 ) & 0xFF;
		REGARG( 0 )++;
	}
}
OPCODE( store_word ) {
	INITMEMOPCODE( REGARG( 0 ) ) {
		* (uint16 *) &mem[ addr ] = REGARG( 1 ) & 0xFFFF;
		REGARG( 0 ) += 2;
	}

	/*printf( "store_word(). mem[ 0x%X ] = 0x%02X (really saved 0x%02X '%c')\n", curopc.args[ 0 ], REGARG( 1 ), * (uint16 *) &mem[ curopc.args[ 0 ] ], (unsigned char) mem[ curopc.args[ 0 ] ] );
	for ( uint32 i = curopc.args[ 0 ]; i < curopc.args[ 0 ] + 2; i++ )
		printf( "0x%02X ", (unsigned char) mem[ i ] );
	puts( "" );
	getch();*/
}
OPCODE( store_dword ) {
	INITMEMOPCODE( REGARG( 0 ) ) {
		* (uint32 *) &mem[ addr ] = REGARG( 1 );
		REGARG( 0 ) += 4;
	}

	/*printf( "store_dword(). mem[ 0x%X ] = 0x%02X (really saved 0x%02X '%c')\n", curopc.args[ 0 ], REGARG( 1 ), * (uint32 *) &mem[ curopc.args[ 0 ] ], (unsigned char) mem[ curopc.args[ 0 ] ] );
	for ( uint32 i = curopc.args[ 0 ]; i < curopc.args[ 0 ] + 4; i++ )
		printf( "0x%02X ", (unsigned char) mem[ i ] );
	puts( "" );*/
}
OPCODE( store_dw_addr ) {
	INITMEMOPCODE( curopc.args[ 0 ] )
		* (uint32 *) &mem[ addr ] = REGARG( 1 );
}


// Floating-point coprocessor:

OPCODE( transfer_int2float ) {
	if ( proc.flags & FDDF ) {
		if ( checkXDoubleReg( 0 ) ) {
			curopc.args[ 0 ] = findInterruptMatrixIndex( except_invalid_opcode );
			op_int();
		} else {
			ints_or_double doubleint;
			doubleint.int0 = REGARG( 1 );
			doubleint.int1 = REGARG( 2 );

			F2DOUBLEREG( FREGARG( 0 ) ) = doubleint.d;
			checkDoubleFlags( F2DOUBLEREG( FREGARG( 0 ) ) );
		}
	} else {
		FREGARG( 0 ) = * ((float *) &REGARG( 1 ));
		checkDoubleFlags( FREGARG( 0 ) );
	}
}

OPCODE( transfer_float2int ) {
	if ( proc.flags & FDDF ) {
		if ( checkXDoubleReg( 2 ) ) {
			curopc.args[ 0 ] = findInterruptMatrixIndex( except_invalid_opcode );
			op_int();
		} else {
			ints_or_double doubleint;
			doubleint.d = * ((double *) &FREGARG( 2 ));

			REGARG( 0 ) = doubleint.int0;
			REGARG( 1 ) = doubleint.int1;
		}
	} else {
		REGARG( 0 ) = * ((uint32 *) &FREGARG( 1 ));
	}
}

OPCODE( freg_precision ) {
	switch ( REGARG( 2 ) ) {
		case 1:
			FREGARG( 0 ) = (float)( F2DOUBLEREG( FREGARG( 1 ) ) );
			checkDoubleFlags( FREGARG( 0 ) );
			break;
		case 2:
			F2DOUBLEREG( FREGARG( 0 ) ) = (double) FREGARG( 1 );
			checkDoubleFlags( F2DOUBLEREG( FREGARG( 0 ) ) );
			break;
		default:
			curopc.args[ 0 ] = findInterruptMatrixIndex( except_invalid_opcode );
			op_int();
			break;
	}
}
