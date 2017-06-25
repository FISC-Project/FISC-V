#ifndef FISCISA_H_
#define FISCISA_H_

#include <string>
#include <functional>
#include <stdint.h>
#include <stdarg.h>
#include <fvm/Utils/Bit.h>
#include <fvm/Utils/String.h>
#include <fvm/Debug/Debug.h>

namespace FISC {

/************************************/
/* FISC Instruction Set Declaration */
/************************************/

#define FISC_INSTRUCTION_SZ         32     /* How wide is the instruction (in bits, not bytes)                                   */
#define FISC_REGISTER_COUNT         32     /* How many registers will we use                                                     */
#define FISC_SPECIAL_REGISTER_COUNT 14     /* How many special registers will we use                                             */
#define FISC_TOTAL_REGISTER_COUNT (FISC_REGISTER_COUNT + FISC_SPECIAL_REGISTER_COUNT) /* How many registers in total             */
#define FISC_DEFAULT_EXEC_MODE      FISC_CPU_MODE_KERNEL /* The default mode of execution of the CPU                             */
#define FISC_PAGES_PER_TABLE        1024   /* How many pages are present on an MMU's Table entry                                 */
#define FISC_TABLES_PER_DIR         1024   /* How many tables can a Page directory hold                                          */
#define FISC_PAGE_SIZE              0x1000 /* How large of a block each Page entry can represent                                 */
#define FISC_USER_SYSCALL_CODE      0xFFF  /* The interrupt number the user must use when executing system calls (or interrupts) */

#define ENDIANNESS_TEXTSECT false /* 0: big endian 1: little endian */
#define ENDIANNESS_DATASECT true  /* 0: big endian 1: little endian */

enum INSTRUCTION_FMT {
	IFMT_R,  /* Register-Register     Format */
	IFMT_I,  /* Register-Immediate    Format */
	IFMT_D,  /* Register-Memory       Format */
	IFMT_B,  /* PC-Immediate-Register Format */
	IFMT_CB, /* PC-Immediate-Register Format */
	IFMT_IW  /* Register-Immediate    Format */
};

#define RF  IFMT_R
#define IF  IFMT_I
#define DF  IFMT_D
#define BF  IFMT_B
#define CBF IFMT_CB
#define IWF IFMT_IW

#define OPCODE_MASK(instr) (((instr) & 0xFFE00000) >> 21)

typedef struct ifmt_r {
	unsigned rd     : 5;
	unsigned rn     : 5;
	unsigned shamt  : 6;
	unsigned rm     : 5;
	unsigned opcode : 11;
} ifmt_r_t;

#define INSTR_TO_IFMT_R(instruction) ((ifmt_r_t*)&instruction)

typedef struct ifmt_i {
	unsigned rd            : 5;
	unsigned rn            : 5;
	unsigned alu_immediate : 12;
	unsigned opcode        : 10; /* The lower 1 bit is discarded, it's like an X (don't care) */
} ifmt_i_t;

#define INSTR_TO_IFMT_I(instruction) ((ifmt_i_t*)&instruction)

typedef struct ifmt_d {
	unsigned rt         : 5;
	unsigned rn         : 5;
	unsigned op         : 2;
	unsigned dt_address : 9;
	unsigned opcode     : 11;
} ifmt_d_t;

#define INSTR_TO_IFMT_D(instruction) ((ifmt_d_t*)&instruction)

typedef struct ifmt_b {
	unsigned br_address : 26;
	unsigned opcode     : 6; /* The lower 5 bits are discarded, they are like X's (don't cares) */
} ifmt_b_t;

#define INSTR_TO_IFMT_B(instruction) ((ifmt_b_t*)&instruction)

typedef struct ifmt_cb {
	unsigned rt              : 5;
	unsigned cond_br_address : 19;
	unsigned opcode          : 8; /* The lower 3 bits are discarded, they are like X's (don't cares) */
} ifmt_cb_t;

#define INSTR_TO_IFMT_CB(instruction) ((ifmt_cb_t*)&instruction)

typedef struct ifmt_iw {
	unsigned rt            : 5;
	unsigned mov_immediate : 16;
	unsigned quadrant      : 2;
	unsigned opcode        : 9;
} ifmt_iw_t;

#define INSTR_TO_IFMT_IW(instruction) ((ifmt_iw_t*)&instruction)

enum CONDITION_CODES { /* Note: condition codes are not 0 indexed */
	BEQ = 1, /* Branch if equal (==)                 */
	BNE,     /* Branch if not equal (!=)             */
	BLT,     /* Branch if less than (<)              */
	BLE,     /* Branch if less or equal than <==)    */
	BGT,     /* Branch if greater than (>)           */
	BGE,     /* Branch if greater or equal than (>=) */
	BLO,     /* Branch if lower than (< signed)      */
	BLS,     /* Branch if lower or same (<= signed)  */
	BHI,     /* Branch if higher (> signed)          */
	BHS,     /* Branch if higher or same (>= signed) */
	BMI,     /* Branch if minus (-)                  */
	BPL,     /* Branch if plus (+)                   */
	BVS,     /* Branch on overflow set               */
	BVC      /* Branch on overflow clear             */
};

enum OPCODE {
	/* ARITHMETIC AND LOGIC */
	ADD   = 0x458, ADDI  = 0x488, ADDIS = 0x588, ADDS = 0x558,
	SUB   = 0x658, SUBI  = 0x688, SUBIS = 0x788, SUBS = 0x758,
	MUL   = 0x4D8, SMULH = 0x4DA, UMULH = 0x4DE,
	SDIV  = 0x4D6, UDIV  = 0x4D7,
	AND   = 0x450, ANDI  = 0x490, ANDIS = 0x790, ANDS = 0x750,
	ORR   = 0x550, ORRI  = 0x590,
	EOR   = 0x650, EORI  = 0x690,
	NEG   = 0x768, NEGI  = 0x388,
	NOT   = 0x769, NOTI  = 0x288,
	LSL   = 0x69B, LSR   = 0x69A,
	MOVK  = 0x794, MOVZ  = 0x694, MOVRK = 0x7D4, MOVRZ = 0x6D4,
	LDPC  = 0x544,
	/* BRANCHING */
	B     = 0x0A0, BL    = 0x4A0,
	BR    = 0x6B0, BRL   = 0x6B1,
	CBNZ  = 0x5A8, CBZ   = 0x5A0,
	BCOND = 0x2A0,
	/* LOAD AND STORE */
	LDR   = 0x7C2, LDRB  = 0x1C2, LDRH  = 0x3C2, LDRSW  = 0x5C4, LDXR  = 0x642,
	LDRR  = 0x7D2, LDRBR = 0x1D2, LDRHR = 0x3D2, LDRSWR = 0x4C4, LDXRR = 0x652,
	STR_  = 0x7C0, STRB  = 0x1C0, STRH  = 0x3C0, STRW   = 0x5C0, STXR  = 0x640,
	STRR  = 0x7D0, STRBR = 0x1D0, STRHR = 0x3D0, STRWR  = 0x5D0, STXRR = 0x5D1,
	/* FLOATING POINT */
	FADDS = 0xF1,  FADDD = 0xF2,
	FSUBS = 0xF3,  FSUBD = 0xF4,
	FCMPS = 0xF5,  FCMPD = 0xF6,
	FMULS = 0xF7,  FMULD = 0xF8,
	FDIVS = 0xF9,  FDIVD = 0xFA,
	LDRS  = 0xFB,  LDRD  = 0xFC,
	STRS  = 0xFD,  STRD  = 0xFE,
	/* CPU STATUS CONTROL */
	MSR   = 0x614, MRS   = 0x5F4,
	/* INTERRUPTS */
	LIVP  = 0x5D4, SIVP  = 0x5B4,
	LEVP  = 0x594, SEVP  = 0x574,
	SESR  = 0x554,
	SINT  = 0x520, RETI  = 0x580,
	/* VIRTUAL MEMORY */
	LPDP  = 0x4F4, SPDP  = 0x4D4,
	LPFLA = 0x4B4
};

/*********************************/
/* Register related declarations */
/*********************************/
#define IP0 16 /* Scratch register 0 */
#define IP1 17 /* Scratch register 1 */
#define SP  28 /* Stack pointer      */
#define FP  29 /* Frame pointer      */
#define LR  30 /* Link register      */
#define XZR 31 /* Zero register      */

enum SPECIAL_REGISTERS {
	SPECIAL_PC = 32, /* Program Counter                   */
	SPECIAL_ESR,     /* Exception Syndrome Register       */
	SPECIAL_ELR,     /* Exception Link / Return Register  */
	SPECIAL_CPSR,    /* Current Processor Status Register */
	/* Saved Processor Status Registers */
	SPECIAL_SPSR0, SPECIAL_SPSR1, SPECIAL_SPSR2,
	SPECIAL_SPSR3, SPECIAL_SPSR4, SPECIAL_SPSR5,
	SPECIAL_IVP, /* Interrupt Vector Pointer  */
	SPECIAL_EVP, /* Exception Vector Pointer  */
	SPECIAL_PDP, /* Page Directory Pointer    */
	SPECIAL_PFLA /* Page Fault Linear Address */
};

typedef struct {
	unsigned mode : 3; /* CPU Execution Mode:    UND.    | User | Kernel | IRQ | SIRQ | EXCEPTION (0b111) */
	unsigned ien  : 2; /* Interrupt Enable Mask: Disable | Enable */
	unsigned pg   : 1; /* Paging:                Disable | Enable */
	unsigned ae   : 2; /* Alignment:             Disable | Enable */
	unsigned c    : 1; /* Carry    flag */
	unsigned v    : 1; /* Overflow flag */
	unsigned z    : 1; /* Zero     flag */
	unsigned n    : 1; /* Negative flag */
} cpsr_t;

enum FISC_CPU_MODE {
	FISC_CPU_MODE_UNDEFINED, /* Undefined mode */
	FISC_CPU_MODE_USER,      /* User mode. Used for executing user applications such as Operating Systems    */
	FISC_CPU_MODE_KERNEL,    /* Kernel Mode. Used to support Operating Systems. Has maximum access privilege */
	FISC_CPU_MODE_IRQ,       /* IRQ - Interrupt Request Mode. Used on interrupt requests (external only)     */
	FISC_CPU_MODE_SIRQ,      /* SIRQ - Software Interrupt Request Mode. Used on interrupt requests triggered by the software instruction SINT */
	FISC_CPU_MODE_EXCEPTION, /* Exception Mode. Used when exceptions are caused either by the internal CPU or by external stimulae / errors (for example an external FPU or (tightly coupled) GPU) */
	FISC_CPU_MODE__COUNT
};

enum FISC_DATATYPE {
	FISC_SZ_64, /* DOUBLE WORD */
	FISC_SZ_8,  /* BYTE        */
	FISC_SZ_16, /* HALF WORD   */
	FISC_SZ_32, /* WORD        */
	FISC_SZ__COUNT
};

enum FISC_RETTYPE {
	FISC_RET_NULL,
	FISC_RET_OK,      /* Used to indicate successful instruction execution                                */
	FISC_RET_INFO,    /* Used to indicate successful instruction execution included with a useful message */
	FISC_RET_WARNING, /* Used to indicate successful instruction execution included with a warning message which could carry information about potential future errors */
	FISC_RET_ERROR,   /* Used to indicate an unsuccessful instruction execution                           */
	FISC_RET_WAIT,    /* Used to indicate to an external device that the CPU is busy                      */
	FISC_RET__COUNT
};

/*******************************/
/* Virtual Memory declarations */
/*******************************/

/* Page definition */
typedef struct {
	unsigned int present       : 1; /* (0) NOT PRESENT (1) PRESENT            */
	unsigned int rw            : 1; /* (0) READ ONLY   (1) WRITABLE           */
	unsigned int user          : 1; /* (0) KERNEL MODE (1) USER MODE          */
	unsigned int writethrough  : 1;
	unsigned int cachedisabled : 1;
	unsigned int accessed      : 1; /* (0) NOT ACCESSED (1) ACCESSED          */
	unsigned int dirty         : 1; /* (0) NOT BEEN WRITTEN TO (1) WRITTEN TO */
	unsigned int unused1       : 1;
	unsigned int global        : 1;
	unsigned int unused2       : 3;
	unsigned int phys_addr     : 20; /* FRAME ADDRESS */
} page_t;

/* Table entry definition: */
typedef struct {
	unsigned int present       : 1;  /* (0) table not present (1) table present            */
	unsigned int rw            : 1;  /* (0) table read only (1) table writable             */
	unsigned int user          : 1;  /* (0) kernel mode (1) user mode                      */
	unsigned int writethrough  : 1;  /* (0) write back caching enabled (1) ... disabled    */
	unsigned int cachedisabled : 1;  /* (0) table won't be cached (1) table will be cached */
	unsigned int accessed      : 1;  /* (0) not accessed (1) accessed                      */
	unsigned int unused1       : 1;
	unsigned int page_size     : 1;  /* (0) 4kb page sizes (1) 4mb page sizes              */
	unsigned int available     : 4;  /* available for use                                  */
	unsigned int table_address : 20; /* address of the page directory table                */
} page_table_entry_t;

/* Many pages definition (as a table) */
typedef struct {
	page_t pages[FISC_PAGES_PER_TABLE]; /* 4MB per table and 4KB per page */
} page_table_t;

/* Directory definition */
typedef struct page_directory {
	page_table_entry_t table_entries[FISC_TABLES_PER_DIR];
	page_table_t     * tables[FISC_TABLES_PER_DIR]; /* Array of page tables, covers entire memory space */
} paging_directory_t;

/*******************/
/* Exception Codes */
/*******************/
enum EXCEPTION_CODE {
	EXC_TRIPLEFAULT,
	EXC_DIVZERO,
	EXC_DEBUG,
	EXC_NONMASK,
	EXC_BREAKPOINT,
	EXC_OVERFLOW,
	EXC_BOUNDRANGE,
	EXC_INVALOPC,
	EXC_NODEV,
	EXC_DOUBLEINTERRUPT, 
	EXC_DOUBLEFAULT,
	EXC_PERMFAULT,
	EXC_PAGEFAULT
};

/*********************************/
/* Instruction class declaration */
/*********************************/

/* Forward decl */
class Instruction;

extern Instruction ** instruction_list_realloc;
extern unsigned int instruction_list_size;
extern bool instruction_list_success_declared;

class CPUModule;

class Instruction {
public:
	Instruction(enum OPCODE opcode, std::string opcodeStr, enum INSTRUCTION_FMT format, std::function<enum FISC_RETTYPE(Instruction*, CPUModule*)> operation)
	{
		this->instruction = (uint32_t)-1;
		this->opcode = opcode;
		if(opcodeStr == "STR_") opcodeStr = "STR"; /* Small silly fix */
		this->opcodeStr = opcodeStr;
		this->format = format;
		this->operation = operation;
		this->retStr = NULLSTR;
		this->timesExecuted = 0;
		this->passOwner = nullptr;

		initialized = true;

		switch (format) {
			case IFMT_R:  
				this->opcodeShifted = this->opcode;
				this->opcodeSize = 11;
				this->formatStr = "R";
				break;
			case IFMT_I:
				this->opcodeShifted = this->opcode >> 1;
				this->opcodeSize = 10;
				this->formatStr = "I";
				break;
			case IFMT_D:
				this->opcodeShifted = this->opcode;
				this->formatStr = "D";
				break;
			case IFMT_B:
				this->opcodeShifted = this->opcode >> 5;
				this->opcodeSize = 6;
				this->formatStr = "B";
				break;
			case IFMT_CB:
				this->opcodeShifted = this->opcode >> 3;
				this->opcodeSize = 8;
				this->formatStr = "CB";
				break;
			case IFMT_IW:
				this->opcodeShifted = this->opcode >> 2;
				this->opcodeSize = 9;
				this->formatStr = "IW";
				break;
			default:
				this->opcodeShifted = (uint16_t)-1;
				this->opcode = (enum OPCODE)-1;
				this->opcodeSize = (unsigned int)-1;
				this->opcodeStr = NULLSTR;
				this->formatStr = NULLSTR;
				initialized = false;
				break;
		}

		for (unsigned int i = 0; i < instruction_list_size; i++) {
			if (instruction_list_realloc[i]->opcodeShifted == this->opcodeShifted)
			{
				/* Oh no, there is a conflict between instructions */
				debug::DEBUG(DERROR, "FATAL ERROR: The instruction %s (opcode 0x%X) is in conflict with instruction %s (opcode 0x%X)", 
					this->opcodeStr.c_str(), this->opcode,
					instruction_list_realloc[i]->opcodeStr.c_str(), instruction_list_realloc[i]->opcode);
				free(instruction_list_realloc);
				instruction_list_realloc = nullptr;
				instruction_list_size = 0;
				instruction_list_success_declared = false; /* Prevent all the other instructions from being initialized */
			}
		}

		if(instruction_list_success_declared) {
			instruction_list_size++;
			instruction_list_realloc = (Instruction**)realloc(instruction_list_realloc, instruction_list_size * sizeof(Instruction*));
			instruction_list_realloc[instruction_list_size - 1] = this;
		}
	}

