//
// Constants, constant and macro defines, enumerates, typedefs, etc.
//

#ifndef __RISCEMUL_DEFINES_H
#define __RISCEMUL_DEFINES_H

#define MAX_REGS 32
#define MAX_FLOAT_REGS 8

#define MAX_MEM (1024 * 1024) /* Statically allocated 1 MB. */

#define MEM_BIOS_START 0
#define MEM_BIOS_CURSTACKPOS 0
#define MEM_BIOS_TEXTCURSORX 4
#define MEM_BIOS_TEXTCURSORY 6
#define MEM_BIOS_OTHER 8
#define MEM_BIOS_END 1023

#define MEM_STACK_START 1024
#define MEM_STACK_END (1024 * 2 - 1)

#define MEM_PROTECTEDMODE_START (1024 * 4)
#define MEM_VIDEOPAGE_START (1024 * 4)
#define MEM_VIDEOPAGE_END (1024 * 4 + 80 * 25)

#define MEM_USER_START (1024 * 8)


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

	RlModeF	= 0x8000,	// Real (root/system) processor mode flag.

	FLAGS_Storable = ZF | SF | CF | OF | FDDF | FNF | FPF // They will be handled via "pushf"/"popf".
} EFlags;

#define IF_REAL_MODE if ( proc.flags & RlModeF )

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
