#ifndef FISC_SYSTEM_H_
#define FISC_SYSTEM_H_

void initialize_stdout()
{
	/* Enable device first */
	io->VMConsole.enDev = 1;
	/* Enable stdout writing second */
	io->VMConsole.enWr = 1;
}

void initialize_stdin()
{
	/* Enable device first */
	io->VMConsole.enDev = 1;
	/* Enable stdin reading second */
	io->VMConsole.enRd = 1;
	/* The text must be sent synchronously */
}

void enable_timer()
{
	/* Enable device first */
	io->Timer.enDev = 1;
	/* Enable stdout writing second */
	io->Timer.enTimer = 1;
}

void initialize_fisc_machine()
{	
	/* Enable standard I/O streams */
	initialize_stdout(); /* Output */
	initialize_stdin();  /* Input */

	/* Finally, enable the timer for triggering CPU interrupts */
	//enable_timer();
}

#endif