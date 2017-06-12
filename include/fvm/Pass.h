#ifndef PASS_H_
#define PASS_H_

#include <fvm/TargetRegistry.h>
#include <string>

enum PassType {
	PASS_NULL,
	PASS_RUNTIME_INIT_FINIT,
	PASS_CONFIG,
	PASS_RUNTIME,
	PASS_COUNT
};

enum PassRetcode {
	PASS_RET_NULL,
	PASS_RET_OK,
	PASS_RET_ERR,
	PASS_RET_COUNT
};

typedef struct {
	std::string name;
	void * ptr;
	void * ptr_saved;
	bool isFn;
	bool locked;
} passResource_t;

class Pass {
public:
	Pass(enum PassType type, unsigned int priority);
	virtual ~Pass() = 0;
	virtual enum PassRetcode init() = 0;
	virtual enum PassRetcode finit() = 0;
	virtual enum PassRetcode run() = 0;
	virtual enum PassRetcode watchdog() = 0;

	enum PassType type;
	std::string passName;
	std::string passNameLong;
	unsigned int priority;

	bool setParentTargetContext(TargetRegistry * parentTarget);
	TargetRegistry * getTarget();

	#define GET_PASS(classname) dynamic_cast<classname*>(getPass(STRING(classname)))
	Pass * getPass(std::string passName);
	Pass * getPass(unsigned int passIndex);

	#define DECL_RES(res) STRING(res),
	#define DECL_WHITELIST(modulename, ...) {STRING(modulename), {FOR_EACH(DECL_RES, __VA_ARGS__)}},
	#define DECL_WHITELIST_ALL(modulename) {STRING(modulename), {}},
	bool shareResource(Pass * passID, void * genericPtr, std::string resourceName);
	bool shareResource(Pass * passID, std::string resourceName);
	void * switchResourceFn(Pass * passID, std::string resourceName);
	bool releaseResource(Pass * passID, std::string resourceName);
	void * getResource(Pass * passID, std::string resourceName);
	void * getResourceFn(Pass * passID, std::string resourceName);
	bool lockResource(Pass * passID, std::string resourceName);
	bool unlockResource(Pass * passID, std::string resourceName);
	bool lockAllResources(Pass * passID);
	bool unlockAllResources(Pass * passID);

	bool changeDebugLevel(enum DEBUG_TYPE type, enum DEBUG_LEVEL newLevel);
	bool changeDebugLevel(enum DEBUG_LEVEL newLevel);
	bool DEBUG(enum DEBUG_TYPE type, std::string fmt, ...);
	bool DEBUG(std::string fmt, ...);

	std::vector<std::pair<std::string, std::vector<std::string> > > whitelist;
	void setWhitelist(std::vector<std::pair<std::string, std::vector<std::string> > > whitelist);

	bool verifyPassID(Pass * passID);

private:
	bool switchResourceFnProtect(Pass * passID, std::string resourceName);
	bool verifyPassID(Pass * passID, std::string resourceName);

	TargetRegistry * parentTarget;
	std::vector<passResource_t> resourceList;
	bool isTargetSet;
	bool resourcesLocked;
};

class InitPass : public Pass {
public:
	InitPass(unsigned int priority);
};

class ConfigPass : public Pass {
public:
	ConfigPass(unsigned int priority);
};

class RunPass : public Pass {
public:
	RunPass(unsigned int priority);
};

#endif
