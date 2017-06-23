#include <fvm/Utils/String.h>
#include <algorithm>
#include <locale>
#include <cctype>

std::string strTolower(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

std::string strToupper(std::string str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	return str;
}

bool strIsNumber(const std::string & s)
{
	char * p;

	if(strIsHexNumber(s))
		/* Try decimal format */
		strtol(s.substr(2, s.size()).c_str(), &p, 16);
	else if(strIsBinNumber(s))
		/* Try binary format */
		strtol(s.substr(2, s.size()).c_str(), &p, 2);
	else if(strIsOctalNumber(s))
		/* Try octal format */
		strtol(s.substr(2, s.size()).c_str(), &p, 8);
	else
		strtol(s.c_str(), &p, 10);	

	return *p == 0;
}

bool strIsBinNumber(const std::string & s)
{
	return strTolower(s.substr(0, 2)) == "0b" || strTolower(s.substr(0, 3)) == "-0b";
}

bool strIsOctalNumber(const std::string & s)
{
	std::string sCopy = strTolower(s);
	return sCopy.size() > 1 && 
		((sCopy[0] == '0' && sCopy[1] != 'x' && sCopy[1] != 'b') || 
		s.substr(0, 2) == "-0");
}

bool strIsHexNumber(const std::string & s)
{
	return strTolower(s.substr(0, 2)) == "0x" || strTolower(s.substr(0, 3)) == "-0x";
}