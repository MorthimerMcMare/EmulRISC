//
// Constants, constant and macro defines, enumerates, typedefs, etc.
//

#ifndef __RISCEMUL_DEFINES_H
#define __RISCEMUL_DEFINES_H

#define RISC_INSTRUCTION_LENGTH 4 /* Byte length of the every one instruction. */

#define MAX_REGS 32 /* As 2^5. */
#define MAX_FLOAT_REGS 16

#define MAX_MEM (1024 * 1024) /* Statically allocated 1 MB. */

#define MEM_BIOS_START 0
#define MEM_BIOS_CURSTACKPOS 0
#define MEM_BIOS_TEXTCURSORX 4
#define MEM_BIOS_TEXTCURSORY 6
#define MEM_BIOS_OTHER 8
#define MEM_BIOS_END 255

#define MEM_INTERRUPT_VECTOR_TABLE_START 256 				/* 192 interrupts at all. */
#define MEM_INTERRUPT_VECTOR_TABLE_BIOS_START 256
#define MEM_INTERRUPT_VECTOR_TABLE_BIOS_AMOUNT 16			/* 16 "BIOS" interrupts (overridden only if address value is not "0"). */
#define MEM_INTERRUPT_VECTOR_TABLE_EXCEPTIONS_FIRST MEM_INTERRUPT_VECTOR_TABLE_BIOS_AMOUNT
#define MEM_INTERRUPT_VECTOR_TABLE_EXCEPTIONS_START (MEM_INTERRUPT_VECTOR_TABLE_BIOS_START + 4 * MEM_INTERRUPT_VECTOR_TABLE_BIOS_AMOUNT)
#define MEM_INTERRUPT_VECTOR_TABLE_EXCEPTIONS_AMOUNT 16		/* 16 exceptions interrupts. */
#define MEM_INTERRUPT_VECTOR_TABLE_USER_START (MEM_INTERRUPT_VECTOR_TABLE_EXCEPTIONS_START + 4 * MEM_INTERRUPT_VECTOR_TABLE_EXCEPTIONS_AMOUNT)
#define MEM_INTERRUPT_VECTOR_TABLE_END 1023

#define MEM_INTERRUPTS_START (1024)
#define MEM_INTERRUPTS_END (1024 * 2 - 1)

#define MEM_PROG_START (1024 * 2)

// Relative to protected mode memory start:
#define MEM_RELATIVEPROTMODE_VIDEOPAGE_START 0
#define MEM_RELATIVEPROTMODE_USER_START (1024 * 2)


typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;

typedef void (opcode_pointer)( void );
#define OPCODE( shortOpcodeName ) void ( op_ ## shortOpcodeName )( void )


typedef enum {
	ZF		= 0x0001,	// Zero flag.
	SF		= 0x0002,	// Sign flag.
	CF		= 0x0004,	// Carry flag [Integers only].
	OF		= 0x0008,	// Overflow flag [Integers only].

	IF		= 0x0010,	// Interrupts enabled flag.

	FDDF	= 0x0020,	// Double-data flag [FloatCPU only].
	FNF		= 0x0040,	// Infinity value flag [FloatCPU only].
	FPF		= 0x0080,	// Precision lose flag [FloatCPU only].

	RlModeF	= 0x4000,	// Real (root/system) processor mode flag.
	EmulEndF= 0x8000,	// Exit flag. Maybe will be removed in future.

	FLAGS_Storable = ZF | SF | CF | OF | FDDF | FNF | FPF // They will be handled via "pushf"/"popf".
} EFlags;

#define IF_REAL_MODE if ( proc.flags & RlModeF )

typedef struct {
	union {
		uint32 t32;
		char c[ 4 ];
	} srcmem;

	unsigned char id;
	uint32 args[ 4 ];
} opcode_mem_struct;



typedef enum {
	OPST_None = 0,	// No args.
	OPST_1Regs,		// 5:[r1], 21:const.
	OPST_2Regs,		// 5:[r1], 5:[r2], 16:const.
	OPST_3Regs,		// 5:[r1], 5:[r2], 5:[r3], 11:const.
	OPST_4Regs,		// 5:[r1], 5:[r2], 5:[r3], 5:[r4].
	OPST_Const,
} EOpcodeStructureType;

typedef struct _opcode_structtype_bitlength {
	int argsAmount;
	char lengths[ 4 ]; // From 6th bit.
} opcode_structtype_length;

opcode_structtype_length opcode_structtype_lengths[] = {
	{ 0 },
	{ 2, { 5, 21 } },
	{ 3, { 5, 5, 16 } },
	{ 4, { 5, 5, 5, 11 } },
	{ 4, { 5, 5, 5, 5 } },
	{ 1, { 26 } }
};

#define QueueArgs0() ( uint32[ 4 ] ){ 0 }
#define QueueArgs1( arg1 ) ( uint32[ 4 ] ){ arg1, 0, 0, 0 }
#define QueueArgs2( arg1, arg2 ) ( uint32[ 4 ] ){ arg1, arg2, 0, 0 }
#define QueueArgs3( arg1, arg2, arg3 ) ( uint32[ 4 ] ){ arg1, arg2, arg3, 0 }
#define QueueArgs4( arg1, arg2, arg3, arg4 ) ( uint32[ 4 ] ){ arg1, arg2, arg3, arg4 }



/*typedef union {
	uint32 t32;
	char c[ 4 ];
} opcode_arg;*/


/*typedef union _rics_types {
	uint64 t64;
	double tdouble;

	union {
		uint32 t32;
		float tfloat;

		struct {
			uint16 t16l;
			uint16 t16h;
		} l;
	};
};*/


#endif /* of #ifndef __RISCEMUL_DEFINES_H */
