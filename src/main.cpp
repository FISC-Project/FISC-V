/*
 * main.cpp
 *
 *  Created on: 25/11/2016
 *      Author: Miguel
 */

#include "fiscv.h"

int main(int argc, char ** argv) {
	/* Instantiate Virtual Machine: */
	FISCV vmachine("./prog.bin");
	/* Run the Virtual Machine: */
	vmachine.run();
	return 0;
}
