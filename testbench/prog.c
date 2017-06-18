#define HALT asm("bl 0");

int main(int argc, char ** argv)
{
	int * counter = 0;
	counter = 0;
	for(int i = 0; i < 2; i++) {
		(void)*(counter++);
	}

	HALT;
	return 0;
}