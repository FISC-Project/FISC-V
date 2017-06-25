#ifndef MOBODEVICE_H_
#define MOBODEVICE_H_

#include <string>
#include <fvm/Utils/String.h>

enum DevRetcode {
	DEV_RET_NULL,
	DEV_RET_OK,
	DEV_RET_WARNING,
	DEV_RET_ERROR,
	DEV_RET_FATAL,
	DEV_RET__COUNT
};

/****************************/
/* Device class declaration */
/****************************/

/* Forward decl */
class Device;

extern Device ** device_list_realloc;
extern unsigned int device_list_size;

class Device {
public:
	Device(std::string deviceName)
	{
		this->deviceName = deviceName;
		uniqueID = device_list_size;

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

	virtual enum DevRetcode init() = 0;
	virtual enum DevRetcode finit() = 0;
	virtual enum DevRetcode run() = 0;
	virtual enum DevRetcode poll() = 0;
	virtual enum DevRetcode read() = 0;
	virtual enum DevRetcode write() = 0;
	virtual enum DevRetcode ioctl() = 0;
	virtual enum DevRetcode watchdog() = 0;

	std::string deviceName;
	std::string targetName;
	uint16_t uniqueID;
	bool initialized;
};

#define NEW_DEVICE(targetname, devicename) static devicename targetname ## _iodev_ ## devicename(STRING(devicename))
#define DEV_CONSTR(devicename) devicename(std::string devName) : Device(devName)

#endif