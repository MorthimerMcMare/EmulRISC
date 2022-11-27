#include "defines.h"
#include "internal.h"

/* Memory control opcodes: */

OPCODE( mov_const ) {
	REGARG( 0 ) = curopc.args[ 1 ];
}
OPCODE( load_byte ) {
	REGARG( 0 ) = (unsigned char) ( mem[ curopc.args[ 1 ] ] + curopc.args[ 2 ] );
}
OPCODE( load_word ) {
	REGARG( 0 ) = ( * (uint16 *) &mem[ curopc.args[ 1 ] ] ) + curopc.args[ 2 ];

	/*printf( "load_word(). mem[ 0x%X ] = 0x%02X. Add %i\n", curopc.args[ 1 ], * (uint16 *) &mem[ curopc.args[ 1 ] ], curopc.args[ 2 ] );
	for ( uint32 i = curopc.args[ 1 ]; i < curopc.args[ 1 ] + 2; i++ )
		printf( "0x%02X ", (unsigned char) mem[ i ] );
	puts( "" );*/
}
OPCODE( load_dword ) {
	REGARG( 0 ) = ( * (uint32 *) &mem[ curopc.args[ 1 ] ] ) + curopc.args[ 2 ];

	/*printf( "load_dword(). mem[ 0x%X ] = 0x%02X. Add %i\n", curopc.args[ 1 ], * (uint32 *) &mem[ curopc.args[ 1 ] ], curopc.args[ 2 ] );
	for ( uint32 i = curopc.args[ 1 ]; i < curopc.args[ 1 ] + 4; i++ )
		printf( "0x%02X ", (unsigned char) mem[ i ] );
	puts( "" );*/
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

	/*printf( "store_word(). mem[ 0x%X ] = 0x%02X (really saved 0x%02X '%c')\n", curopc.args[ 0 ], REGARG( 1 ), * (uint16 *) &mem[ curopc.args[ 0 ] ], (unsigned char) mem[ curopc.args[ 0 ] ] );
	for ( uint32 i = curopc.args[ 0 ]; i < curopc.args[ 0 ] + 2; i++ )
		printf( "0x%02X ", (unsigned char) mem[ i ] );
	puts( "" );
	getch();*/
}
OPCODE( store_dword ) {
	* (uint32 *) &mem[ curopc.args[ 0 ] ] = proc.regs[ curopc.args[ 1 ] ];

	/*printf( "store_dword(). mem[ 0x%X ] = 0x%02X (really saved 0x%02X '%c')\n", curopc.args[ 0 ], REGARG( 1 ), * (uint32 *) &mem[ curopc.args[ 0 ] ], (unsigned char) mem[ curopc.args[ 0 ] ] );
	for ( uint32 i = curopc.args[ 0 ]; i < curopc.args[ 0 ] + 4; i++ )
		printf( "0x%02X ", (unsigned char) mem[ i ] );
	puts( "" );*/
}
