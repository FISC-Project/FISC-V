/*
 * fiscv.h
 *
 *  Created on: 27/11/2016
 *      Author: Miguel
 */

#ifndef SRC_FISCV_H_
#define SRC_FISCV_H_

#include "memory.h"
#include "fisc_isa.h"
#include "limits.h"

/************/
/** FISC-V **/
/************/
class FISCV {
	Memory memory; /* Main Memory */
	/* TODO: Declare I/O */

	/********************/
	/** CPU Variables: **/
	/********************/

	/* Registers: */
	uint32_t pc; /* Program Counter */
	uint64_t x[FISC_REGISTER_COUNT];
	#define IP0 x[16]
	#define IP1 x[17]
	#define SP  x[28]
	#define FP  x[29]
	#define LR  x[30]
	#define XZR x[31]

	/* Flags: */
	flags_t flags;

	/* Exceptions and Interrupts: */
	/* TODO */

	bool is_branching;
	#define REL_BRANCH(new_addr) pc = (pc + new_addr); is_branching = true;
	#define ABS_BRANCH(new_addr) pc = (new_addr);      is_branching = true;

	/* CPU Methods: */
	void stop() {
		printf("\n>> INFO: Program '%s' finished executing @pc=0x%x.\n", memory.program_filename.c_str(), pc);
	}

