#include <fvm/TargetRegistry.h>
#include <algorithm>
#include <stdio.h>

TargetRegistry::TargetRegistry(std::string targetName, std::vector<Pass*> passList)
{
	this->targetName = targetName;
	TargetRegistry::TheTargetList.push_back(this);
	for (auto pass : passList)
		TargetRegistry::passList.push_back(pass);
}

TargetRegistry::~TargetRegistry()
{

}

void TargetRegistry::run()
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

	/* First run initilization passes (the 2 foreach loops were intentional) */
	for (auto initFinitPass : sublistPassInitFinit)
		initFinitPass->init(); /* Initialize target parameters */
	for (auto initFinitPass : sublistPassInitFinit)
		initFinitPass->run(); /* Initialize the machine at the implementation level */

	/* Now run config pass */
	for (auto configPass : sublistPassConfig)
	{
		configPass->init();
		configPass->run();
		configPass->finit();
	}

	/* Initialize all machine implementations */
	for (auto runPass : sublistPassRun)
		runPass->init();

	/* Execute all machine implementations */
	for (auto runPass : sublistPassRun)
		runPass->run();

	/* The main thread will now stay here polling each 
	 * Watchdog pass and waiting for any close request 
	 * from any pass */
	/* TODO */
	
	/* Close and cleanup all machine implementations */
	for (auto runPass : sublistPassRun)
		runPass->finit();

	/* Finally, run finit passes */
	for (auto initFinitPass : sublistPassInitFinit)
		initFinitPass->finit();
}

void TargetRegistry::launchTarget(std::string targetName)
{

}

void TargetRegistry::launchTarget(unsigned int targetIndex)
{
	TargetRegistry::TheTargetList[targetIndex]->run();
}