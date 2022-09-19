//
// QuasiBIOS functions.
//

#ifndef __RISCEMUL_QUASIBIOS_H
#define __RISCEMUL_QUASIBIOS_H


// Prints a string to the stdout (aux.).
//   Reg0: index of the first memory char cell;
//   Reg1: maximal amount of symbols.
BIOSOPCODE( printstr ) {
	char *curchar = &mem[ proc.a0 ];

	uint32 maxchars = proc.a1;
	if ( maxchars == 0 )
		maxchars = UINT32_MAX;

	//printf( "op_bios_printstr(). maxchars %u, curchar %i. Mem[0x%X..+10] == ", maxchars, *curchar, proc.a0 );
	//for ( uint32 i = 0; i < 10; i++ )
	//	printf( "%02X ", mem[ i + proc.a0 ] );
	//puts( "" );

	for ( uint32 i = maxchars; *curchar != '\x0' && i > 0; curchar++, i-- )
		putchar( *curchar );

	puts( "" );
}


#endif // of #ifndef __RISCEMUL_QUASIBIOS_H
