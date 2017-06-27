#ifndef STDIO_H_
#define STDIO_H_

#include "string.h"

#define IOSPACE_VMCONSOLE_OUT 0x5000
#define IOSPACE_VMCONSOLE_IN  0x5001

void putc(char ch)
{
	*((char*)IOSPACE_VMCONSOLE_OUT) = (char)ch;
}

void puts(char * str)
{
	for(size_t i = 0; i < strlen(str); i++)
		putc(str[i]);
}

#define DEBUGLOC 0x10000
#define DEBUG(loc, intnum) (((uint32_t*)DEBUGLOC)[(loc)] = ((uint32_t)(intnum)))
#define DEBUGLIST(listsize, list) for(size_t _i_ = 0; _i_ < ((size_t)(listsize)); _i_++) DEBUG(_i_, ((uint32_t*)(list)[_i_]));
#define DEBUGLISTOFF(offset, listsize, list) for(size_t _i_ = ((size_t)(offset)); _i_ < ((size_t)(offset)) + ((size_t)(listsize)); _i_++) DEBUG(_i_, ((uint32_t*)(list)[_i_ - ((size_t)(offset))]));

#endif