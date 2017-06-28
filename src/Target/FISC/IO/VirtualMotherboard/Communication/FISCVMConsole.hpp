/*----------------------------------------------------------------------------------------------------------
- FILE NAME: FISCVMConsole.hpp
- SUB MODULE NAME: Machine Console
- PURPOSE: Uses the Machine's already opened command line as a standard serial communication terminal
- AUTHOR: MIGUEL SANTOS
-----------------------------------------------------------------------------------------------------------*/

#pragma once
#include "../MoboDevice.h"
#include <fvm/Debug/Debug.h>
#include <vector>
#include <conio.h>

static mutex glob_iomodule_vmconsole_mutex;

#define IO_VMCONSOLE_MAX_STDOUT_FIFOBUFFER_SIZE 8192 /* Maximum amount of characters the stdout buffer can hold */
#define IO_VMCONSOLE_MAX_STDIN_FIFOBUFFER_SIZE  512  /* Maximum amount of characters the stdin buffer can hold  */

/* Define the size of the address space for this device (in bytes) */
#define IO_VMCONSOLE_BANDWIDTH (8)

/* -- Device address allocation --
  Address   |  Operation / Meaning 
  ---------------------------------
  0         | Enable Device (0-disable. 1-enable)
  1         | Enable stdout (0-disable. 1-enable)
  2         | Enable stdin  (0-disable. 1-enable)
  3         | Get status    (returns 3 bits: StdinEnabled | StdoutEnabled | DeviceEnabled)
  4         | Write byte to stdout
  5         | Is write buffer (filled by CPU) ready to be outputted into stdout
  6         | Read byte from stdin
  7         | Is read buffer (filled by stdin) ready to be read by the CPU  
*/

enum VMCONSOLE_ADDRESS_IOCTL {
	VMCONSOLE_ENDEV,
	VMCONSOLE_ENSTDOUT,
	VMCONSOLE_ENSTDIN,
	VMCONSOLE_GETSTATUS,
	VMCONSOLE_WR,
	VMCONSOLE_WRRDY,
	VMCONSOLE_RD,
	VMCONSOLE_RDRDY,
};

class VMConsole : public Device {
private:
	bool isStdoutEnabled; 
	bool isStdinEnabled;
	bool isWrBufferReady;
	bool isRdBufferReady;
	std::vector<char> stdoutFIFOBuffer;
	std::vector<char> stdinFIFOBuffer;
	std::unique_ptr<thread> stdinReaderThread;

	enum DevRetcode stdoutWrite(char byte)
	{
		/* Just ignore this request if the device / operation is disabled */
		if(!isDeviceEnabled || !isStdoutEnabled)
			return DEV_RET_OK;

		/* Push this byte into an async FIFO buffer and keep flushing that
		   buffer into stdout using the run() method instead of outputting it here */

		if(stdoutFIFOBuffer.size() > IO_VMCONSOLE_MAX_STDOUT_FIFOBUFFER_SIZE)
			return DEV_RET_OK; /* Just return OK. It's the implementation's responsability to check if the buffer is full, not the VM's */
	
		isWrBufferReady = false;
		stdoutFIFOBuffer.push_back(byte);
		return DEV_RET_OK;
	}

	enum DevRetcode  stdinPush(int ch)
	{
		/* Just ignore this request if the device / operation is disabled */
		if (!isDeviceEnabled || !isStdinEnabled) return DEV_RET_OK;
		/* Buffer has contents. Set read flag to ready */
		isRdBufferReady = true;
		stdinFIFOBuffer.push_back((char)ch);

		return DEV_RET_OK;
	}

	enum DevRetcode stdoutRead(uint64_t & outData)
	{
		LOCK(glob_iomodule_vmconsole_mutex);
		/* Just ignore this request if the device / operation is disabled */
		if(!isDeviceEnabled || !isStdinEnabled)
			return DEV_RET_OK;

