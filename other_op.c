#include "defines.h"
#include "internal.h"

/* Other opcodes:
	* int
	* reti
	nop
	brkp
	stf
	clf
*/

void opcodeCall( opcode_pointer );


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

		// Interrupts are overridden only if address value in the IVT is not equal to zero:
		if ( interruptCodePtr == 0 && intdata->defaddress ) {
			intdata->defaddress();
			proc.flags |= ( proc.sf & TF );

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
	if ( proc.sf & TF )
		proc.flags = ( proc.sf & ~TF ) | PostTF;
	else
		proc.flags = proc.sf;

	proc.instructionptr = proc.ra + RISC_INSTRUCTION_LENGTH;
}

OPCODE( nop ) {}

OPCODE( brkp ) {
	except_breakpoint();
}

OPCODE( read_bva ) {
	static int bvaindex = 0;
	REGARG( 0 ) = proc.bva[ bvaindex ];
	bvaindex = ( bvaindex + 1 ) % 64;
}
OPCODE( readwrite_tlb ) {
	if ( REGARG( 1 ) < MAX_TLB ) {
		if ( REGARG( 2 ) == 0 )
			proc.tlb[ REGARG( 1 ) ] = REGARG( 0 );
		else
			REGARG( 0 ) = proc.tlb[ REGARG( 1 ) ];
	} else {
		proc.ra = proc.instructionptr;
		curopc.args[ 0 ] = findInterruptMatrixIndex( except_tlb );
		opcodeCall( op_int );
	}
}

OPCODE( setflag )	{ proc.flags |= ( curopc.args[ 0 ] & FLAGS_Storable ); }
OPCODE( clearflag )	{ proc.flags &= ~( curopc.args[ 0 ] & FLAGS_Storable ); }