	void decode_and_execute(uint32_t instruction, uint16_t opcode) {
		/* Cover the 11-bit opcodes: */
		switch(opcode) {
			case ADD:   printf("(ADD)\n");     add(INSTR_TO_IFMT_R(instruction));    break;
			case ADDS:  printf("(ADDS)\n");    adds(INSTR_TO_IFMT_R(instruction));   break;
			case SUB:   printf("(SUB)\n");     sub(INSTR_TO_IFMT_R(instruction));    break;
			case SUBS:  printf("(SUBS)\n");    subs(INSTR_TO_IFMT_R(instruction));   break;
			case MUL:   printf("(MUL)\n");     mul(INSTR_TO_IFMT_R(instruction));    break;
			case SMULH: printf("(SMULH)\n");   smulh(INSTR_TO_IFMT_R(instruction));  break;
			case UMULH: printf("(UMULH)\n");   umulh(INSTR_TO_IFMT_R(instruction));  break;
			case SDIV:  printf("(SDIV)\n");    sdiv(INSTR_TO_IFMT_R(instruction));   break;
			case UDIV:  printf("(UDIV)\n");    udiv(INSTR_TO_IFMT_R(instruction));   break;
			case AND:   printf("(AND)\n");     and_(INSTR_TO_IFMT_R(instruction));   break;
			case ANDS:  printf("(ANDS)\n");    ands(INSTR_TO_IFMT_R(instruction));   break;
			case ORR:   printf("(ORR)\n");     orr(INSTR_TO_IFMT_R(instruction));    break;
			case EOR:   printf("(EOR)\n");     eor(INSTR_TO_IFMT_R(instruction));    break;
			case NEG:   printf("(NEG)\n");     neg(INSTR_TO_IFMT_R(instruction));    break;
			case NOT:   printf("(NOT)\n");     not_(INSTR_TO_IFMT_R(instruction));   break;
			case LSL:   printf("(LSL)\n");     lsl(INSTR_TO_IFMT_R(instruction));    break;
			case LSR:   printf("(LSR)\n");     lsr(INSTR_TO_IFMT_R(instruction));    break;
			case MOVK+3:case MOVK+2:case MOVK+1:
			case MOVK:  printf("(MOVK)\n");    movk(INSTR_TO_IFMT_IW(instruction));  break;
			case MOVZ+3:case MOVZ+2:case MOVZ+1:
			case MOVZ:  printf("(MOVZ)\n");    movz(INSTR_TO_IFMT_IW(instruction));  break;
			case BR:    printf("(BR)\n");      br(INSTR_TO_IFMT_R(instruction));     break;
			case LDUR:  printf("(LDUR)\n");    ldur(INSTR_TO_IFMT_D(instruction));   break;
			case LDURB: printf("(LDURB)\n");   ldurb(INSTR_TO_IFMT_D(instruction));  break;
			case LDURH: printf("(LDURH)\n");   ldurh(INSTR_TO_IFMT_D(instruction));  break;
			case LDURSW: printf("(LDURSW)\n"); ldursw(INSTR_TO_IFMT_D(instruction)); break;
			case LDXR:  printf("(LDXR)\n");    ldxr(INSTR_TO_IFMT_D(instruction));   break;
			case STUR:  printf("(STUR)\n");    stur(INSTR_TO_IFMT_D(instruction));   break;
			case STURB: printf("(STURB)\n");   sturb(INSTR_TO_IFMT_D(instruction));  break;
			case STURH: printf("(STURH)\n");   sturh(INSTR_TO_IFMT_D(instruction));  break;
			case STURW: printf("(STURW)\n");   sturw(INSTR_TO_IFMT_D(instruction));  break;
			case STXR:  printf("(STXR)\n");    stxr(INSTR_TO_IFMT_D(instruction));   break;

			/* Cover the 10-bit opcodes: */
			default: {
				switch(opcode >> 1) {
					case ADDI:  printf("(ADDI)\n");  addi(INSTR_TO_IFMT_I(instruction));  break;
					case ADDIS: printf("(ADDIS)\n"); addis(INSTR_TO_IFMT_I(instruction)); break;
					case SUBI:  printf("(SUBI)\n");  subi(INSTR_TO_IFMT_I(instruction));  break;
					case SUBIS: printf("(SUBIS)\n"); subis(INSTR_TO_IFMT_I(instruction)); break;
					case ANDI:  printf("(ANDI)\n");  andi(INSTR_TO_IFMT_I(instruction));  break;
					case ANDIS: printf("(ANDIS)\n"); andis(INSTR_TO_IFMT_I(instruction)); break;
					case ORRI:  printf("(ORRI)\n");  orri(INSTR_TO_IFMT_I(instruction));  break;
					case EORI:  printf("(EORI)\n");  eori(INSTR_TO_IFMT_I(instruction));  break;
					case NEGI:  printf("(NEGI)\n");  negi(INSTR_TO_IFMT_I(instruction));  break;
					case NOTI:  printf("(NOTI)\n");  noti(INSTR_TO_IFMT_I(instruction));  break;

					/* Cover the 8-bit opcodes: */
					default :  {
						switch(opcode >> 3) {
							case CBNZ:  printf("(CBNZ)\n");  cbnz(INSTR_TO_IFMT_CB(instruction));  break;
							case CBZ:   printf("(CBZ)\n");   cbz(INSTR_TO_IFMT_CB(instruction));   break;
							case BCOND: printf("(BCOND)\n"); bcond(INSTR_TO_IFMT_CB(instruction)); break;

							/* Cover the 6-bit opcodes: */
							default: {
								switch(opcode >> 5) {
									case B:  printf(" (B)\n"); b(INSTR_TO_IFMT_B(instruction));  break;
									case BL: printf("(BL)\n"); bl(INSTR_TO_IFMT_B(instruction)); break;

									/* Unknown/Invalid instruction: */
									default: {
										printf(" (!INVALID!)\n"); break;
									}
								}
							}
						}
					}
				}
			}
		}
	}
public:
	void run() {
		printf("\n>> INFO: Executing...\n\n>> Instruction count: %d\n", (uint32_t)(memory.get_prog_size() / 4));

		while(pc < memory.get_prog_size() / 4) {
			/* Steps: */
			/* 1- Fetch: */
			uint32_t instruction = memory.read((int)pc, SZ_32);
			printf("@pc = %d: 0x%x | opcode: 0x%x ", pc, instruction, OPCODE_MASK(instruction));

			/* 2,3- Decode and Execute: */
			decode_and_execute(instruction, OPCODE_MASK(instruction));

			/* NOTE: Memory Access and Writeback are already done on the 'Execute' stage */

			if(!is_branching) pc++; /* Increment only if not branching */
			is_branching = false;
		}
		stop();
	}

	FISCV(std::string bootloader_file) : pc(0), is_branching(false) {
		printf("-------------------------------------------\n"
				"--------- FISC-V: Virtual Machine ---------\n"
				"-------------------------------------------\n\n");
		/* Initialize Memory and Load Program: */
		if(!memory.load(bootloader_file)) return;

		/* Clear out the registers and flags: */
		for(int i = 0; i < FISC_REGISTER_COUNT; i++) x[i] = 0;
		flags.carry = flags.negative = flags.overflow = flags.zero = false;
	}
private:
	bool detect_overflow(uint64_t op1, uint64_t op2, char operation) {
		switch(operation) {
			case '+': return (op2 > 0) && (op1 > INT_MAX - op2);
			case '-': return (op2 < 0) && (op1 > INT_MAX + op2);
			default: /* Unknown/Invalid operation */ return false;
		}
	}

