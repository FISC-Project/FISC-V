#ifndef TargetRegistry_H_
#define TargetRegistry_H_

#include <vector>
#include <string>
#include <fvm/Utils/String.h>
#include <fvm/Utils/Macro.h>

#define DECL_OBJ(prefix, classname) static classname prefix ## classname;
#define DECL_OBJNAME_ADDR(prefix, objname) &prefix ## objname,
#define SETOWNOBJNAME(prefix, objname) prefix ## objname.passName = STRING(objname); prefix ## objname.passNameLong = STRING(prefix ## objname);

#define REGISTER_TARGET(Targetname, ...)\
\
FOR_EACH_ARG(DECL_OBJ, Targetname, __VA_ARGS__)\
\
class Targetname ## Target : public TargetRegistry { \
public:\
	Targetname ## Target() : TargetRegistry(STRING(Targetname), { FOR_EACH_ARG(DECL_OBJNAME_ADDR, Targetname, __VA_ARGS__) }) {\
	FOR_EACH_ARG(SETOWNOBJNAME, Targetname, __VA_ARGS__)\
	}\
	~Targetname ## Target() {}\
};\
static Targetname ## Target The ## Targetname ## Target;\

class Pass;

class TargetRegistry {
public:
	static std::vector<TargetRegistry*> TheTargetList;

	TargetRegistry(std::string targetName, std::vector<Pass*> passList);
	~TargetRegistry();

	static void launchTarget(std::string targetName);
	static void launchTarget(unsigned int targetIndex);

	Pass * getPass(Pass * passID, std::string passName);
	Pass * getPass(Pass * passID, unsigned int passIndex);
	
	void run();

private:
	std::string targetName;
	std::vector<Pass*> passList;
};

#endif
