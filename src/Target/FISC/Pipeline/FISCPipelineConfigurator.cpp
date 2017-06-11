#pragma once
#include <fvm/Pass.h>
#include <stdio.h>

class PipelineConfigurator : public ConfigPass {
public:
	PipelineConfigurator() : ConfigPass(2) {}

	enum PassRetcode init() {
		return PASS_RET_OK;
	}

	enum PassRetcode finit() {
		return PASS_RET_OK;
	}

	enum PassRetcode run() {
		return PASS_RET_OK;
	}

	enum PassRetcode watchdog() {
		return PASS_RET_OK;
	}
};