	bool detect_carry(uint64_t op1, uint64_t op2, char operation) {
		switch(operation) {
			case '+': {
				int64_t res = op1 + op2;
				return 1 & (((op1 & op2 & ~res) | (~op1 & ~op2 & res)) >> 63);
			}
			case '-': {
				int64_t res = op1 - op2;
				return 1 & (((op1 & op2 & ~res) | (~op1 & ~op2 & res)) >> 63);
			}
			default: /* Unknown/Invalid operation */ return false;
		}
	}

	/**********************************/
	/* Register Read/Write functions: */
	/**********************************/
	void write_reg(uint8_t dst, uint64_t data, bool set_flags, uint64_t op1, uint64_t op2, char operation) {
		if(set_flags) {
			flags.negative = (((int64_t)data) < 0) ? true : false;
			flags.zero     = data == 0 ? true : false;
			flags.overflow = detect_overflow(op1, op2, operation);
			flags.carry    = detect_carry(op1, op2, operation);
		}

		if(dst == 31) return;
		x[dst] = data;
	}

	uint64_t read_reg(uint8_t dst) {
		if(dst == 31) return 0;
		return x[dst];
	}

	/********************************/
	/* Instruction Implementations: */
	/********************************/
	void add(ifmt_r_t * instr) {
		write_reg(instr->rd, x[instr->rn] + x[instr->rm], false, 0, 0, 0);
	}

	void addi(ifmt_i_t * instr) {
		int64_t val = instr->alu_immediate;
		if(val & (1<<(12-1))) val = -((~val+1) & 0xFFF); /* Fix non 64-bit number signedness */
		write_reg(instr->rd, x[instr->rn] + val, false, 0, 0, 0);
	}

	void addis(ifmt_i_t * instr) {
		int64_t val = instr->alu_immediate;
		if(val & (1<<(12-1))) val = -((~val+1) & 0xFFF); /* Fix non 64-bit number signedness */
		write_reg(instr->rd, x[instr->rn] + val, true, x[instr->rn], val, '+');
	}

	void adds(ifmt_r_t * instr) {
		write_reg(instr->rd, x[instr->rn] + x[instr->rm], true, x[instr->rn], x[instr->rm], '+');
	}

	void sub(ifmt_r_t * instr) {
		write_reg(instr->rd, x[instr->rn] - x[instr->rm], false, 0, 0, 0);
	}

	void subi(ifmt_i_t * instr) {
		int64_t val = instr->alu_immediate;
		if(val & (1<<(12-1))) val = -((~val+1) & 0xFFF); /* Fix non 64-bit number signedness */
		write_reg(instr->rd, x[instr->rn] - val, false, 0, 0, 0);
	}

	void subis(ifmt_i_t * instr) {
		int64_t val = instr->alu_immediate;
		if(val & (1<<(12-1))) val = -((~val+1) & 0xFFF); /* Fix non 64-bit number signedness */
		write_reg(instr->rd, x[instr->rn] - val, true, x[instr->rn], val, '-');
	}

	void subs(ifmt_r_t * instr) {
		write_reg(instr->rd, x[instr->rn] - x[instr->rm], true, x[instr->rn], x[instr->rm], '-');
	}

	void mul(ifmt_r_t * instr) {
		write_reg(instr->rd, x[instr->rn] * x[instr->rm], false, 0, 0, 0);
	}

	void smulh(ifmt_r_t * instr) {
		write_reg(instr->rd, x[instr->rn] * x[instr->rm], false, 0, 0, 0);
	}

	void umulh(ifmt_r_t * instr) {
		write_reg(instr->rd, x[instr->rn] * x[instr->rm], false, 0, 0, 0);
	}

	void sdiv(ifmt_r_t * instr) {
		write_reg(instr->rd, x[instr->rn] / x[instr->rm], false, 0, 0, 0);
	}

	void udiv(ifmt_r_t * instr) {
		write_reg(instr->rd, x[instr->rn] / x[instr->rm], false, 0, 0, 0);
	}

	void and_(ifmt_r_t * instr) {
		write_reg(instr->rd, x[instr->rn] & x[instr->rm], false, 0, 0, 0);
	}

	void andi(ifmt_i_t * instr) {
		int64_t val = instr->alu_immediate;
		if(val & (1<<(12-1))) val = -((~val+1) & 0xFFF); /* Fix non 64-bit number signedness */
		write_reg(instr->rd, x[instr->rn] & val, false, 0, 0, 0);
	}

	void andis(ifmt_i_t * instr) {
		int64_t val = instr->alu_immediate;
		if(val & (1<<(12-1))) val = -((~val+1) & 0xFFF); /* Fix non 64-bit number signedness */
		write_reg(instr->rd, x[instr->rn] & val, true, x[instr->rn], val, '&');
	}

