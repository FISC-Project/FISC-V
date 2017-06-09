#include <fvm/Pass.h>

Pass::Pass(enum PassType type, unsigned int priority)
{
	this->type = type;
	this->priority = priority;
}

Pass::~Pass()
{

}
