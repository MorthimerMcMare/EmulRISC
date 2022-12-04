#include "defines.h"
#include "internal.h"

/* Jump, call and branch opcodes: */

OPCODE( call_const ) {	// "JAL r_saveto, const_offset".
	REGARG( 0 ) = proc.instructionptr + RISC_INSTRUCTION_LENGTH;
	proc.instructionptr = curopc.args[ 1 ];
}
OPCODE( call_reg ) {	// "JALR r_saveto, r_offset, const_offset".
	REGARG( 0 ) = proc.instructionptr + RISC_INSTRUCTION_LENGTH;
	proc.instructionptr = REGARG( 1 ) + curopc.args[ 2 ];
}

OPCODE( jmp_near ) {
	//printf( "jmp_near(). prev 0x%X, add %i, new 0x%X\n", proc.instructionptr, ( (int16) curopc.args[ 0 ] ) * RISC_INSTRUCTION_LENGTH, proc.instructionptr + ( (int16) curopc.args[ 0 ] ) * RISC_INSTRUCTION_LENGTH );
	proc.instructionptr += ( (int16) curopc.args[ 0 ] ) * RISC_INSTRUCTION_LENGTH;
	//getch();
}
OPCODE( jmp_far ) {
	proc.instructionptr += RISC_INSTRUCTION_LENGTH * (int32) REGARG( 0 );
}

OPCODE( jf_near ) { if ( proc.flags & REGARG( 0 ) ) proc.instructionptr += ( (int16) curopc.args[ 1 ] ) * RISC_INSTRUCTION_LENGTH; }
OPCODE( jnf_near ) { if ( !( proc.flags & REGARG( 0 ) ) ) proc.instructionptr += ( (int16) curopc.args[ 1 ] ) * RISC_INSTRUCTION_LENGTH; }
OPCODE( jf_far ) { if ( proc.flags & REGARG( 0 ) ) proc.instructionptr = REGARG( 1 ); }
OPCODE( jnf_far ) { if ( !( proc.flags & REGARG( 0 ) ) ) proc.instructionptr = REGARG( 1 ); }

// printf( "op_beq_near(). [r0] %i == [r1] %i? new pos 0x%04X\n", REGARG( 0 ), REGARG( 1 ), proc.instructionptr + (int) curopc.args[ 2 ] * RISC_INSTRUCTION_LENGTH );
OPCODE( beq_near ) { if ( REGARG( 0 ) == REGARG( 1 ) ) proc.instructionptr += ( (int16) curopc.args[ 2 ] ) * RISC_INSTRUCTION_LENGTH; }
OPCODE( bne_near ) { if ( REGARG( 0 ) != REGARG( 1 ) ) proc.instructionptr += ( (int16) curopc.args[ 2 ] ) * RISC_INSTRUCTION_LENGTH; }
OPCODE( beq_far ) { if ( REGARG( 0 ) == REGARG( 1 ) ) proc.instructionptr = REGARG( 2 ); }
OPCODE( bne_far ) { if ( REGARG( 0 ) != REGARG( 1 ) ) proc.instructionptr = REGARG( 2 ); }

OPCODE( blt_near ) { if ( (int32) REGARG( 0 ) <  (int32) REGARG( 1 ) ) proc.instructionptr += ( (int16) curopc.args[ 2 ] ) * RISC_INSTRUCTION_LENGTH; }
OPCODE( bge_near ) { if ( (int32) REGARG( 0 ) >= (int32) REGARG( 1 ) ) proc.instructionptr += ( (int16) curopc.args[ 2 ] ) * RISC_INSTRUCTION_LENGTH; }
OPCODE( blt_far ) { if ( (int32) REGARG( 0 ) <  (int32) REGARG( 1 ) ) proc.instructionptr = REGARG( 2 ); }
OPCODE( bge_far ) { if ( (int32) REGARG( 0 ) >= (int32) REGARG( 1 ) ) proc.instructionptr = REGARG( 2 ); }

OPCODE( bltu_near ) { if ( (uint32) REGARG( 0 ) <  (uint32) REGARG( 1 ) ) proc.instructionptr += ( (int16) curopc.args[ 2 ] ) * RISC_INSTRUCTION_LENGTH; }
OPCODE( bgeu_near ) { if ( (uint32) REGARG( 0 ) >= (uint32) REGARG( 1 ) ) proc.instructionptr += ( (int16) curopc.args[ 2 ] ) * RISC_INSTRUCTION_LENGTH; }
OPCODE( bltu_far ) { if ( (uint32) REGARG( 0 ) <  (uint32) REGARG( 1 ) ) proc.instructionptr = REGARG( 2 ); }
OPCODE( bgeu_far ) { if ( (uint32) REGARG( 0 ) >= (uint32) REGARG( 1 ) ) proc.instructionptr = REGARG( 2 ); }
