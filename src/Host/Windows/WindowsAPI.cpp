#include <fvm/Host/Windows/WindowsAPI.h>

BOOL winapi_consoleAttributesStored = FALSE;
WORD winapi_consoleDefaultAttributes = 0;

static BOOL winapi_console_storeDefaultAttributes()
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info))
		return FALSE;
	winapi_consoleDefaultAttributes = info.wAttributes;
	winapi_consoleAttributesStored = TRUE;
	return TRUE;
}

BOOL winapi_console_reset()
{
	if(!winapi_consoleAttributesStored)
		winapi_console_storeDefaultAttributes();
	return SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), winapi_consoleDefaultAttributes);
}

BOOL winapi_console_setAttr(WORD c)
{
	if (!winapi_consoleAttributesStored)
		winapi_console_storeDefaultAttributes();
	return SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), c);
}

WORD winapi_console_getAttr()
{
	if (!winapi_consoleAttributesStored)
		winapi_console_storeDefaultAttributes();
	CONSOLE_SCREEN_BUFFER_INFO info;
	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info))
		return 0;
	return info.wAttributes;
}

WORD winapi_console_getDefaultAttr()
{
	if (!winapi_consoleAttributesStored)
		winapi_console_storeDefaultAttributes();
	return winapi_consoleDefaultAttributes;
}

BOOL winapi_console_print(std::ostream & os, std::string str, int console_attr)
{
	if (!winapi_consoleAttributesStored)
		winapi_console_storeDefaultAttributes();
	BOOL success = SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), console_attr);
	os << str;
	return success;
}

BOOL winapi_console_print(std::ostream & os, std::string str)
{
	BOOL success = winapi_console_reset();
	os << str;
	return success;
}