	void ands(ifmt_r_t * instr) {
		write_reg(instr->rd, x[instr->rn] & x[instr->rm], true, x[instr->rn], x[instr->rm], '&');
	}

	void orr(ifmt_r_t * instr) {
		write_reg(instr->rd, x[instr->rn] | x[instr->rm], false, 0, 0, 0);
	}

	void orri(ifmt_i_t * instr) {
		int64_t val = instr->alu_immediate;
		if(val & (1<<(12-1))) val = -((~val+1) & 0xFFF); /* Fix non 64-bit number signedness */
		write_reg(instr->rd, x[instr->rn] | val, false, 0, 0, 0);
	}

	void eor(ifmt_r_t * instr) {
		write_reg(instr->rd, x[instr->rn] ^ x[instr->rm], false, 0, 0, 0);
	}

	void eori(ifmt_i_t * instr) {
		int64_t val = instr->alu_immediate;
		if(val & (1<<(12-1))) val = -((~val+1) & 0xFFF); /* Fix non 64-bit number signedness */
		write_reg(instr->rd, x[instr->rn] ^ val, false, 0, 0, 0);
	}

	void neg(ifmt_r_t * instr) {
		write_reg(instr->rd, x[instr->rm]*-1, false, 0, 0, 0);
	}

	void negi(ifmt_i_t * instr) {
		int64_t val = instr->alu_immediate;
		if(val & (1<<(12-1))) val = -((~val+1) & 0xFFF); /* Fix non 64-bit number signedness */
		write_reg(instr->rd, val*-1, false, 0, 0, 0);
	}

	void not_(ifmt_r_t * instr) {
		write_reg(instr->rd, ~x[instr->rm], false, 0, 0, 0);
	}

	void noti(ifmt_i_t * instr) {
		int64_t val = instr->alu_immediate;
		if(val & (1<<(12-1))) val = -((~val+1) & 0xFFF); /* Fix non 64-bit number signedness */
		write_reg(instr->rd, ~val, false, 0, 0, 0);
	}

	void lsl(ifmt_r_t * instr) {
		write_reg(instr->rd, x[instr->rm] << instr->shamt, false, 0, 0, 0);
	}

	void lsr(ifmt_r_t * instr) {
		write_reg(instr->rd, x[instr->rm] >> instr->shamt, false, 0, 0, 0);
	}

	void movk(ifmt_iw_t * instr) {
		if(instr->rt == 31) return;
		switch(instr->opcode & 0b11) {
		case 0b00:
			x[instr->rt] &= 0xFFFFFFFFFFFF0000;
			x[instr->rt] |= instr->mov_immediate & 0xFFFF;
			break;
		case 0b01:
			x[instr->rt] &= 0xFFFFFFFF0000FFFF;
			x[instr->rt] |= (instr->mov_immediate & 0xFFFF) << 16;
			break;
		case 0b10:
			x[instr->rt] &= 0xFFFF0000FFFFFFFF;
			x[instr->rt] |= (uint64_t)((instr->mov_immediate & 0xFFFF)) << 32;
			break;
		case 0b11:
			x[instr->rt] &= 0x0000FFFFFFFFFFFF;
			x[instr->rt] |= (uint64_t)((instr->mov_immediate & 0xFFFF)) << 48;
			break;
		default: /* Invalid quadrant */ break;
		}
	}

	void movz(ifmt_iw_t * instr) {
		if(instr->rt == 31) return;
		switch(instr->opcode & 0b11) {
		case 0b00:
			x[instr->rt] = instr->mov_immediate & 0xFFFF;
			break;
		case 0b01:
			x[instr->rt] = (instr->mov_immediate & 0xFFFF) << 16;
			break;
		case 0b10:
			x[instr->rt] = (uint64_t)((instr->mov_immediate & 0xFFFF)) << 32;
			break;
		case 0b11:
			x[instr->rt] = (uint64_t)((instr->mov_immediate & 0xFFFF)) << 48;
			break;
		default: /* Invalid quadrant */ break;
		}
	}

	void b(ifmt_b_t * instr) {
		int32_t addr = instr->br_address;
		if(addr & (1<<(26-1))) addr = -((~addr+1) & 0x3FFFFFF);
		REL_BRANCH(addr);
	}

