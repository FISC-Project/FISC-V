#include <fvm/TargetRegistry.h>
#include <fvm/Pass.h>

TargetRegistry::TargetRegistry(std::string targetName, std::vector<Pass*> passList)
: runContext(this)
{
	this->targetName = targetName;
	TargetRegistry::TheTargetList.push_back(this);
	for (auto pass : passList)
		TargetRegistry::passList.push_back(pass);
}

TargetRegistry::~TargetRegistry()
{

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
