#ifndef UTILS_STRING_H_
#define UTILS_STRING_H_

#include <string>

#define STR(str) #str
#define STRING(str) STR(str)

#define NULLSTR "(null)"

extern std::string strTolower(std::string str);
extern std::string strToupper(std::string str);
extern bool strIsNumber(const std::string& s);

#endif