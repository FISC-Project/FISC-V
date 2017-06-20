#include <fvm/Utils/ELFLoader.h>
#include <iostream>
#include <elfio/elfio.hpp>

using namespace ELFIO;

typedef struct {
    std::string   name;
    Elf64_Addr    value;
    Elf_Xword     size;
    Elf_Xword     originalSize;
    int           fixupSizeDifference;
    unsigned char bind;
    unsigned char type;
    Elf_Half      section_index;
    unsigned char other;
} symprop_t;

static elfio elfReader;
static bool isElfReaderInit = false;

static bool initELFReader(File & file)
{
    return (isElfReaderInit = elfReader.load(file.fileName));
}

bool isFileELF(File & file)
{
    if(!isElfReaderInit)
        if(!initELFReader(file))
            return false;
    return true;
}

uint32_t elfToFlatBinary(std::vector<std::bitset<8> > & loadedELF)
{
    uint32_t byteCount = 0;
    uint32_t totalDataByteCount = 0;
    Elf_Half n = elfReader.sections.size();

    if(n == 0)
        return byteCount;

    for (Elf_Half i = 1; i < n; i++) {
        section * sect = elfReader.sections[i];
        std::string sectName = sect->get_name();
        const char * data = sect->get_data();
        if(!data)
            continue;
        
        /* Only care about the following sections (for now) */

        if (sectName == ".text") {
            /* Copy the .text section to the memory as is */
            for (uint32_t j = 0; j < sect->get_size(); j++)
                loadedELF[byteCount++] = data[j] & 0xFF;
        }
        else if (sectName == ".data") {
            /* We need to copy the .data section in big endian byte order */
            Elf_Half data_section_index = sect->get_index();
            
            for (auto symsec : elfReader.sections) {
                if (symsec->get_type() == SHT_SYMTAB) {
                    
                    const symbol_section_accessor symbols(elfReader, symsec);
                    unsigned int symCount = (unsigned int)symbols.get_symbols_num();
                    
                    if (symCount == 0)
                        continue;

                    /* Fetch all valid .data symbols */
                    std::vector<symprop_t> symList;
                    int symListSize = 0;
                    
                    for (unsigned int j = 0; j < symCount; j++) {
                        symprop_t tmp;
                        /* Read symbol properties */
                        symbols.get_symbol(j, tmp.name, tmp.value, tmp.size, tmp.bind, tmp.type, tmp.section_index, tmp.other);
                        
                        if (tmp.type == STT_OBJECT && tmp.section_index == data_section_index) {
                            /* Found a valid data symbol.
                               Store it into the symbol list */
                            symList.push_back(tmp);
                        }
                    }

                    symListSize = symList.size();
                    if (symListSize == 0)
                        continue;

                    /* Sort the symbols by their address offset */
                    std::sort(symList.begin(), symList.end(), [](const symprop_t& lhs, const symprop_t& rhs)
                    {
                        return lhs.value < rhs.value;
                    });

                    /* Fixup the size alignments */
                    symList[symListSize - 1].originalSize = symList[symListSize - 1].size;
                    symList[symListSize - 1].fixupSizeDifference = 0;

                    for (int j = symListSize - 2; j >= 0; j--) {
                        symList[j].originalSize = symList[j].size;
                        if(symList[j].size < symList[j+1].originalSize)
                            symList[j].size = symList[j+1].size;
                        symList[j].fixupSizeDifference = ((int)symList[j].size) - ((int)symList[j].originalSize);
                    }
                    
                    
                    /* Finally, copy the raw symbol data into the elf buffer
                       with big endian byte order */
                    for (auto sym : symList) {                                               
                        int subCounter = 0;
                        for (int byten = ((int)sym.originalSize) - 1; byten >= 0; byten--) {
                            loadedELF[byteCount + ((unsigned int)sym.value) + (subCounter++)] = data[sym.value + byten] & 0xFF;
                            totalDataByteCount++;
                        }

                        if(sym.fixupSizeDifference > 0) {
                            /* Fill up the rest with zeros */
                            for (int byten = 0; byten < sym.fixupSizeDifference; byten++) {
                                loadedELF[byteCount + ((unsigned int)sym.value) + (subCounter++)] = 0;
                                totalDataByteCount++;
                            }
                        }
                    }
                }
            }
        }
    }

    byteCount += totalDataByteCount;

    /* Return the number of bytes that need to be loaded */
    return byteCount;
}
