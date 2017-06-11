#include <fvm/Pass.h>
#include <stdio.h>

class InitFinit : public InitPass {
public:
	InitFinit() : InitPass(0) {}

	enum PassRetcode init() {
		printf("- Initializing FISC Target\n");
		return PASS_RET_OK;
	}

	enum PassRetcode finit() {
		printf("- Terminating FISC Target\n");
		return PASS_RET_OK;
	}

	enum PassRetcode run() {
		return PASS_RET_OK;
	}

	enum PassRetcode watchdog() {
		return PASS_RET_OK;
	}
};
