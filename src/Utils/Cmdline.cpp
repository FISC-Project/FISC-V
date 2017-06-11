#include <string>
#include <utility>
#include <vector>
#include <fvm/Utils/String.h>
using namespace std;

static vector<std::pair<char, string>> opts_single_list;
static vector<std::pair<string, string>> opts_list;

char isOpt(string opt)
{
	return !opt.find("--") ? 2 : !opt.find("-") ? 1 : 0;
}

char getSingleOpt(string opt)
{
	for (unsigned i = 0; i < opt.size(); i++)
		if (opt[i] != '-')
			return opt[i];
	return -1;
}

string getStringOpt(string opt)
{
	for (unsigned i = 0; i < opt.size(); i++)
		if (opt[i] != '-')
			return opt.substr(i, opt.size());
	return NULLSTR;
}

/* Pushes options such as -o, -c, -e, etc */
void cmdPushSingle(string opt, string arg)
{
	opts_single_list.push_back(make_pair(getSingleOpt(opt), arg));
}

/* Pushes options such as --opt=, --compile, --enable="feature", etc */
void cmdPush(string opt, string arg)
{
	opts_list.push_back(make_pair(getStringOpt(opt), arg));
}

std::pair<char, string> cmdQuery(char optQuery)
{
	for (auto pair : opts_single_list)
		if (pair.first == optQuery) return pair;
	return make_pair(0, NULLSTR);
}

std::pair<string, string> cmdQuery(string optQuery)
{
	for (auto pair : opts_list)
		if (pair.first == optQuery) return pair;
	return make_pair(NULLSTR, NULLSTR);
}

bool cmdHasOpt(char opt)
{
	for (auto pair : opts_single_list)
		if (pair.first == opt) return true;
	return false;
}

bool cmdHasOpt(string opt)
{
	for (auto pair : opts_list)
		if (pair.first == opt) return true;
	return false;
}

unsigned cmdGetOptCount(void)
{
	return opts_single_list.size() + opts_list.size();
}

void cmdlineParse(int argc, char ** argv)
{
	/* Iterate through the cmdline: */
	int ctr = 1;
	while (ctr < argc) {
		string opt = string(argv[ctr++]);
		char opt_type;
		if ((opt_type = isOpt(opt)))
			if (opt_type == 1)
				cmdPushSingle(opt, ctr < argc ? string(argv[ctr]) : NULLSTR);
			else if (opt_type == 2)
				cmdPush(opt, ctr < argc ? string(argv[ctr]) : NULLSTR);
	}
}