		if(!stdinFIFOBuffer.empty()) {
			outData = (char)stdinFIFOBuffer.front();
			stdinFIFOBuffer.erase(stdoutFIFOBuffer.begin());
		}
		
		isRdBufferReady = !stdinFIFOBuffer.empty();

		return DEV_RET_OK;
	}

public:
	DEV_CONSTR(VMConsole)
	{
		/* Nothing to construct */
	}

	enum DevRetcode init()
	{
		isStdoutEnabled = false;
		isStdinEnabled  = false;
		isWrBufferReady = false;
		isRdBufferReady = false;
		return DEV_RET_OK;
	}

	enum DevRetcode finit()
	{
		return DEV_RET_OK;
	}

	enum DevRetcode run(runDevLaunchCommandPacket_t * runCmd)
	{
		while (IS_IO_LIVE()) {
			LOCK(glob_iomodule_vmconsole_mutex);

			/* We'll need to flush the write FIFO buffer into stdout here.
			   Meanwhile, if there is no text to output, stay idle (until the IO Module closes) */

			if(!stdoutFIFOBuffer.empty()) {
				/* We've got some text to output */
				putc(stdoutFIFOBuffer.front(), stdout);
				/* Pop the front of the buffer */
				stdoutFIFOBuffer.erase(stdoutFIFOBuffer.begin());
			}
			else {
				isWrBufferReady = true;
			}

			/* Push keyboard hits into the stdin buffer (TODO: I know that this is not platform portable... this is temporary) */
			if(_kbhit())
				stdinPush(_getch());
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
		switch ((enum VMCONSOLE_ADDRESS_IOCTL)address) {
		/* Ignore these requests for reading */
		case VMCONSOLE_ENDEV:
		case VMCONSOLE_ENSTDOUT:
		case VMCONSOLE_ENSTDIN:
		case VMCONSOLE_WR:
			break;
		/*****************/
		/* Read requests */
		/*****************/
		case VMCONSOLE_GETSTATUS:
			outData = (uint64_t)((((int)isStdinEnabled) << 2) | (((int)isStdoutEnabled) << 1) | ((int)isDeviceEnabled));
			break;
		case VMCONSOLE_WRRDY:
			if(!isDeviceEnabled) break; /* Ignore request if device disabled */
			outData = (uint64_t)(isWrBufferReady);
			break;
		case VMCONSOLE_RD:
			success = stdoutRead(outData);
			break;
		case VMCONSOLE_RDRDY:
			if(!isDeviceEnabled) break; /* Ignore request if device disabled */
			outData = (uint64_t)(isRdBufferReady);
			break;
		default: /* We never get undefined requests. The IO Module makes sure of that */ break;
		}

		return success;
	}

	enum DevRetcode write(uint64_t data, uint32_t address, enum FISC_DATATYPE dataType, bool debug)
	{
		LOCK(glob_iomodule_vmconsole_mutex);

		enum DevRetcode success = DEV_RET_OK;

		/* This device expects to receive the following requests */
		switch ((enum VMCONSOLE_ADDRESS_IOCTL)address) {
		/******************/
		/* Write requests */
		/******************/
		case VMCONSOLE_ENDEV:
			isDeviceEnabled = data > 0 ? true : false;
			break;
		case VMCONSOLE_ENSTDOUT:
			if(!isDeviceEnabled) break; /* Ignore request if device disabled */
			isStdoutEnabled = data > 0 ? true : false;
			break;
		case VMCONSOLE_ENSTDIN:
			if(!isDeviceEnabled) break; /* Ignore request if device disabled */
			isStdinEnabled = data > 0 ? true : false;
			break;
		case VMCONSOLE_WR:
			return stdoutWrite((char)data);
		/* Ignore these requests for writing */
		case VMCONSOLE_GETSTATUS:
		case VMCONSOLE_WRRDY:
		case VMCONSOLE_RD:
		case VMCONSOLE_RDRDY:
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
NEW_DEVICE(FISC, VMConsole, IO_VMCONSOLE_BANDWIDTH);
