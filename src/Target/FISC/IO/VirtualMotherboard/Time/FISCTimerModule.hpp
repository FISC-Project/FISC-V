/*----------------------------------------------------------------------------------------------------------
- FILE NAME: FISCTimerModule.hpp
- SUB MODULE NAME: System Timer Module
- PURPOSE: Is responsible for triggering a consistent square wave like interrupt on the CPU
- AUTHOR: MIGUEL SANTOS
-----------------------------------------------------------------------------------------------------------*/

#pragma once
#include "../MoboDevice.h"
#include "../../../CPU/FISCCPUModule.h"
#include <fvm/Debug/Debug.h>

/* Define the size of the address space for this device (in bytes) */
#define IO_TIMERMODULE_BANDWIDTH (1)

class TimerModule : public Device {
private:
	CPUModule * cpu;

public:
	DEV_CONSTR(TimerModule)
	{
		/* Nothing to construct */
	}

	enum DevRetcode init()
	{
		enum DevRetcode success = DEV_RET_OK;
		if (!(cpu = dynamic_cast<CPUModule*>(ioContext->getPass("CPUModule")))) {
			/* We were unable to find a CPUModule pass!
			   We cannot continue the execution of this device */
			ioContext->DEBUG(DERROR, "Could not fetch the CPU Module Pass at target %s@%s@%s@%s", targetName.c_str(), ioContext->passName.c_str(), deviceName.c_str(), __func__);
			success = DEV_RET_ERROR;
		}

		return success;
	}

	enum DevRetcode finit()
	{
		return DEV_RET_OK;
	}

	enum DevRetcode run(runDevLaunchCommandPacket_t * runCmd)
	{
		#define MAX_PRELOAD_VALUE 1000
		uint64_t counter = 0;

		while (IS_IO_LIVE()) {
			this_thread::sleep_for(chrono::nanoseconds(1));
			if (++counter >= MAX_PRELOAD_VALUE) {
				cpu->triggerHardInterrupt(0);
				counter = 0;
			}
		}
		return DEV_RET_OK;
	}

	enum DevRetcode poll()
	{
		/* Nothing to poll */
		return DEV_RET_OK;
	}

	enum DevRetcode read(uint32_t address, enum FISC_DATATYPE dataType, bool debug)
	{

		return DEV_RET_OK;
	}

	enum DevRetcode write(uint64_t data, uint32_t address, enum FISC_DATATYPE dataType, bool debug)
	{
		return DEV_RET_OK;
	}

	enum DevRetcode ioctl(void * ioctlPacket)
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
NEW_DEVICE(FISC, TimerModule, IO_TIMERMODULE_BANDWIDTH);
