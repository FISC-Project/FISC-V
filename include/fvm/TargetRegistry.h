#ifndef TargetRegistry_H_
#define TargetRegistry_H_

#include <fvm/Runtime.h>
#include <vector>
#include <string>
#include <fvm/Utils/String.h>
#include <fvm/Utils/Macro.h>
#include <fvm/Debug/Debug.h>

using namespace debug;

#define DECL_OBJ(prefix, classname) static classname prefix ## classname;
#define DECL_OBJNAME_ADDR(prefix, objname) &prefix ## objname,
#define SETOWNOBJNAME(prefix, objname) prefix ## objname.passName = STRING(objname); prefix ## objname.passNameLong = STRING(prefix ## objname);

#define REGISTER_TARGET(Targetname, TargetnameLong, TargetOwnerDescription, ...)\
namespace Targetname {\
FOR_EACH_ARG(DECL_OBJ, Targetname, __VA_ARGS__)\
\
class Targetname ## Target : public TargetRegistry { \
public:\
	Targetname ## Target() : TargetRegistry(STRING(Targetname), STRING(TargetnameLong), STRING(TargetOwnerDescription), { FOR_EACH_ARG(DECL_OBJNAME_ADDR, Targetname, __VA_ARGS__) }) {\
	FOR_EACH_ARG(SETOWNOBJNAME, Targetname, __VA_ARGS__)\
	}\
	~Targetname ## Target() {}\
};\
static Targetname ## Target The ## Targetname ## Target;\
}\

class Pass;

class TargetRegistry {
public:
	static std::vector<TargetRegistry*> TheTargetList;
	std::string targetName;
	std::string targetNameLong;
	std::string targetOwnerDescription;

	TargetRegistry(std::string targetName, 
	               std::string targetNameLong, 
				   std::string targetOwnerDescription, 
				   std::vector<Pass*> passList);
	~TargetRegistry();

	Pass * getPass(Pass * passID, std::string passName);
	Pass * getPass(Pass * passID, unsigned int passIndex);

	enum PassStatus getPassStatus(Pass * passID, std::string passName);
	enum PassStatus getPassStatus(Pass * passID, unsigned int passIndex);

private:
	Runtime runContext;
	std::vector<Pass*> passList;

	friend class Runtime;
};

#endif
