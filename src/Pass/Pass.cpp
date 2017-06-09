#include <fvm/Pass.h>

Pass::Pass(enum PassType type, unsigned int priority)
{
	this->type = type;
	this->priority = priority;
}

Pass::~Pass()
{

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