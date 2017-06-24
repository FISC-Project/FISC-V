#ifndef RUNTIME_H_
#define RUNTIME_H_

#include <vector>
#include <string>
#include <fvm/Runtime/RunContext.h>

enum RuntimeServiceRetcode {
	RUNTIME_SERV_NULL,
	RUNTIME_SERV_OK,
	RUNTIME_SERV_ERROR,
	RUNTIME_SERV_WATCHDOG_EXPIRED,
	RUNTIME_SERV_FATAL,
	RUNTIME_SERV__COUNT
};

enum RuntimePanicSeverity {
	RUNTIME_PANIC_SEVERITY_0,
	RUNTIME_PANIC_SEVERITY_1,
	RUNTIME_PANIC_SEVERITY_2,
	RUNTIME_PANIC_SEVERITY_MAX,
};

class Runtime {
public:
	Runtime(TargetRegistry * theTarget);
	static bool launchTarget(std::string targetName);
	static bool launchTarget(unsigned int targetIndex);
private:
	static bool systemHealthy;
	bool running;
	TargetRegistry * theTarget;
	static uint32_t liveThreads;
	std::vector<std::unique_ptr<runtimeThreadContext_t> > runtimeThreads;

	static bool run(TargetRegistry * theTarget);
	static void selfDestruct(enum RuntimePanicSeverity severity, std::string lastWords, TargetRegistry * theTarget, Pass* responsiblePass);
	static void panic(enum RuntimePanicSeverity severity, TargetRegistry * theTarget);
	static enum RuntimeServiceRetcode pollRuntimePass(Pass * runtimePass);
};

#endif