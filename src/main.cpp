/*
 * main.cpp
 *
 *  Created on: 25/11/2016
 *      Author: Miguel
 */
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <vector>
#include <stdint.h>
#include <cstring>

#include "fisc_isa.h"

/*****************/
/** MAIN MEMORY **/
/*****************/
#define MEMORY_WIDTH   8    /* The width of the memory */
#define MEMORY_DEPTH   1024 /* Size of memory in bytes */
#define MEMORY_LOADLOC 0    /* Where to load the program on startup */

#define ALIGN16(addr) ((addr)*2)
#define ALIGN32(addr) ((addr)*4)
#define ALIGN64(addr) ((addr)*8)

class Memory {
	uint64_t load_prgm_size; /* Size of the loaded program */
	std::vector<std::bitset<MEMORY_WIDTH> > mem; /* Actual memory */
public:
	std::string program_filename;

	Memory () : load_prgm_size(0), mem(MEMORY_DEPTH, -1), program_filename("(null)") {

	}

	uint64_t get_size() {
		return mem.size();
	}

	uint64_t get_prog_size() {
		return load_prgm_size;
	}

	bool load(std::string memory_file) {
		printf(">> INFO: Loading Program '%s' ...", memory_file.c_str());
		program_filename = memory_file;

		std::ifstream file;
		file.open(memory_file.c_str(), std::ios::in | std::ios::binary);
		if(file.is_open()) {
			char line;
			uint64_t i = MEMORY_LOADLOC;
			while(!file.eof()) {
				file.read(&line, MEMORY_WIDTH / 8);
				mem[i++] = std::bitset<MEMORY_WIDTH>(line);
				load_prgm_size++;
			}
			printf(" Success!");
			return true;
		} else {
			printf("\n>> ERROR: File '%s' not found", memory_file.c_str());
		}
		file.close();
		return false;
	}

	uint64_t read(uint32_t address, uint8_t datasize) {
		switch(datasize) {
			case SZ_8:  if(address >= mem.size()) return (uint64_t)-1; break;
			case SZ_16: if(ALIGN16(address)+1 >= mem.size()) return (uint64_t)-1;break;
			case SZ_32: if(ALIGN32(address)+3 >= mem.size()) return (uint64_t)-1;break;
			case SZ_64: if(ALIGN64(address)+7 >= mem.size()) return (uint64_t)-1;break;
			default: return (uint64_t)-1;
		}

		switch(datasize) {
			case SZ_8:
				return mem[address].to_ulong();
			case SZ_16:
				return (mem[ALIGN16(address)].to_ulong() << 8) |
						mem[ALIGN16(address)+1].to_ulong();
			case SZ_32:
				return  (mem[ALIGN32(address)].to_ulong()   << 24) |
						(mem[ALIGN32(address)+1].to_ulong() << 16) |
						(mem[ALIGN32(address)+2].to_ulong() << 8)  |
						 mem[ALIGN32(address)+3].to_ulong();
			case SZ_64:
				return ((uint64_t)(mem[ALIGN64(address)].to_ulong())  << 56) |
						((uint64_t)mem[ALIGN64(address)+1].to_ulong() << 48) |
						((uint64_t)mem[ALIGN64(address)+2].to_ulong() << 40) |
						((uint64_t)mem[ALIGN64(address)+3].to_ulong() << 32) |
						((uint64_t)mem[ALIGN64(address)+4].to_ulong() << 24) |
						((uint64_t)mem[ALIGN64(address)+5].to_ulong() << 16) |
						((uint64_t)mem[ALIGN64(address)+6].to_ulong() << 8)  |
						 (uint64_t)mem[ALIGN64(address)+7].to_ulong();
			default: return (uint64_t)-1;
		}
		return (uint64_t)-1;
	}

	bool write(uint64_t data, uint32_t address, uint8_t datasize) {
		if(address >= mem.size()) return false;
		switch(datasize) {
			case SZ_8:
				mem[address]   = (uint8_t)data;
				break;
			case SZ_16:
				mem[ALIGN16(address)]   = (uint8_t)((data & 0xFF00) >> 8);
				mem[ALIGN16(address)+1] = (uint8_t)  data & 0xFF;
				break;
			case SZ_32:
				mem[ALIGN32(address)]   = (uint8_t)((data & 0xFF000000) >> 24);
				mem[ALIGN32(address)+1] = (uint8_t)((data & 0xFF0000)   >> 16);
				mem[ALIGN32(address)+2] = (uint8_t)((data & 0xFF00)     >> 8);
				mem[ALIGN32(address)+3] = (uint8_t)  data & 0xFF;
				break;
			case SZ_64:
				mem[ALIGN64(address)]   = (uint8_t)((data & 0xFF00000000000000) >> 56);
				mem[ALIGN64(address)+1] = (uint8_t)((data & 0xFF000000000000)   >> 48);
				mem[ALIGN64(address)+2] = (uint8_t)((data & 0xFF0000000000)     >> 40);
				mem[ALIGN64(address)+3] = (uint8_t)((data & 0xFF00000000)       >> 32);
				mem[ALIGN64(address)+4] = (uint8_t)((data & 0xFF000000)         >> 24);
				mem[ALIGN64(address)+5] = (uint8_t)((data & 0xFF0000)           >> 16);
				mem[ALIGN64(address)+6] = (uint8_t)((data & 0xFF00)             >> 8);
				mem[ALIGN64(address)+7] = (uint8_t)  data & 0xFF;
				break;
			default: return false;
		}
		return true;
	}
};

