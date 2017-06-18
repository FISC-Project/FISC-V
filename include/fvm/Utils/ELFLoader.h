#ifndef ELFLOADER_H_
#define ELFLOADER_H_

#include <fvm/Utils/IO/File.h>
#include <vector>
#include <bitset>

bool isFileELF(File & file);
uint32_t elfToFlatBinary(std::vector<std::bitset<8> > & loadedELF);

#endif