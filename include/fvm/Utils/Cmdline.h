#ifndef CMDLINE_H_
#define CMDLINE_H_

#include <string>

/* Command line API: */
extern void cmdlineParse(int argc, char ** argv);
extern bool cmdHasOpt(std::string opt, unsigned nth);
extern bool cmdHasOpt(char opt, unsigned nth);
extern bool cmdHasOpt(std::string opt);
extern bool cmdHasOpt(char opt);
extern unsigned cmdGetOptCount(void);
extern std::pair<std::string, std::string> cmdQuery(std::string optQuery, unsigned nth);
extern std::pair<std::string, std::string> cmdQuery(std::string optQuery);
extern std::pair<char, std::string> cmdQuery(char optQuery, unsigned nth);
extern std::pair<char, std::string> cmdQuery(char optQuery);

#endif