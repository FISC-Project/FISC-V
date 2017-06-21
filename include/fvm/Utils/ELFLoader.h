#ifndef ELFLOADER_H_
#define ELFLOADER_H_

#include <fvm/Utils/IO/File.h>
#include <vector>
#include <bitset>

typedef struct {
	uint32_t    start; /* Byte aligned        */
	uint32_t    end;   /* Byte aligned        */
	std::string name;  /* Name of the section */
	bool        isLittleEndian; /* Indicates whether this section contains big (0) or little (1) endian data */
} elfsection_t;

typedef std::vector<elfsection_t> elfsection_list_t;

bool isFileELF(File & file);
uint32_t elfToFlatBinary(std::vector<std::bitset<8> > & loadedELF, elfsection_list_t & elfsection_list, bool textIsLittle, bool dataIsLittle);

#endif