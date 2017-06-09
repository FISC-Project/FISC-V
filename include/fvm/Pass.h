#ifndef PASS_H_
#define PASS_H_

#include <string>

enum PassType {
	PASS_NULL,
	PASS_RUNTIME_INIT_FINIT,
	PASS_CONFIG,
	PASS_RUNTIME,
	PASS_COUNT
};

enum PassRetcode {
	PASS_RET_NULL,
	PASS_RET_OK,
	PASS_RET_ERR,
	PASS_RET_COUNT
};

class Pass {
public:
	Pass(enum PassType type, unsigned int priority);
	virtual ~Pass() = 0;
	virtual enum PassRetcode init() = 0;
	virtual enum PassRetcode finit() = 0;
	virtual enum PassRetcode run() = 0;
	virtual enum PassRetcode watchdog() = 0;

	enum PassType type;
	std::string passName;
	unsigned int priority;
};

#endif
