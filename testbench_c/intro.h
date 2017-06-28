void showHeaderFooterLine()
{
	puts("\n________________________________________________________");
}

void welcome()
{
	initialize_fisc_machine();
	showHeaderFooterLine();
	puts("\n\n            W E L C O M E    T O    T H E ");
	puts("\n      F I S C    V I R T U A L      M A C H I N E\n");
	showHeaderFooterLine();
	puts("\n");
}

void goodbye()
{
	puts("\n\n");
	showHeaderFooterLine();
	puts("\n\n> Closing the FISC CPU ...\n> Goodbye");
	showHeaderFooterLine();
	puts("\n");
}