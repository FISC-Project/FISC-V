#ifndef RUNTIME_H_
#define RUNTIME_H_

#include <string>

class TargetRegistry;

class Runtime {
public:
	Runtime(TargetRegistry * theTarget);
	static bool launchTarget(std::string targetName);
	static bool launchTarget(unsigned int targetIndex);
private:
	bool running;
	TargetRegistry * theTarget;
	static bool run(TargetRegistry * theTarget);
};

#endif