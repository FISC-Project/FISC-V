#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#define ELFIO_NO_INTTYPES
#endif

#include <fvm/Utils/ELFLoader.h>
#include <iostream>
#include <algorithm>
#include <elfio/elfio.hpp>

using namespace ELFIO;

/* We care about only these two sections (for now) */
#define TEXTSECT ".text"
#define DATASECT ".data"

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

uint32_t elfToFlatBinary(std::vector<std::bitset<8> > & loadedELF, elfsection_list_t & elfsection_list, bool textIsLittle, bool dataIsLittle)
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

        if (sectName == TEXTSECT || sectName == DATASECT) {
            /* Copy the .text section to the memory as is */
            for (uint32_t j = 0; j < sect->get_size(); j++)
                loadedELF[byteCount++] = data[j] & 0xFF;

            if (sectName == TEXTSECT) {
                elfsection_t textSection;
                textSection.start = 0;
                textSection.end = byteCount;
                textSection.name = TEXTSECT;
                textSection.isLittleEndian = textIsLittle;
                elfsection_list.push_back(textSection);
            }
            else if (sectName == DATASECT) {
                for (auto & textSect : elfsection_list) {
                    if (textSect.name == TEXTSECT) {
                        elfsection_t dataSection;
                        dataSection.start = textSect.end;
                        dataSection.end = byteCount-1;
                        dataSection.name = DATASECT;
                        dataSection.isLittleEndian = dataIsLittle;
                        elfsection_list.push_back(dataSection);
                        break;
                    }
                }
            }
        }
    }

    byteCount += totalDataByteCount;

    /* Return the number of bytes that need to be loaded */
    return byteCount;
}
