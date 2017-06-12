#include <fvm/Pass.h>

Pass::Pass(enum PassType type, unsigned int priority)
: parentTarget(nullptr), isTargetSet(false), resourcesLocked(false)
{
	this->type = type;
	this->priority = priority;
	whitelist.clear();
}

Pass::~Pass()
{

}

bool Pass::setParentTargetContext(TargetRegistry * parentTarget)
{
	if(!isTargetSet) {
		this->parentTarget = parentTarget;
		isTargetSet = true;
		return isTargetSet;
	}
	else {
		/* Cannot re-set the target context now */
		return false;
	}
}

TargetRegistry * Pass::getTarget()
{
	return parentTarget;
}

Pass * Pass::getPass(std::string passName)
{
	return parentTarget ? parentTarget->getPass(this, passName) : nullptr;
}

Pass * Pass::getPass(unsigned int passIndex)
{
	return parentTarget ? parentTarget->getPass(this, passIndex) : nullptr;
}

bool Pass::shareResource(Pass * passID, void * genericPtr, std::string resourceName)
{
	if(resourcesLocked) return false; /* Can't touch these if they're locked */
	if (!verifyPassID(passID, resourceName)) return false;
	passResource_t newResource;
	newResource.name = resourceName;
	newResource.ptr = genericPtr;
	newResource.ptr_saved = genericPtr;
	newResource.isFn = false;
	newResource.locked = false;
	resourceList.push_back(newResource);
	return true;
}

bool Pass::shareResource(Pass * passID, std::string resourceName)
{
	if (resourcesLocked) return false; /* Can't touch these if they're locked */
	if (!verifyPassID(passID, resourceName)) return false;
	passResource_t newResource;
	newResource.name = resourceName;
	newResource.ptr = nullptr;
	newResource.ptr_saved = nullptr;
	newResource.isFn = true;
	newResource.locked = false;
	resourceList.push_back(newResource);
	return true;
}

void * Pass::switchResourceFn(Pass * passID, std::string resourceName)
{
	if(!switchResourceFnProtect(passID, resourceName)) return nullptr;
	/* The specialization class must implement this */
	return nullptr;
}

bool Pass::releaseResource(Pass * passID, std::string resourceName)
{
	if (resourcesLocked) return false; /* Can't touch these if they're locked */
	if (!verifyPassID(passID, resourceName)) return false;
	for (unsigned int i = 0; i < resourceList.size(); i++)
		if (strTolower(resourceList[i].name) == strTolower(resourceName)) {
			resourceList.erase(resourceList.begin() + i);
			break;
		}
	return true;
}

void * Pass::getResource(Pass * passID, std::string resourceName)
{
	if (resourcesLocked) return nullptr; /* Can't touch these if they're locked */
	if (!verifyPassID(passID, resourceName)) return nullptr;
	for(passResource_t res : resourceList)
		if (strTolower(res.name) == strTolower(resourceName))
			return res.ptr;
	/* Couldn't find the resource */
	return nullptr;
}

void * Pass::getResourceFn(Pass * passID, std::string resourceName)
{
	if (resourcesLocked) return nullptr; /* Can't touch these if they're locked */
	if (!verifyPassID(passID, resourceName)) return nullptr;
	for (passResource_t res : resourceList)
		if (strTolower(res.name) == strTolower(resourceName))
			return switchResourceFn(passID, resourceName);
	/* Couldn't find the resource */
	return nullptr;
}

bool Pass::lockResource(Pass * passID, std::string resourceName)
{
	if (resourcesLocked) return false; /* Can't touch these if they're locked */
	if (!verifyPassID(passID, resourceName)) return false;
	for (passResource_t res : resourceList)
		if (strTolower(res.name) == strTolower(resourceName)) {
			/* Deny access to the data */
			res.ptr = nullptr;
			res.locked = true;
			break;
		}
	return true;
}

