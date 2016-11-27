/*
 * memory.h
 *
 *  Created on: 27/11/2016
 *      Author: Miguel
 */

#ifndef SRC_MEMORY_H_
#define SRC_MEMORY_H_

#include <bitset>
#include <fstream>
#include <vector>
#include <stdint.h>
#include <stdio.h>
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

#endif /* SRC_MEMORY_H_ */
