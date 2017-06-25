/*----------------------------------------------------------------------------------------------------------
- FILE NAME: FISCVMConsole.hpp
- SUB MODULE NAME: Machine Console
- PURPOSE: Uses the Machine's already opened command line as a standard serial communication terminal
- AUTHOR: MIGUEL SANTOS
-----------------------------------------------------------------------------------------------------------*/

#pragma once
#include "../MoboDevice.h"

class VMConsole : public Device {
public:
	DEV_CONSTR(VMConsole)
	{
		/* Nothing to construct */
	}

	enum DevRetcode init()
	{
		return DEV_RET_OK;
	}

	enum DevRetcode finit()
	{
		return DEV_RET_OK;
	}

	enum DevRetcode run()
	{
		/* Nothing to run */
		return DEV_RET_OK;
	}

	enum DevRetcode poll()
	{
		/* Nothing to poll */
		return DEV_RET_OK;
	}

	enum DevRetcode read()
	{
		/* TODO: The main memory calls this */
		return DEV_RET_OK;
	}

	enum DevRetcode write()
	{
		/* TODO: The main memory calls this */
		return DEV_RET_OK;
	}

	enum DevRetcode ioctl()
	{
		/* Nothing to control */
		return DEV_RET_OK;
	}

	enum DevRetcode watchdog()
	{
		return DEV_RET_OK;
	}
};

/* Register / instantiate device */
NEW_DEVICE(FISC, VMConsole);