bool Pass::unlockResource(Pass * passID, std::string resourceName)
{
	if (resourcesLocked) return false; /* Can't touch these if they're locked */
	if (!verifyPassID(passID, resourceName)) return false;
	for (passResource_t res : resourceList)
		if (strTolower(res.name) == strTolower(resourceName)) {
			/* Restore access */
			res.ptr = res.ptr_saved;
			res.locked = false;
			break;
		}
	return true;
}

bool Pass::lockAllResources(Pass * passID)
{
	if (resourcesLocked) return false; /* Can't lock what's already locked */
	if(!verifyPassID(passID)) return false;
	resourcesLocked = true; /* No one can touch these pointers now */
	for (passResource_t res : resourceList) {
		/* Deny access to the data */
		res.ptr = nullptr;
	}
	/* The owner must call unlockAllResources() to allow access to them again */
	return true;
}

bool Pass::unlockAllResources(Pass * passID)
{
	if (!verifyPassID(passID)) return false;
	resourcesLocked = false;
	for (passResource_t res : resourceList) {
		/* Restore access */
		res.ptr = res.ptr_saved;
	}
	return true;
}

bool Pass::switchResourceFnProtect(Pass * passID, std::string resourceName)
{
	if (resourcesLocked) return false;
	if (!verifyPassID(passID, resourceName)) return false;
	return true;
}

void Pass::setWhitelist(std::vector<std::pair<std::string, std::vector<std::string> > > whitelist)
{
	this->whitelist = whitelist;
}

bool Pass::verifyPassID(Pass * passID)
{
	if(passID == this) {
		/* The owner class called itself */
		return true;
	} else {
		/* Check the white list to see if this Pass has permission */
		std::string passName = passID->passName;
		if(!whitelist.size())
			return true; /* The whitelist is empty. Just give access to everyone */
		for (std::pair<std::string, std::vector<std::string> > whitelistEntry : whitelist) {
			if (strTolower(whitelistEntry.first) == strTolower(passID->passName) || 
				strTolower(whitelistEntry.first) == strTolower(passID->passNameLong))
			{
				return true; /* Ok, this Pass has permission */
			}
		}
	}
	/* No can do */
	return false;
}

bool Pass::verifyPassID(Pass * passID, std::string resourceName)
{
	/* Let's check if the calling Pass has access to this Pass at all */
	if (passID == this)
		return true; /* The owner class called itself */
	if(!verifyPassID(passID))
		return false;

	if (!whitelist.size())
		return true; /* The whitelist is empty. Just give access to everyone */
	
	/* Ok, so does the calling Pass have access to this particular resource? */
	for (std::pair<std::string, std::vector<std::string> > whitelistEntry : whitelist) {
		if (strTolower(whitelistEntry.first) == strTolower(passID->passName) || 
			strTolower(whitelistEntry.first) == strTolower(passID->passNameLong))
		{
			if(!whitelistEntry.second.size()) {
				/* Before we declare victory, let's see if the resource is lcoked */
				for (auto res : resourceList)
					if (strTolower(res.name) == strTolower(resourceName) && res.locked)
						return false; /* Oops, the Pass almost got it. Unfortunately, the resource is locked, even if the Pass has permission */
				/* The Pass has permission but the resource list is empty. This means the calling Pass can have everything */
				return true;
			} else {
				for(std::string resName : whitelistEntry.second) {
					if(strTolower(resName) == strTolower(resourceName)) {
						/* Before we declare victory, let's see if the resource is lcoked */
						for(auto res : resourceList)
							if(strTolower(res.name) == strTolower(resourceName) && res.locked)
								return false; /* Oops, the Pass almost got it. Unfortunately, the resource is locked, even if the Pass has permission */
						return true; /* Yep, the Pass has total access. At least to this particular resource */
					}
				}
			}
		}
	}
	/* No can do */
	return false;
}

InitPass::InitPass(unsigned int priority)
: Pass(PASS_RUNTIME_INIT_FINIT, priority)
{

}

ConfigPass::ConfigPass(unsigned int priority)
: Pass(PASS_CONFIG, priority)
{

}

RunPass::RunPass(unsigned int priority)
 : Pass(PASS_RUNTIME, priority)
{

}