/**********************/
/** MAIN MEMORY: END **/
/**********************/

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
	uint64_t x[FISC_REGISTER_COUNT];
	uint32_t pc; /* Program Counter */
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
	#define REL_BRANCH(new_addr) pc = (pc+new_addr); is_branching = true;
	#define ABS_BRANCH(new_addr) pc = (new_addr);    is_branching = true;

	/* CPU Methods: */
	void stop() {
		printf("\n>> INFO: Program '%s' finished executing.\n", memory.program_filename.c_str());
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
					case ADDI: printf("(ADDI)\n");   addi(INSTR_TO_IFMT_I(instruction));  break;
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
		printf("\n>> INFO: Executing...\n\n>> Instruction count: %d\n", memory.get_prog_size() / 4);

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

		/* Clear out the registers: */
		memset(x, 0, FISC_REGISTER_COUNT);
		memset(&flags, 0, sizeof(flags_t));
	}
private:
	/********************************/
	/* Instruction Implementations: */
	/********************************/
	void add(ifmt_r_t * instr) {

	}

	void addi(ifmt_i_t * instr) {

	}

	void addis(ifmt_i_t * instr) {

	}

	void adds(ifmt_r_t * instr) {

	}

	void sub(ifmt_r_t * instr) {

	}

	void subi(ifmt_i_t * instr) {

	}

	void subis(ifmt_i_t * instr) {

	}

	void subs(ifmt_r_t * instr) {

	}

	void mul(ifmt_r_t * instr) {

	}

	void smulh(ifmt_r_t * instr) {

	}

	void umulh(ifmt_r_t * instr) {

	}

	void sdiv(ifmt_r_t * instr) {

	}

	void udiv(ifmt_r_t * instr) {

	}

	void and_(ifmt_r_t * instr) {

	}

	void andi(ifmt_i_t * instr) {

	}

	void andis(ifmt_i_t * instr) {

	}

	void ands(ifmt_r_t * instr) {

	}

	void orr(ifmt_r_t * instr) {

	}

	void orri(ifmt_i_t * instr) {

	}

	void eor(ifmt_r_t * instr) {

	}

	void eori(ifmt_i_t * instr) {

	}

	void neg(ifmt_r_t * instr) {

	}

	void negi(ifmt_i_t * instr) {

	}

	void not_(ifmt_r_t * instr) {

	}

	void noti(ifmt_i_t * instr) {

	}

	void lsl(ifmt_r_t * instr) {

	}

	void lsr(ifmt_r_t * instr) {

	}

	void movk(ifmt_iw_t * instr) {

	}

	void movz(ifmt_iw_t * instr) {

	}

	void b(ifmt_b_t * instr) {
		int32_t addr = instr->br_address;
		if(addr & 0x2000000)
			addr = -((~addr+1) & 0x3FFFFFF);
		REL_BRANCH(addr);
	}

	void bcond(ifmt_cb_t * instr) {

	}

	void bl(ifmt_b_t * instr) {

	}

	void br(ifmt_r_t * instr) {

	}

	void cbnz(ifmt_cb_t * instr) {

	}

	void cbz(ifmt_cb_t * instr) {

	}

	void ldur(ifmt_d_t * instr) {

	}

	void ldurb(ifmt_d_t * instr) {

	}

	void ldurh(ifmt_d_t * instr) {

	}

	void ldursw(ifmt_d_t * instr) {

	}

	void ldxr(ifmt_d_t * instr) {

	}

	void stur(ifmt_d_t * instr) {

	}

	void sturb(ifmt_d_t * instr) {

	}

	void sturh(ifmt_d_t * instr) {

	}

	void sturw(ifmt_d_t * instr) {

	}

	void stxr(ifmt_d_t * instr) {

	}

	/* Etc... */
};
/*****************/
/** FISC-V: END **/
/*****************/

int main(int argc, char ** argv) {
	FISCV vmachine("./prog.bin");
	/* Run the Virtual Machine: */
	vmachine.run();
	return 0;
}
