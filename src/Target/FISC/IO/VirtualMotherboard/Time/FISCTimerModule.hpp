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

static mutex glob_iomodule_timer_mutex;

/* -- Device address allocation --
Address   |  Operation / Meaning
---------------------------------
0         | Enable Device          (1) (0-disable. 1-enable)
1         | Enable timer           (1) (0-disable. 1-enable)
2..5      | Set timer period in ns (4)
6         | Get Status             (1) (returns 2 bits: TimerEnabled | DeviceEnabled)
*/

enum TIMERMODULE_ADDRESS_IOCTL {
	TIMERMODULE_ENDEV,
	TIMERMODULE_ENTIMER,
	TIMERMODULE_SETPERIOD0, TIMERMODULE_SETPERIOD1, TIMERMODULE_SETPERIOD2, TIMERMODULE_SETPERIOD3,
	TIMERMODULE_GETSTATUS,
	TIMERMODULE_ADDRESS_IOCTL__COUNT
};

/* Define the size of the address space for this device (in bytes) */
#define IO_TIMERMODULE_BANDWIDTH (TIMERMODULE_ADDRESS_IOCTL__COUNT)

#define IO_TIMERMODULE_ENABLE_SLEEPING (0) /* Do we even allow the timer to sleep its thread (0-No. 1-Yes) */

class TimerModule : public Device {
private:
	CPUModule * cpu;
	bool isTimerEnabled;

	#define TIMER_INTCODE                      0        /* The interrupt code of the timer seen by the CPU. This represents the interrupt offset on the interrupt vector for the timer */
	#define DEFAULT_TIMER_SLEEPTIME_NS         100000   /* Default sleep time for the timer (in nanosecond scale)                                                                      */
	#define DEFAULT_TIMER_QUANTA_SLEEPTIME_NS  10000    /* The smallest time atomic scale each sleep trigger sleeps for                                                                */
	#define DEFAULT_TIMER_MINIMUM_SLEEPTIME_NS 100000   /* The minimum value the variable timerSleeptime can hold                                                                      */
	#define DEFAULT_TIMER_MAXIMUM_SLEEPTIME_NS 10000000 /* The maximum value the variable timerSleeptime can hold                                                                      */
	uint64_t timerSleeptime; /* The period of the timer's interrupt                                                                                                                 */
	uint64_t sleepCounter;   /* The counter which counts up and triggers the CPU interrupt whenever it reaches the value of timerSleeptime, resetting back to 0 after the match     */

	void timerSleep()
	{
		this_thread::sleep_for(chrono::nanoseconds(DEFAULT_TIMER_QUANTA_SLEEPTIME_NS));
	}

public:
	DEV_CONSTR(TimerModule)
	{
		/* Nothing to construct */
	}

	enum DevRetcode init()
	{
		enum DevRetcode success = DEV_RET_OK;

		isTimerEnabled = false;
		timerSleeptime = DEFAULT_TIMER_SLEEPTIME_NS;
		sleepCounter = 0;
		
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
		/* Keep triggering the CPU while the following conditions are true:
		   1- The IO Module is alive 
		   2- The Timer device is enabled
		   3- The Timer interrupts are enabled
		*/
		
		while (IS_IO_LIVE()) {
			if(isDeviceEnabled && isTimerEnabled) {
#if IO_TIMERMODULE_ENABLE_SLEEPING > 0
				timerSleep();
#endif
				if (++sleepCounter >= timerSleeptime) {
					/* Trigger CPU hardware interrupt */
					cpu->triggerHardInterrupt(TIMER_INTCODE);
					sleepCounter = 0;
				}
			}
			else {
				sleepCounter = 0;
			}
		}
		return DEV_RET_OK;
	}

	enum DevRetcode poll()
	{
		/* Nothing to poll */
		return DEV_RET_OK;
	}

	enum DevRetcode read(uint64_t & outData, uint32_t address, enum FISC_DATATYPE dataType, bool debug)
	{
		enum DevRetcode success = DEV_RET_OK;

		/* This device expects to receive the following requests */
		switch ((enum TIMERMODULE_ADDRESS_IOCTL)address) {
		/* Ignore this request for reading */
		case TIMERMODULE_ENDEV:
		case TIMERMODULE_ENTIMER:
		case TIMERMODULE_SETPERIOD0: case TIMERMODULE_SETPERIOD1: case TIMERMODULE_SETPERIOD2: case TIMERMODULE_SETPERIOD3:
			break;
		/*****************/
		/* Read requests */
		/*****************/
		case TIMERMODULE_GETSTATUS:
			outData = (uint64_t)((((int)isTimerEnabled) << 1) | ((int)isDeviceEnabled));
			break;
		default: /* We never get undefined requests. The IO Module makes sure of that */ break;
		}

		return success;
	}

	enum DevRetcode write(uint64_t data, uint32_t address, enum FISC_DATATYPE dataType, bool debug)
	{
		LOCK(glob_iomodule_timer_mutex);

		enum DevRetcode success = DEV_RET_OK;
		
		/* This device expects to receive the following requests */
		switch ((enum TIMERMODULE_ADDRESS_IOCTL)address) {
		/******************/
		/* Write requests */
		/******************/
		case TIMERMODULE_ENDEV:
			isDeviceEnabled = data > 0 ? true : false;
			break;
		case TIMERMODULE_ENTIMER:
			if(!isDeviceEnabled) break; /* Ignore request if device disabled */
			isTimerEnabled = data > 0 ? true : false;
			break;
		case TIMERMODULE_SETPERIOD0: case TIMERMODULE_SETPERIOD1: case TIMERMODULE_SETPERIOD2: case TIMERMODULE_SETPERIOD3:
			if(!isDeviceEnabled) break; /* Ignore request if device disabled */
			sleepCounter = 0;
			timerSleeptime = (uint64_t)(data & 0xFFFFFFFF); /* We only really care about 32 bits of this value (for now) */

			/* Cap the sleeptime value */
			if(timerSleeptime < DEFAULT_TIMER_MINIMUM_SLEEPTIME_NS)
				timerSleeptime = DEFAULT_TIMER_MINIMUM_SLEEPTIME_NS;

			if(timerSleeptime > DEFAULT_TIMER_MAXIMUM_SLEEPTIME_NS)
				timerSleeptime = DEFAULT_TIMER_MAXIMUM_SLEEPTIME_NS;

			break;
		/* Ignore this request for writing */
		case TIMERMODULE_GETSTATUS:
			break;
		default: /* We never get undefined requests. The IO Module makes sure of that */ break;
		}

		return success;
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
