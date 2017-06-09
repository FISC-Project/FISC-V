#include <fvm/Pass.h>
#include <stdio.h>

class InitFinit : public Pass {
public:
	InitFinit() : Pass(PASS_RUNTIME_INIT_FINIT, 0) {}

	enum PassRetcode init() {
		printf("- Initializing FISC Target\n");
		return PASS_RET_OK;
	}

	enum PassRetcode finit() {
		printf("- Closing and cleaning up the FISC implementation and target\n");
		return PASS_RET_OK;
	}

	enum PassRetcode run() {
		printf("- Initializing FISC Implementation\n");
		return PASS_RET_OK;
	}

	enum PassRetcode watchdog() {
		return PASS_RET_OK;
	}
};
