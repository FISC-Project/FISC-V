#include <fvm/Utils/ELFLoader.h>
#include <iostream>
#include <elfio/elfio.hpp>
#include <elfio/elfio_dump.hpp>

using namespace ELFIO;

static elfio elfReader;
static bool isElfReaderInit = false;

bool initELFReader(File & file)
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

bool elfRelocate(std::vector<std::bitset<8> > & loadedELF)
{
    
    return true;
}

uint32_t elfToFlatBinary(std::vector<std::bitset<8> > & loadedELF)
{
    uint32_t byteCount = 0;
    Elf_Half n = elfReader.sections.size();

    if(n == 0)
        return byteCount;

    for (Elf_Half i = 1; i < n; i++) {
        section * sect = elfReader.sections[i];

        /* Only care about the following sections (for now) */

        if(!(sect->get_name() == ".text" || sect->get_name() == ".data"))
            continue;

        const char * data = sect->get_data();
        if (data) {
            for (uint32_t j = 0; j < sect->get_size(); j++) {
                loadedELF[byteCount] = data[j] & 0xFF;
                byteCount++;
            }
        }
    }

    /* Return the number of bytes that need to be loaded */
    return byteCount;
}
