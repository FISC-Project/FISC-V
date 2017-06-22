#define DEBUGLOC 0x10000
#define DEBUG(loc, intnum) (((unsigned int*)DEBUGLOC)[(loc)] = ((unsigned int)(intnum)))

typedef int (*fptr_t)(void);

int foo(void)
{
	return 7;
}

void start()
{
	fptr_t fptr = &foo;
	int r = fptr();

	DEBUG(0, fptr());
	DEBUG(1, fptr());
	DEBUG(2, r);
	DEBUG(3, 10);
}
