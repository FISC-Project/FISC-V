#include <fvm/Runtime.h>
#include <fvm/TargetRegistry.h>
#include <fvm/Pass.h>
#include <algorithm>

Runtime::Runtime(TargetRegistry * theTarget)
: theTarget(theTarget)
{

}

static void runErrorDebug(std::string errorMessage, TargetRegistry * theTarget, Pass * theFailingPass)
{
	DEBUG(DERROR, errorMessage.c_str(), theTarget->targetName.c_str(), theFailingPass->passName.c_str());
}

bool Runtime::run(TargetRegistry * theTarget)
{
	std::vector<Pass*> sublistPassInitFinit;
	std::vector<Pass*> sublistPassConfig;
	std::vector<Pass*> sublistPassRun;

	/* Split the passList into different categories */
	for (auto pass : theTarget->passList)
	{
		switch (pass->type) {
		case PASS_RUNTIME_INIT_FINIT:
			sublistPassInitFinit.push_back(pass);
			break;
		case PASS_CONFIG:
			sublistPassConfig.push_back(pass);
			break;
		case PASS_RUNTIME:
			sublistPassRun.push_back(pass);
			break;
		}

		/* While we're at it, set the parent target context
		so that the passes can access each other */
		pass->setParentTargetContext(theTarget);
	}

	/* Sort each category by priority */
	std::sort(sublistPassInitFinit.begin(), sublistPassInitFinit.end(), [](const Pass*lhs, const Pass*rhs) {
		return lhs->priority < rhs->priority;
	});
	std::sort(sublistPassConfig.begin(), sublistPassConfig.end(), [](const Pass*lhs, const Pass*rhs) {
		return lhs->priority < rhs->priority;
	});
	std::sort(sublistPassRun.begin(), sublistPassRun.end(), [](const Pass*lhs, const Pass*rhs) {
		return lhs->priority < rhs->priority;
	});

	theTarget->runContext.running = true;

	/* First, run global target initilization passes serially 
	   (the 2 foreach loops were intentional) */
	for (auto initFinitPass : sublistPassInitFinit) {
		if (initFinitPass->init() != PASS_RET_OK) { /* Initialize target parameters */
			runErrorDebug("Could not globally initialize target %s @ %s", theTarget, initFinitPass);
			return false;
		}
	}
	for (auto initFinitPass : sublistPassInitFinit) {
		if (initFinitPass->run() != PASS_RET_OK) { /* Initialize the machine at the implementation level */
			runErrorDebug("Could not globally initialize the running parameters of target %s @ %s", theTarget, initFinitPass);
			return false;
		}
	}

	/* Now run all the config passes (also serially) */
	for (auto configPass : sublistPassConfig)
	{
		if (configPass->init() != PASS_RET_OK) {
			runErrorDebug("Could not initialize the configurator of target %s @ %s", theTarget, configPass);
			return false;
		}
		if (configPass->run() != PASS_RET_OK) {
			runErrorDebug("Could not configure the target %s @ %s", theTarget, configPass);
			return false;
		}
		if (configPass->finit() != PASS_RET_OK) {
			runErrorDebug("Could not terminate configuration for target %s @ %s", theTarget, configPass);
			return false;
		}
	}

	/* Initialize all machine implementations serially */
	for (auto runPass : sublistPassRun) {
		if (runPass->init() != PASS_RET_OK) {
			runErrorDebug("Could not initialize implementation of target %s @ %s", theTarget, runPass);
			return false;
		}
	}

	/* Execute all machine implementations all in separate threads */
	for (auto runPass : sublistPassRun) {
		if (runPass->run() != PASS_RET_OK) {
			runErrorDebug("Execution of target %s @ %s finished with errors", theTarget, runPass);
			return false;
		}
	}

	/* The main thread will now stay here polling each
	   Watchdog pass and waiting for any close request
	   from any pass */
	/* TODO */

	/* Close and cleanup all machine implementations in reverse order and serially */
	for (int i = sublistPassRun.size() - 1; i >= 0; i--) {
		if (sublistPassRun[i]->finit() != PASS_RET_OK) {
			runErrorDebug("Could not terminate implementation of target %s @ %s", theTarget, sublistPassRun[i]);
			return false;
		}
	}

	/* Finally, run finit passes serially */
	for (auto initFinitPass : sublistPassInitFinit) {
		if (initFinitPass->finit() != PASS_RET_OK) {
			runErrorDebug("Could not globally terminate target %s @ %s", theTarget, initFinitPass);
			return false;
		}
	}

	theTarget->runContext.running = false;
	return true;
}

bool Runtime::launchTarget(std::string targetName)
{
	for (auto target : TargetRegistry::TheTargetList)
		if (strTolower(target->targetName) == strTolower(targetName)) {
			bool success = run(target);
			target->runContext.running = false;
			return success;
		}

	/* Target not found */
	DEBUG(DERROR, "Could not find target '%s'!", targetName.c_str());
	return false;
}

bool Runtime::launchTarget(unsigned int targetIndex)
{
	if (targetIndex >= 0 && targetIndex <  TargetRegistry::TheTargetList.size()) {
		bool success = run(TargetRegistry::TheTargetList[targetIndex]);
		TargetRegistry::TheTargetList[targetIndex]->runContext.running = false;
		return success;
	}
	else {
		/* Target not found */
		DEBUG(DERROR, "Could not find the selected target with index %d!", targetIndex);
		return false;
	}
}