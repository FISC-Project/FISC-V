#define HALT asm("bl 0");

int array[2] = {1,2};

int main(int argc, char ** argv)
{
	int * counter = (int*)0x100;
	*counter = array[1];

	HALT;
	return 0;
}