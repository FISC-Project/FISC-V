#include <fvm/Utils/String.h>
#include <algorithm>
#include <locale>
#include <cctype>

std::string & strTolower(std::string & str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

std::string & strToupper(std::string & str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	return str;
}

bool strIsNumber(const std::string& s)
{
	char * p;
	strtol(s.c_str(), &p, 10);

	return (*p == 0);
}
