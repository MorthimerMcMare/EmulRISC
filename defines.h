//
// Constants, constant and macro defines, enumerates, typedefs, etc.
//

#ifndef __RISCEMUL_DEFINES_H
#define __RISCEMUL_DEFINES_H

#define RISC_INSTRUCTION_LENGTH 4 /* Byte length of the every one instruction. */

#define MAX_REGS 32 /* As 2^5. */
#define MAX_FLOAT_REGS 16

#define MAX_MEM (1024 * 1024 * 64) /* Statically allocated 64 MB. */

// Main statical:

#define MEM_KERNEL_START 0

/* 256 interrupt addresses in table at all. */
#define MEM_INTERRUPT_VECTOR_TABLE_START 1024
#define MEM_INTERRUPT_VECTOR_TABLE_END (MEM_INTERRUPT_VECTOR_TABLE_START + 4 * 256 - 1)
/* 16 exceptions interrupt addresses: */
#define MEM_INTERRUPT_VECTOR_TABLE_EXCEPTIONS_FIRST 0
#define MEM_INTERRUPT_VECTOR_TABLE_EXCEPTIONS_START MEM_INTERRUPT_VECTOR_TABLE_START
#define MEM_INTERRUPT_VECTOR_TABLE_EXCEPTIONS_AMOUNT 16
/* 16 quasiBIOS interrupt addresses (overridden only if address value is not "0"): */
#define MEM_INTERRUPT_VECTOR_TABLE_BIOS_FIRST MEM_INTERRUPT_VECTOR_TABLE_EXCEPTIONS_AMOUNT
#define MEM_INTERRUPT_VECTOR_TABLE_BIOS_START (MEM_INTERRUPT_VECTOR_TABLE_EXCEPTIONS_START + 4 * MEM_INTERRUPT_VECTOR_TABLE_EXCEPTIONS_AMOUNT)
#define MEM_INTERRUPT_VECTOR_TABLE_BIOS_AMOUNT 16
/* All other interrupt addresses: */
#define MEM_INTERRUPT_VECTOR_TABLE_USER_FIRST (MEM_INTERRUPT_VECTOR_TABLE_EXCEPTIONS_AMOUNT + MEM_INTERRUPT_VECTOR_TABLE_BIOS_AMOUNT)
#define MEM_INTERRUPT_VECTOR_TABLE_USER_START (MEM_INTERRUPT_VECTOR_TABLE_BIOS_START + 4 * MEM_INTERRUPT_VECTOR_TABLE_BIOS_AMOUNT)

/*  */
#define MEM_TBL_START (MEM_INTERRUPTS_END + 1)
#define MEM_TBL_AMOUNT 64
#define MEM_TBL_END (MEM_TBL_START + 8 * MEM_TBL_AMOUNT)

/* Interrupts itself: */
#define MEM_INTERRUPTS_START (MEM_INTERRUPT_VECTOR_TABLE_END + 1)
#define MEM_INTERRUPTS_END (MEM_INTERRUPTS_START + 1024 * 16 - 1)

// Virtual kernel mode memory:

#define MEM_PROG_KERNEL_START (1024 * 256)

// Videopage:
#define MEM_KERNEL_END (MEM_VIDEOPAGE_START - 1)

#define MEM_VIDEOPAGE_TEXTAMOUNT (80 * 25)
#define MEM_VIDEOPAGE_START (MEM_USER_START - MEM_VIDEOPAGE_TEXTAMOUNT)
#define MEM_VIDEOPAGE_END (MEM_USER_START - 1)

// Virtual user memory:

#define MEM_USER_START (1024 * 1024 * 32)
#define MEM_USER_END MAX_MEM


typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef int64_t int64;
typedef int32_t int32;
typedef int16_t int16;

typedef void (opcode_pointer)( void );
#define OPCODE( shortOpcodeName ) void ( op_ ## shortOpcodeName )( void )
#define BIOSOPCODE( shortOpcodeName ) void ( bios_ ## shortOpcodeName )( void )
#define EXCEPTIONOPCODE( shortOpcodeName ) void ( except_ ## shortOpcodeName )( void )

#define FINDINT( interruptName ) findInterruptMatrixIndex( interruptName )

