#include <fvm/TargetRegistry.h>
#include <fvm/Pass.h>
#include <algorithm>

TargetRegistry::TargetRegistry(std::string targetName, std::vector<Pass*> passList)
: running(false)
{
	this->targetName = targetName;
	TargetRegistry::TheTargetList.push_back(this);
	for (auto pass : passList)
		TargetRegistry::passList.push_back(pass);
}

TargetRegistry::~TargetRegistry()
{

}

bool TargetRegistry::run()
{
	std::vector<Pass*> sublistPassInitFinit;
	std::vector<Pass*> sublistPassConfig;
	std::vector<Pass*> sublistPassRun;

	/* Split the passList into different categories */
	for (auto pass : passList) 
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
		pass->setParentTargetContext(this);
	}

	/* Sort each category by priority */
	std::sort(sublistPassInitFinit.begin(), sublistPassInitFinit.end(), [](const Pass*lhs, const Pass*rhs) {
		return lhs->priority < rhs->priority;
	});
	std::sort(sublistPassConfig.begin(), sublistPassConfig.end(), [](const Pass*lhs, const Pass*rhs){
		return lhs->priority < rhs->priority;
	});
	std::sort(sublistPassRun.begin(), sublistPassRun.end(), [](const Pass*lhs, const Pass*rhs){
		return lhs->priority < rhs->priority;
	});

	running = true;

	/* First run initilization passes (the 2 foreach loops were intentional) */
	for (auto initFinitPass : sublistPassInitFinit)
		if(initFinitPass->init() != PASS_RET_OK) /* Initialize target parameters */
			return false;
	for (auto initFinitPass : sublistPassInitFinit)
		if(initFinitPass->run() != PASS_RET_OK) /* Initialize the machine at the implementation level */
			return false;

	/* Now run config pass */
	for (auto configPass : sublistPassConfig)
	{
		if(configPass->init() != PASS_RET_OK)
			return false;
		if(configPass->run() != PASS_RET_OK)
			return false;
		if(configPass->finit() != PASS_RET_OK)
			return false;
	}

	/* Initialize all machine implementations */
	for (auto runPass : sublistPassRun)
		if(runPass->init() != PASS_RET_OK)
			return false;

	/* Execute all machine implementations */
	for (auto runPass : sublistPassRun)
		if(runPass->run() != PASS_RET_OK)
			return false;

	/* The main thread will now stay here polling each 
	 * Watchdog pass and waiting for any close request 
	 * from any pass */
	/* TODO */
	
	/* Close and cleanup all machine implementations in reverse order */
	for(int i = sublistPassRun.size() - 1; i >= 0; i--)
		if(sublistPassRun[i]->finit() != PASS_RET_OK)
			return false;

	/* Finally, run finit passes */
	for (auto initFinitPass : sublistPassInitFinit)
		if(initFinitPass->finit() != PASS_RET_OK)
			return false;

	running = false;
	return true;
}

bool TargetRegistry::launchTarget(std::string targetName)
{
	for (auto target : TargetRegistry::TheTargetList)
		if(strTolower(target->targetName) == strTolower(targetName)) {
			bool success = target->run();
			target->running = false;
			return success;
		}

	/* Target not found */
	DEBUG(DERROR, "Could not find target '%s'!", targetName.c_str());
	return false;
}

bool TargetRegistry::launchTarget(unsigned int targetIndex)
{
	if(targetIndex >= 0 && targetIndex <  TargetRegistry::TheTargetList.size()) {
		bool success = TargetRegistry::TheTargetList[targetIndex]->run();
		TargetRegistry::TheTargetList[targetIndex]->running = false;
		return success;
	} else {
		/* Target not found */
		DEBUG(DERROR, "Could not find the selected target with index %d!", targetIndex);
		return false;
	}
}

Pass * TargetRegistry::getPass(Pass * passID, std::string passName)
{
	for(auto pass : passList)
		if(strTolower(pass->passName) == strTolower(passName)) {
			if(pass->verifyPassID(passID))
				return pass;
			else
				return nullptr; /* Calling Pass has no access permission to this pass */
		}
	/* Pass not found */
	return nullptr;
}

Pass * TargetRegistry::getPass(Pass * passID, unsigned int passIndex)
{
	if (passIndex >= 0 && passIndex < passList.size()) {
		if(passList[passIndex]->verifyPassID(passID))
			return passList[passIndex];
		else
			return nullptr; /* Calling Pass has no access permission to this pass */
	}
	/* Pass not found */
	return nullptr;
}
