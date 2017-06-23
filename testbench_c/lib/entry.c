#define HALT asm("bl 0");

extern void start();

int main(int argc, char ** argv)
{
	start();
	HALT;
	return 0;
}