	void bcond(ifmt_cb_t * instr) {
		int64_t addr = instr->cond_br_address;
		if(addr & (1<<(19-1))) addr = -((~addr+1) & 0x7FFFF); /* Fix non 64-bit number signedness */

		switch(instr->rt) {
		case 0: /* BEQ */
			if(flags.zero) { REL_BRANCH(addr); }
			break;
		case 1: /* BNE */
			if(!flags.zero) { REL_BRANCH(addr); }
			break;
		case 2: /* BLT */
			if(flags.negative ^ flags.overflow) { REL_BRANCH(addr); }
			break;
		case 3: /* BLE */
			if(!(!flags.zero & !(flags.negative ^ flags.overflow))) { REL_BRANCH(addr); }
			break;
		case 4: /* BGT */
			if((!flags.zero & !(flags.negative ^ flags.overflow))) { REL_BRANCH(addr); }
			break;
		case 5: /* BGE */
			if(!(flags.negative ^ flags.overflow)) { REL_BRANCH(addr); }
			break;
		case 6: /* BLO */
			if(!(flags.carry)) { REL_BRANCH(addr); }
			break;
		case 7: /* BLS */
			if(!(!flags.zero & flags.carry)) { REL_BRANCH(addr); }
			break;
		case 8: /* BHI */
			if((!flags.zero & flags.carry)) { REL_BRANCH(addr); }
			break;
		case 9: /* BHS */
			if(flags.carry) { REL_BRANCH(addr); }
			break;
		case 10: /* BMI */
			if(flags.negative) { REL_BRANCH(addr); }
			break;
		case 11: /* BPL */
			if(!flags.negative) { REL_BRANCH(addr); }
			break;
		case 12: /* BVS */
			if(flags.overflow) { REL_BRANCH(addr); }
			break;
		case 13: /* BVC */
			if(!flags.overflow) { REL_BRANCH(addr); }
			break;
		default: /* Invalid Conditional Branch type */ break;
		}
	}

	void bl(ifmt_b_t * instr) {
		int64_t addr = instr->br_address;
		if(addr & (1<<(26-1))) addr = -((~addr+1) & 0x3FFFFFF); /* Fix non 64-bit number signedness */
		LR = pc + 1;
		REL_BRANCH(addr);
	}

	void br(ifmt_r_t * instr) {
		ABS_BRANCH(x[instr->rd]);
	}

	void cbnz(ifmt_cb_t * instr) {
		int64_t addr = instr->cond_br_address;
		if(addr & (1<<(19-1))) addr = -((~addr+1) & 0x7FFFF); /* Fix non 64-bit number signedness */
		if(x[instr->rt] != 0)
			REL_BRANCH(addr);
	}

	void cbz(ifmt_cb_t * instr) {
		int64_t addr = instr->cond_br_address;
		if(addr & (1<<(19-1))) addr = -((~addr+1) & 0x7FFFF); /* Fix non 64-bit number signedness */
		if(x[instr->rt] == 0)
			REL_BRANCH(addr);
	}

	void ldur(ifmt_d_t * instr) {
		x[instr->rt] = memory.read(x[instr->rn] + instr->dt_address, SZ_64);
	}

	void ldurb(ifmt_d_t * instr) {
		x[instr->rt] = memory.read(x[instr->rn] + instr->dt_address, SZ_8);
	}

	void ldurh(ifmt_d_t * instr) {
		x[instr->rt] = memory.read(x[instr->rn] + instr->dt_address, SZ_16);
	}

	void ldursw(ifmt_d_t * instr) {
		x[instr->rt] = memory.read(x[instr->rn] + instr->dt_address, SZ_32);
	}

	void ldxr(ifmt_d_t * instr) {
		x[instr->rt] = memory.read(x[instr->rn] + instr->dt_address, SZ_64);
		/* TODO Atomic */
	}

	void stur(ifmt_d_t * instr) {
		memory.write(x[instr->rn], x[instr->rt] + instr->dt_address, SZ_64);
	}

	void sturb(ifmt_d_t * instr) {
		memory.write(x[instr->rn], x[instr->rt] + instr->dt_address, SZ_8);
	}

	void sturh(ifmt_d_t * instr) {
		memory.write(x[instr->rn], x[instr->rt] + instr->dt_address, SZ_16);
	}

	void sturw(ifmt_d_t * instr) {
		memory.write(x[instr->rn], x[instr->rt] + instr->dt_address, SZ_32);
	}

	void stxr(ifmt_d_t * instr) {
		memory.write(x[instr->rn], x[instr->rt] + instr->dt_address, SZ_64);
		/* TODO Atomic */
	}
};
/*****************/
/** FISC-V: END **/
/*****************/

#endif /* SRC_FISCV_H_ */
