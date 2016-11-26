/*
 * fisc_isa.h
 *
 *  Created on: 26/11/2016
 *      Author: Miguel
 */

#ifndef SRC_FISC_ISA_H_
#define SRC_FISC_ISA_H_

/************************************/
/* FISC Instruction Set Definition: */
/************************************/
#define FISC_INSTRUCTION_SZ 32 /* How wide is the instruction (in bits, not bytes) */
#define FISC_REGISTER_COUNT 32 /* How many registers will we use */

enum OPCODES {
	/* ARITHMETIC AND LOGIC */
	ADD  = 0x458,  ADDI  = 0x488, ADDIS = 0x588, ADDS   = 0x558,
	SUB  = 0x658,  SUBI  = 0x688, SUBIS = 0x788, SUBS   = 0x758,
	MUL  = 0x4D8,  SMULH = 0x4DA, UMULH = 0x4DE,
	SDIV = 0x4D6,  UDIV  = 0x4D7,
	AND  = 0x450,  ANDI  = 0x490, ANDIS = 0x790, ANDS   = 0x750,
	ORR  = 0x550,  ORRI  = 0x590,
	EOR  = 0x650,  EORI  = 0x690,
	NEG  = 0x768,  NEGI  = 0x388,
	NOT  = 0x769,  NOTI  = 0x288,
	LSL  = 0x69B,  LSR   = 0x69A,
	MOVK = 0x794,  MOVZ  = 0x694,
	/* BRANCHING */
	B    = 0x0A0,  BL    = 0x4A0, BR    = 0x6B0, CBNZ   = 0x5A8,
	CBZ  = 0x5A0,  BEQ   = 0x2A0, BNE   = 0x2A1, BLT    = 0x2A2,
	BLE  = 0x2A3,  BGT   = 0x2A4, BGE   = 0x2A5, BLO    = 0x2A6,
	BLS  = 0x2A7,  BHI   = 0x2A8, BHS   = 0x2A9, BMI    = 0x2AA,  BPL  = 0x2AB,
	BVS  = 0x2AC,  BVC   = 0x2AD,
	/* LOAD AND STORE */
	LDUR = 0x7C2,  LDURB = 0x1C2, LDURH = 0x3C2, LDURSW = 0x5C4,  LDXR = 0x642,
	STUR = 0x7C0,  STURB = 0x1C0, STURH = 0x3C0, STURW  = 0x5C0,  STXR = 0x640,
};

#define OPCODE_MASK(instr) (((instr) & 0xFFE00000) >> 21)

enum INSTRUCTION_FMT {
	IFMT_R,  /* Register-Register Format     */
	IFMT_I,  /* Register-Immediate Format    */
	IFMT_D,  /* Register-Memory Format       */
	IFMT_B,  /* PC-Immediate-Register Format */
	IFMT_CB, /* PC-Immediate-Register Format */
	IFMT_IW  /* Register-Immediate Format    */
};

typedef struct ifmt_r {
	unsigned rd : 5;
	unsigned rn : 5;
	unsigned shamt : 6;
	unsigned rm : 5;
	unsigned opcode : 11;
} ifmt_r_t;

#define INSTR_TO_IFMT_R(instruction) ((ifmt_r_t*)&instruction)

typedef struct ifmt_i {
	unsigned rd : 5;
	unsigned rn : 5;
	unsigned alu_immediate : 12;
	unsigned opcode : 10; // The lower 1 bit is discarded, it's like an X (don't care)
} ifmt_i_t;

#define INSTR_TO_IFMT_I(instruction) ((ifmt_i_t*)&instruction)

typedef struct ifmt_d {
	unsigned rt : 5;
	unsigned rn : 5;
	unsigned op : 2;
	unsigned dt_address : 9;
	unsigned opcode : 11;
} ifmt_d_t;

#define INSTR_TO_IFMT_D(instruction) ((ifmt_d_t*)&instruction)

typedef struct ifmt_b {
	unsigned br_address : 26;
	unsigned opcode : 6; // The lower 5 bits are discarded, they are like X's (don't cares)
} ifmt_b_t;

#define INSTR_TO_IFMT_B(instruction) ((ifmt_b_t*)&instruction)

typedef struct ifmt_cb {
	unsigned rt : 5;
	unsigned cond_br_address : 19;
	unsigned opcode : 8; // The lower 3 bits are discarded, they are like X's (don't cares)
} ifmt_cb_t;

#define INSTR_TO_IFMT_CB(instruction) ((ifmt_cb_t*)&instruction)

typedef struct ifmt_iw {
	unsigned rt : 5;
	unsigned mov_immediate : 16;
	unsigned opcode : 11;
} ifmt_iw_t;

#define INSTR_TO_IFMT_IW(instruction) ((ifmt_iw_t*)&instruction)

enum DATATYPE {
	SZ_8, SZ_16, SZ_32, SZ_64
};

typedef struct _flags {
	bool negative;
	bool zero;
	bool overflow;
	bool carry;
} flags_t;

#endif /* SRC_FISC_ISA_H_ */
