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

/* -- Device address allocation --
Address   |  Operation / Meaning
---------------------------------
0         | Enable Device               (1) (0-disable. 1-enable)
1         | Get Status                  (1) (returns 3 bits: isVGAInit | isVGAEnabled | DeviceEnabled
2..3      | Pixel channel: x pos        (4) (sets the x pos variable for accessing the internal pixel information)
4..5      | Pixel channel: y pos        (4) (sets the y pos variable for accessing the internal pixel information)
6         | Pixel channel: access_width (1) (sets the pixel access width to read/write 8/16/32/64 bits on one go from the internal pixel information)
7         | Pixel channel: render       (1) (copies second buffer into the current renderbuffer)
8         | Pixel channel: read         (1) (triggers a read operation on the pixel channel)
9..x      | Pixel channel: data         (x) (sets the pixel data variable for accessing the internal pixel information)
*/

#define PIXEL_CHANNEL_COUNT (200)

enum VGAMODULE_ADDRESS_IOCTL {
	VGAMODULE_ENDEV,
	VGAMODULE_GETSTATUS,
	VGAMODULE_PX_XPOS0, VGAMODULE_PX_XPOS1,
	VGAMODULE_PX_YPOS0, VGAMODULE_PX_YPOS1,
	VGAMODULE_PXACCESSWIDTH,
	VGAMODULE_PXRENDER,
	VGAMODULE_PXRD,
	VGAMODULE_PXDATA,
	VGAMODULE_ADDRESS_IOCTL__COUNT = 9 + (PIXEL_CHANNEL_COUNT * 8)
};

#define IO_VGA_POLLRATE_NS 10000 /* The rate at which the IO device VGA updates the run function, in nanoseconds */

/* Define the size of the address space for this device (in bytes) */
#define IO_VGAMODULE_BANDWIDTH (VGAMODULE_ADDRESS_IOCTL__COUNT)

#define WINDOW_TITLE  "Virtual Machine - FISC"
#define WINDOW_ICON   "res/fisc_logo.bmp"
#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define LINEAR_FRAMEBUFFER_SIZE WINDOW_WIDTH * WINDOW_HEIGHT * 4

class VGAModule : public Device {
private:
	bool vgaRequestInit;
	bool isVGAInit;
	bool isVGAEnabled;
	SDL_Window   * window;
	SDL_Renderer * renderer;
	SDL_Texture  * texture;

	uint8_t renderbuffer[LINEAR_FRAMEBUFFER_SIZE];
	uint8_t doublerenderbuffer[LINEAR_FRAMEBUFFER_SIZE];
	uint8_t * current_renderbuffer;
	uint8_t * other_renderbuffer;
	
	struct {
		uint16_t xpos;
		uint16_t ypos;
		uint8_t  access_width;
		uint8_t  red;
		uint8_t  green;
		uint8_t  blue;
		uint8_t  alpha;
		uint8_t  access_counter;
	} pixel_channel {0, 0, 0, 0, 0, 0, 0, 0};

private:

	bool vga_flip_buffer()
	{
		other_renderbuffer = current_renderbuffer;
		current_renderbuffer = current_renderbuffer == renderbuffer ? doublerenderbuffer : renderbuffer;

		memcpy(other_renderbuffer, current_renderbuffer, LINEAR_FRAMEBUFFER_SIZE);

		return true;
	}
	