	uint32_t instruction;
	ifmt_r_t  * ifmt_r;
	ifmt_i_t  * ifmt_i;
	ifmt_d_t  * ifmt_d;
	ifmt_b_t  * ifmt_b;
	ifmt_cb_t * ifmt_cb;
	ifmt_iw_t * ifmt_iw;
	enum OPCODE opcode;
	uint16_t opcodeShifted;
	std::string opcodeStr;
	unsigned int opcodeSize;
	enum INSTRUCTION_FMT format;
	std::string formatStr;
	std::function<enum FISC_RETTYPE(Instruction*, CPUModule*)> operation;
	std::string targetName;
	std::string retStr;
	uint32_t timesExecuted;
	bool initialized;
	CPUModule * passOwner;
};

#define NEW_INSTRUCTION(targetname, mnemonic, format, operation) static Instruction targetname ## _instruction_ ## mnemonic(mnemonic, STRING(mnemonic), format, [] (Instruction * _this_, CPUModule * _cpu_) operation)

#define RETURN(type, msg) do{ _this_->retStr = msg; return type; } while(0);

#define ALIGN_BASE(base, op) (op == 1 ? base : op == 2 ? ALIGN16(base) : op == 3 ? ALIGN32(base) : op == 0 ? ALIGN64(base) : -1)
#define ALIGN_DTADDR(dtaddr, op) (op == 1 ? dtaddr : op == 2 ? ALIGN16(dtaddr) : op == 3 ? ALIGN32(dtaddr) : op == 0 ? ALIGN64(dtaddr) : -1)

}
#endif

using namespace FISC;
