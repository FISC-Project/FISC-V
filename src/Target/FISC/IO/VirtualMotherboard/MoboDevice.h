#ifndef MOBODEVICE_H_
#define MOBODEVICE_H_

#include <string>
#include <stdint.h>
#include <memory>
#include <fvm/Utils/String.h>
#include <TinyThread++-1.1/tinythread.h>

using namespace tthread;

/* Forward decl */
class IOMachineModule;

/****************************/
/* Device class declaration */
/****************************/

enum DevRetcode {
	DEV_RET_NULL,
	DEV_RET_OK,
	DEV_RET_NOTHINGTODO,
	DEV_RET_WARNING,
	DEV_RET_ERROR,
	DEV_RET_FATAL,
	DEV_RET__COUNT
};

/* Forward decl */
class Device;
class IOMachineModule;

extern Device ** device_list_realloc;
extern unsigned int device_list_size;

typedef struct {
	Device * theRunningDevice;
	enum DevRetcode retval;
	bool hasReturned;
} runDevLaunchCommandPacket_t;

class Device {
public:
	Device(std::string deviceName, const uint32_t addressSpaceSize)
	: addressSpaceSize(addressSpaceSize), uniqueID(device_list_size)
	{
		this->deviceName = deviceName;
		initialized = true;
		
		/* Add this device to a temporary array
		   to indicate the configurator that this
		   device instantiation exists */

		device_list_size++;
		device_list_realloc = (Device**)realloc(device_list_realloc, device_list_size * sizeof(Device*));
		device_list_realloc[device_list_size - 1] = this;
	}

	~Device()
	{

	}

private:
	/* These are private methods handled by the IO Module */
	virtual enum DevRetcode init() = 0;
	virtual enum DevRetcode finit() = 0;
	virtual enum DevRetcode poll() = 0;
	virtual enum DevRetcode watchdog() = 0;

	friend class IOMachineModule;
	friend class IOMachineConfigurator;

public:
	/* Run needs to be public because it is called by a global static function
	   and it needs access to this method. */
	virtual enum DevRetcode run(runDevLaunchCommandPacket_t * runCmd) = 0;

	/* These are public methods called by other passes */
	virtual enum DevRetcode read(uint32_t address, enum FISC_DATATYPE dataType, bool debug) = 0;
	virtual enum DevRetcode write(uint64_t data, uint32_t address, enum FISC_DATATYPE dataType, bool debug) = 0;
	virtual enum DevRetcode ioctl(void * ioctlPacket) = 0;

	std::string deviceName;
	std::string targetName;

	IOMachineModule * ioContext;
	#define IS_IO_LIVE() ioContext->isLive()

private:
	const uint32_t addressSpaceSize;
	const uint16_t uniqueID;
	bool initialized;
	std::unique_ptr<thread> runThread;
	runDevLaunchCommandPacket_t runCmd;
	bool alreadyJoined;
};

#define NEW_DEVICE(targetname, devicename, addrspacesize) static devicename targetname ## _iodev_ ## devicename(STRING(devicename), addrspacesize)
#define DEV_CONSTR(devicename) devicename(std::string devName, uint32_t addressSpaceSize) : Device(devName, addressSpaceSize)

#endif