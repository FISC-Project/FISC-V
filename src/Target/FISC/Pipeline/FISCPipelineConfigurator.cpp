#include <fvm/Pass.h>
#include <stdio.h>

class PipelineConfigurator : public ConfigPass {
public:
	PipelineConfigurator() : ConfigPass(2) {}

	enum PassRetcode init() {
		printf("- Initializating Pipeline Configuration\n");
		return PASS_RET_OK;
	}

	enum PassRetcode finit() {
		printf("- Terminating Pipeline Configuration\n");
		return PASS_RET_OK;
	}

	enum PassRetcode run() {
		printf("- Configuring Pipeline\n");
		return PASS_RET_OK;
	}

	enum PassRetcode watchdog() {
		return PASS_RET_OK;
	}
};
