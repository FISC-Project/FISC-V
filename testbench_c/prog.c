#include "lib/lib.h"

void start()
{
	int arraysize = 5;

	/* Create and store data on ptr1 */
	int * ptr1 = (int*)malloc(sizeof(int) * arraysize);
	for(int i = 0; i <= arraysize; i++)
		ptr1[i] = i + 1;

	/* Create ptr2 and copy the data from ptr1 */
	int * ptr2 = (int*)malloc(sizeof(int) * arraysize);
	memcpy(ptr2, ptr1, sizeof(int) * arraysize);

	/* Show ptr2 contents */
	DEBUGLIST(arraysize, ptr2);

	puts("\nHello World\n");
}
