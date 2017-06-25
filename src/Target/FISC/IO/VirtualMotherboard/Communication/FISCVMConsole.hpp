/*----------------------------------------------------------------------------------------------------------
- FILE NAME: FISCVMConsole.hpp
- SUB MODULE NAME: Machine Console
- PURPOSE: Uses the Machine's already opened command line as a standard serial communication terminal
- AUTHOR: MIGUEL SANTOS
-----------------------------------------------------------------------------------------------------------*/

#pragma once
#include "../MoboDevice.h"
#include <fvm/Debug/Debug.h>

/* Define the size of the address space for this device (in bytes) */
#define IO_VMCONSOLE_BANDWIDTH (2) /* 2 bytes are enough. 1 for writing serially and another for reading serially */

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

	enum DevRetcode run(runDevLaunchCommandPacket_t * runCmd)
	{
		while (IS_IO_LIVE()); /* Stay idle while the IO Module is live */	
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
		/* This device expects to receive 1 single byte at address 0 */
		char fmt[2];
		sprintf(fmt, "%c", (char)(data & 0xFF));
		raw_print(fmt, nullptr, false);
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
NEW_DEVICE(FISC, VMConsole, IO_VMCONSOLE_BANDWIDTH);
