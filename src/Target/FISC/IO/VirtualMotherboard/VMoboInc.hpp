/*------------------------------------
  INCLUDE ALL MOTHERBOARD DEVICES HERE
--------------------------------------*/

/*
	Device 1 -
		Virtual Machine Console
	Description -
		Uses the Virtual Machine's console output as a standard serial communication terminal

	Device 2 -
		Timer
	Description -
		Keeps triggering the CPU interrupt wire

	Device 3 -
		VGA
	Description -
		Displays pixel data to the screen

	Device 4 -
		Keyboard
	Description -
		Reads user input through a virtual keyboard that is associated with the VGA display device

	Device 5 -
		Mouse
	Description -
		Reads user input through a virtual mouse that is associated with the VGA display device
*/

#include "Communication/FISCVMConsole.hpp"
#include "Time/FISCTimerModule.hpp"
#include "Video/FISCVGAModule.hpp"
#include "Input/FISCKeyboardModule.hpp"
#include "Input/FISCMouseModule.hpp"
