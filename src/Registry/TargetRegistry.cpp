#include <fvm/TargetRegistry.h>
#include <fvm/Pass.h>

TargetRegistry::TargetRegistry(std::string targetName,
	                           std::string targetNameLong, 
			                   std::string targetOwnerDescription, 
			               	   std::vector<Pass*> passList)
: runContext(this)
{
	this->targetName = targetName;
	this->targetNameLong = targetNameLong;
	this->targetOwnerDescription = targetOwnerDescription;
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

enum PassStatus TargetRegistry::getPassStatus(Pass * passID, std::string passName)
{
	Pass * pass = getPass(passID, passName);
	if(pass == nullptr) /* It's possible the pass doesn't exist. We assume NOAUTH regardless */
		return PASS_STATUS_NOAUTH;
	return pass->getStatus();
}

enum PassStatus TargetRegistry::getPassStatus(Pass * passID, unsigned int passIndex)
{
	Pass * pass = getPass(passID, passIndex);
	if (pass == nullptr) /* It's possible the pass doesn't exist. We assume NOAUTH regardless */
		return PASS_STATUS_NOAUTH;
	return pass->getStatus();
}

enum PassStatus TargetRegistry::waitForPassToFinish(Pass * passID, std::string passName)
{
	Pass * pass = getPass(passID, passName);
	if (pass == nullptr) /* It's possible the pass doesn't exist. We assume NOAUTH regardless */
		return PASS_STATUS_NOAUTH;

	enum PassStatus retcode = PASS_STATUS_NULL;
	while (1) {
		switch ((retcode = pass->getStatus())) {
		case PASS_STATUS_NULL: case PASS_STATUS_NOTSTARTED: case PASS_STATUS_NOAUTH: case PASS_STATUS_COMPLETED: case PASS_STATUS_COMPLETEDWITHWARNINGS:
		case PASS_STATUS_COMPLETEDWITHERRORS: case PASS_STATUS_COMPLETEDWITHFATALERRORS: /* Finished execution */ return retcode;
		default: /* The pass is still running */ break;
		}
	}
	return retcode;
}

enum PassStatus TargetRegistry::waitForPassToFinish(Pass * passID, unsigned int passIndex)
{
	Pass * pass = getPass(passID, passIndex);
	if (pass == nullptr) /* It's possible the pass doesn't exist. We assume NOAUTH regardless */
		return PASS_STATUS_NOAUTH;

	enum PassStatus retcode = PASS_STATUS_NULL;
	while (1) {
		switch ((retcode = pass->getStatus())) {
		case PASS_STATUS_NULL: case PASS_STATUS_NOTSTARTED: case PASS_STATUS_NOAUTH: case PASS_STATUS_COMPLETED: case PASS_STATUS_COMPLETEDWITHWARNINGS:
		case PASS_STATUS_COMPLETEDWITHERRORS: case PASS_STATUS_COMPLETEDWITHFATALERRORS: /* Finished execution */ return retcode;
		default: /* The pass is still running */ break;
		}
	}
	return retcode;
}