	uint64_t vga_read_pixeldata(uint16_t x, uint16_t y, uint8_t access_width)
	{
		uint32_t loc = y * WINDOW_WIDTH * 4 + x * 4;

		if (loc >= LINEAR_FRAMEBUFFER_SIZE) return (uint64_t)-1;

		switch (access_width) {
		case 0:
			return (uint64_t)other_renderbuffer[loc];
		case 1:
			return (uint64_t)(((uint8_t)other_renderbuffer[loc] << 8) |
				   (uint8_t)other_renderbuffer[loc + 1]);
		case 2:
			return (uint64_t)(((uint8_t)(other_renderbuffer[loc]) << 24) |
				   ((uint8_t)(other_renderbuffer[loc + 1]) << 16)        |
				   ((uint8_t)(other_renderbuffer[loc + 2]) << 8)         |
				   (uint8_t)(other_renderbuffer[loc  + 3]));
		case 3:
			return (uint64_t)(((uint64_t)(other_renderbuffer[loc]) << 56) |
				   ((uint64_t)other_renderbuffer[loc + 1] << 48)          |
				   ((uint64_t)other_renderbuffer[loc + 2] << 40)          |
				   ((uint64_t)other_renderbuffer[loc + 3] << 32)          |
				   ((uint64_t)other_renderbuffer[loc + 4] << 24)          |
				   ((uint64_t)other_renderbuffer[loc + 5] << 16)          |
				   ((uint64_t)other_renderbuffer[loc + 6] << 8)           |
				   (uint64_t)other_renderbuffer[loc  + 7]);
		default: return (uint64_t)-1;
		}
		return 0;
	}

	void vga_write_to_buffer_at(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
	{
		uint32_t loc = y * WINDOW_WIDTH * 4 + x * 4;

		other_renderbuffer[loc] = b;
		other_renderbuffer[loc + 1] = g;
		other_renderbuffer[loc + 2] = r;
		other_renderbuffer[loc + 3] = a;
	}

	bool vga_init()
	{
		isVGAEnabled = true;

		/* Initialise SDL: */
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
			DEBUG(DERROR, "(SDL) Could not initialize SDL! SDL Error: %s\n", SDL_GetError());
			return false;
		}

		/* Initialise and create window: */
		window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
		if (window == NULL) {
			DEBUG(DERROR, "(SDL) Window could not be created! SDL Error: %s\n", SDL_GetError());
			SDL_Quit();
			return false;
		}
		SDL_SetWindowIcon(window, SDL_LoadBMP(WINDOW_ICON));

		/* Initialise both render buffers: */
		memset(renderbuffer, 0, LINEAR_FRAMEBUFFER_SIZE);
		memset(doublerenderbuffer, 0, LINEAR_FRAMEBUFFER_SIZE);
		
		/* Create and set renderer */
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);

