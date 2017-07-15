/*----------------------------------------------------------------------------------------------------------
- FILE NAME: FISCVGAModule.hpp
- SUB MODULE NAME: VGA Module
- PURPOSE: Is responsible for outputting pixel data to a virtual screen
- AUTHOR: MIGUEL SANTOS
-----------------------------------------------------------------------------------------------------------*/

#pragma once
#include "../MoboDevice.h"
#include "../../../CPU/FISCCPUModule.h"
#include <fvm/Debug/Debug.h>

#if _WIN32
#define _TTHREAD_WIN32_
#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"
#include <windows.h>
#include <winbase.h>
#else
#include <SDL.h>
#endif

static mutex glob_iomodule_vga_mutex;

#define IO_VGA_POLLRATE_NS 10000 /* The rate at which the IO device VGA updates the run function, in nanoseconds */

/* Define the size of the address space for this device (in bytes) */
#define IO_VGAMODULE_BANDWIDTH (2)

/* -- Device address allocation --
Address   |  Operation / Meaning
---------------------------------
0         | Enable Device  (0-disable. 1-enable)
1         | Get Status     (returns 1 bits: DeviceEnabled)
*/

enum VGAMODULE_ADDRESS_IOCTL {
	VGAMODULE_ENDEV,
	VGAMODULE_GETSTATUS,
};

#define WINDOW_TITLE  "Virtual Machine - FISC"
#define WINDOW_ICON   "res/fisc_logo.bmp"
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

class VGAModule : public Device {
private:
	SDL_Window   * window = 0;
	SDL_Renderer * renderer;
	SDL_Texture  * texture;

private:
	
	void enable_device()
	{
		SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL, &window, &renderer);
		SDL_SetWindowTitle(window, WINDOW_TITLE);
		SDL_SetWindowIcon(window, SDL_LoadBMP(WINDOW_ICON));

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);
	}

public:
	DEV_CONSTR(VGAModule)
	{
		/* Nothing to construct */
	}

	enum DevRetcode init()
	{
		enum DevRetcode success = DEV_RET_OK;
		/* Nothing to initialise */
		return success;
	}

	enum DevRetcode finit()
	{
		return DEV_RET_OK;
	}

	enum DevRetcode run(runDevLaunchCommandPacket_t * runCmd)
	{
		while (IS_IO_LIVE()) {
#if IO_VGA_POLLRATE_NS > 0
			this_thread::sleep_for(chrono::nanoseconds(IO_VGA_POLLRATE_NS));
#endif

			/* TODO */
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
		switch ((enum VGAMODULE_ADDRESS_IOCTL)address) {
			/* Ignore this request for reading */
		case VGAMODULE_ENDEV:
			break;
			/*****************/
			/* Read requests */
			/*****************/
		case VGAMODULE_GETSTATUS:
			outData = (uint64_t)(((int)isDeviceEnabled));
			break;
		default: /* We never get undefined requests. The IO Module makes sure of that */ break;
		}

		return success;
	}

	enum DevRetcode write(uint64_t data, uint32_t address, enum FISC_DATATYPE dataType, bool debug)
	{
		LOCK(glob_iomodule_vga_mutex);
		
		enum DevRetcode success = DEV_RET_OK;

		/* This device expects to receive the following requests */
		switch ((enum VGAMODULE_ADDRESS_IOCTL)address) {
			/******************/
			/* Write requests */
			/******************/
		case VGAMODULE_ENDEV:
			isDeviceEnabled = data > 0 ? true : false;
			if(isDeviceEnabled)
				enable_device();
			break;
			/* Ignore this request for writing */
		case VGAMODULE_GETSTATUS:
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
NEW_DEVICE(FISC, VGAModule, IO_VGAMODULE_BANDWIDTH);