#if defined(__WIN32) || defined(__MSDOS__)
# include <conio.h>
#elif defined( USE_NCURSES )
# include <ncurses.h>
# define clrscr() clear()
# define printf printw
# define puts(string); printw( "%s\n", (string) );
# define putchar(pchar) printw( "%c", (char) (pchar) )
# define LINUX_OS
#else
# define getch getchar
#endif

#ifndef LINUX_OS
//# define STRGKEY_BACKSPACE 8
# ifdef __WIN32
   void clrscr( void ) { system( "cls" ); }
# endif
//#else
//# define STRGKEY_BACKSPACE 127
#endif


typedef enum {
	ZF		= 0x0001,	// Zero flag.
	SF		= 0x0002,	// Sign flag.
	CF		= 0x0004,	// Carry flag [Integers only].
	OF		= 0x0008,	// Overflow flag [Integers only].

	IF		= 0x0010,	// Interrupts enabled flag.
	TF		= 0x0020,	// Trace enabled flag.

	FDDF	= 0x0100,	// Double-data flag [FloatCPU only].
	FNF		= 0x0200,	// Infinity value flag [FloatCPU only].
	FPF		= 0x0400,	// Precision lose flag [FloatCPU only].

	RlModeF	= 0x4000,	// Real (root/system) processor mode flag.
	EndEmulF= 0x8000,	// Exit flag. Maybe will be removed in future.

	FLAGS_Storable = ZF | SF | CF | OF | IF | TF | FDDF | FNF | FPF // They will be handled via "pushf"/"popf".
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

#define REGARG( index ) proc.regs[ curopc.args[ index ] ]

#define checkZeroFlag( value )\
if ( value == 0 )\
	proc.flags |= ZF;\
else\
	proc.flags &= ~ZF;


typedef enum {
	OPST_None = 0,	// No args.
	OPST_1Reg,		// 5:[r1].
	OPST_1RegC,		// 5:[r1], 21:const.
	OPST_2Reg,		// 5:[r1], 5:[r2].
	OPST_2RegC,		// 5:[r1], 5:[r2], 16:const.
	OPST_3Reg,		// 5:[r1], 5:[r2], 5:[r3].
	OPST_3RegC,		// 5:[r1], 5:[r2], 5:[r3], 11:const.
	OPST_4Reg,		// 5:[r1], 5:[r2], 5:[r3], 5:[r4].
	OPST_ByteConst,	// 8:const.
	OPST_WordConst,	// 16:const.
	OPST_MaxConst,	// 26:const.
	OPST_C1Reg,		// 21:const, 5:[r1].
} EOpcodeStructureType;

typedef enum {
	OPSPT_Undefined = 0,
	OPSPT_Reg,
	OPSPT_Const,

	OPSPT_Register = OPSPT_Reg,
	OPSPT_Constant = OPSPT_Const
} EOpcodeStructPartType;

typedef enum {
	INTT_Exception = 0,
	INTT_BIOS,
	INTT_IRQ,
	INTT_User,

	INTT_Unmaskable		= 0x1000
} EInterruptType;

typedef struct _opcode_structtype_bitlength {
	int argsAmount;
	char lengths[ 4 ]; // From 6th bit.
	EOpcodeStructPartType types[ 4 ];
} opcode_structtype_length;

opcode_structtype_length opcode_structtype_lengths[] = {
	{ 0 },
	{ 1, { 5 }, 			{ OPSPT_Reg } },
	{ 2, { 5, 21 }, 		{ OPSPT_Reg, OPSPT_Const } },
	{ 2, { 5, 5 }, 			{ OPSPT_Reg, OPSPT_Reg } },
	{ 3, { 5, 5, 16 }, 		{ OPSPT_Reg, OPSPT_Reg, OPSPT_Const } },
	{ 3, { 5, 5, 5 }, 		{ OPSPT_Reg, OPSPT_Reg, OPSPT_Reg } },
	{ 4, { 5, 5, 5, 11 }, 	{ OPSPT_Reg, OPSPT_Reg, OPSPT_Reg, OPSPT_Const } },
	{ 4, { 5, 5, 5, 5 }, 	{ OPSPT_Reg, OPSPT_Reg, OPSPT_Reg, OPSPT_Reg } },
	{ 1, { 8 }, 			{ OPSPT_Const } },
	{ 1, { 16 },  			{ OPSPT_Const } },
	{ 1, { 26 }, 			{ OPSPT_Const } },
	{ 2, { 21, 5 },			{ OPSPT_Const, OPSPT_Reg } },
};


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