		/* Create texture that will be constantly rendered */
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, WINDOW_WIDTH, WINDOW_HEIGHT);
		
		/* All done */
		isVGAInit = true;
		return true;
	}

	bool vga_finit()
	{
		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return true;
	}

	void vga_render(void)
	{
		SDL_UpdateTexture(texture, NULL, &current_renderbuffer[0], WINDOW_WIDTH * 4);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	void vga_update(void)
	{
		SDL_Event evt;

		/* We shall now keep rendering and handling events
		   until the user decides to close the screen or the
		   entire system shuts down. */
		while (isVGAEnabled && IS_IO_LIVE())
		{
			while (SDL_PollEvent(&evt)) {
				switch (evt.type) {
				case SDL_QUIT:
					isVGAEnabled = false;
					break;
				}
			}
			
			vga_render();
#if IO_VGA_POLLRATE_NS > 0
			this_thread::sleep_for(chrono::nanoseconds(IO_VGA_POLLRATE_NS));
#endif
		}
	}

public:
	DEV_CONSTR(VGAModule)
	{
		/* Nothing to construct */
	}

	enum DevRetcode init()
	{
		enum DevRetcode success = DEV_RET_OK;
		vgaRequestInit = false;
		isVGAInit = false;
		isVGAEnabled = false;
		window = nullptr;
		renderer = nullptr;
		texture = nullptr;
		current_renderbuffer = &renderbuffer[0];
		other_renderbuffer = &doublerenderbuffer[0];
		return success;
	}

	enum DevRetcode finit()
	{
		/* VGA was already cleaned up as soon as the run function ended its execution */
		return DEV_RET_OK;
	}

	enum DevRetcode run(runDevLaunchCommandPacket_t * runCmd)
	{
		while (IS_IO_LIVE()) {
			
			/* Do nothing while the VGA device is disabled */

#if IO_VGA_POLLRATE_NS > 0
			this_thread::sleep_for(chrono::nanoseconds(IO_VGA_POLLRATE_NS));
#endif

			if(vgaRequestInit && !isVGAEnabled) {
				vgaRequestInit = false;
				/* CPU requested the initialization of the VGA device */
				vga_init();
				/* Now keep updating it */
				vga_update();
				/* If we get here then the screen was closed / the IO Controller has finished execution */
				vga_finit();
				break;
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
		switch ((enum VGAMODULE_ADDRESS_IOCTL)address) {
			/* Ignore these requests for reading */
		case VGAMODULE_ENDEV:
		case VGAMODULE_PX_XPOS0: case VGAMODULE_PX_XPOS1:
		case VGAMODULE_PX_YPOS0: case VGAMODULE_PX_YPOS1:
		case VGAMODULE_PXACCESSWIDTH:
		case VGAMODULE_PXRENDER:
			break;
			/*****************/
			/* Read requests */
			/*****************/
		case VGAMODULE_GETSTATUS:
			outData = (uint64_t)((((int)isVGAInit) << 2) | (((int)isVGAEnabled) << 1) | ((int)isDeviceEnabled));
			break;
		case VGAMODULE_PXRD:
			outData = vga_read_pixeldata(pixel_channel.xpos, pixel_channel.ypos, pixel_channel.access_width);
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
			if(isDeviceEnabled && !isVGAInit && !vgaRequestInit)
				vgaRequestInit = true;
			break;
		case VGAMODULE_PX_XPOS0:
			pixel_channel.xpos = (uint16_t)data;
			break;
		case VGAMODULE_PX_XPOS1:
			pixel_channel.xpos |= ((uint8_t)data) << 8;
			break;
		case VGAMODULE_PX_YPOS0:
			pixel_channel.ypos = (uint16_t)data;
			break;
		case VGAMODULE_PX_YPOS1:
			pixel_channel.ypos |= ((uint8_t)data) << 8;
			break;
		case VGAMODULE_PXACCESSWIDTH:
			pixel_channel.access_width = (uint8_t)data;
			break;
		case VGAMODULE_PXRENDER:
			if (data && !vga_flip_buffer())
				success = DEV_RET_ERROR;
			break;
			/* Ignore these requests for writing */
		case VGAMODULE_GETSTATUS:
		case VGAMODULE_PXRD:
			break;
		default: {
				/* Handle the pixel writing operation */
				if (address >= VGAMODULE_PXDATA) {
					address = ((address - VGAMODULE_PXDATA) / 8) * 2;
					
					vga_write_to_buffer_at(pixel_channel.xpos + address, pixel_channel.ypos, (uint8_t)(data & 0xFF), (uint8_t)((data & 0xFF00) >> 8), (uint8_t)((data & 0xFF0000) >> 16), (uint8_t)((data & 0xFF000000) >> 24));
					data >>= 32;

					if(pixel_channel.xpos + address + 1 >= WINDOW_WIDTH)
						vga_write_to_buffer_at(0, pixel_channel.ypos + 1, (uint8_t)(data & 0xFF), (uint8_t)((data & 0xFF00) >> 8), (uint8_t)((data & 0xFF0000) >> 16), (uint8_t)((data & 0xFF000000) >> 24));
					else
						vga_write_to_buffer_at(pixel_channel.xpos + address + 1, pixel_channel.ypos, (uint8_t)(data & 0xFF), (uint8_t)((data & 0xFF00) >> 8), (uint8_t)((data & 0xFF0000) >> 16), (uint8_t)((data & 0xFF000000) >> 24));
				}
							
				/* We never get undefined requests. The IO Module makes sure of that */
				break;
			}
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
