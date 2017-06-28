#ifndef STDIO_H_
#define STDIO_H_

#include "iospace.h"
#include "string.h"
#include "stdint.h"
#include "attr.h"

/**********************/
/******* OUTPUT *******/
/**********************/
void wait_stdout_flush()
{
	while(!io->VMConsole.wrRdy);
}

void putc(char ch)
{
	/* Send byte to virtual console */
	io->VMConsole.wr = ch;
}

void puts(char * str)
{
	/* Send full string to virtual console */
	for(size_t i = 0; i < strlen(str); i++)
		putc(str[i]);
}

/*********************/
/******* INPUT *******/
/*********************/
bool kbhit()
{
	return io->VMConsole.rdRdy;
}

char getch()
{
	while(!kbhit());
	return io->VMConsole.rd;
}

char getch_async()
{
	return io->VMConsole.rd;
}

#define DEBUGLOC 0x10000
#define DEBUG(loc, intnum) (((uint32_t*)DEBUGLOC)[(loc)] = ((uint32_t)(intnum)))
#define DEBUGLIST(listsize, list) for(size_t _i_ = 0; _i_ < ((size_t)(listsize)); _i_++) DEBUG(_i_, ((uint32_t*)(list)[_i_]));
#define DEBUGLISTOFF(offset, listsize, list) for(size_t _i_ = ((size_t)(offset)); _i_ < ((size_t)(offset)) + ((size_t)(listsize)); _i_++) DEBUG(_i_, ((uint32_t*)(list)[_i_ - ((size_t)(offset))]));

#endif