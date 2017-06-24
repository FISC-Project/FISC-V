#ifndef RUNCONTEXT_H_
#define RUNCONTEXT_H_

#include <memory>
#include <TinyThread++-1.1/tinythread.h>
#include <TinyThread++-1.1/fast_mutex.h>

using namespace tthread;

class TargetRegistry;
class Pass;

typedef struct {
	Pass * theRuntimePass;
	enum PassRetcode retval;
	bool hasReturned;
} runtimeLaunchCommandPacket_t;

typedef struct {
	std::unique_ptr<thread> theThread;
	runtimeLaunchCommandPacket_t runCmd;
	bool alreadyJoined;
} runtimeThreadContext_t;

#endif