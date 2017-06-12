#include <fvm/Pass.h>
#include <stdio.h>

class InitFinit : public InitPass {
public:
	InitFinit() : InitPass(0) {}

	enum PassRetcode init() {
		DEBUG(DGOOD, "Initializing FISC Target");
		return PASS_RET_OK;
	}

	enum PassRetcode finit() {
		DEBUG(DGOOD, "Terminating FISC Target");
		return PASS_RET_OK;
	}

	enum PassRetcode run() {
		return PASS_RET_OK;
	}

	enum PassRetcode watchdog() {
		return PASS_RET_OK;
	}
};
