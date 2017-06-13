#ifndef FISC_BRANCH_H_
#define FISC_BRANCH_H_

#include <fvm/Utils/String.h>
#include "FISCISA.h"

/**************************************************************/
/******************** B AND BL INSTRUCTIONS *******************/
/**************************************************************/
NEW_INSTRUCTION(FISC, B, BF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, BL, BF,
{
	return FISC_RET_OK;
});

/**************************************************************/
/*********************** BR INSTRUCTION ***********************/
/**************************************************************/
NEW_INSTRUCTION(FISC, BR, RF,
{
	return FISC_RET_OK;
});

/**************************************************************/
/************** CONDITIONAL BRANCH INSTRUCTIONS ***************/
/**************************************************************/
NEW_INSTRUCTION(FISC, BCOND, CBF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, CBNZ, CBF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, CBZ, CBF,
{
	return FISC_RET_OK;
});

#endif
