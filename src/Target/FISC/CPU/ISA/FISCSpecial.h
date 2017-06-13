#ifndef FISC_SPECIAL_H_
#define FISC_SPECIAL_H_

#include <fvm/Utils/String.h>
#include "FISCISA.h"

/**************************************************************/
/***************** STATUS CONTROL INSTRUCTIONS ****************/
/**************************************************************/
NEW_INSTRUCTION(FISC, MSR, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, MRS, RF,
{
	return FISC_RET_OK;
});

/**************************************************************/
/******************* INTERRUPT INSTRUCTIONS *******************/
/**************************************************************/
NEW_INSTRUCTION(FISC, LIVP, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, SIVP, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, LEVP, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, SEVP, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, SESR, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, SINT, BF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, RETI, BF,
{
	return FISC_RET_OK;
});

/**************************************************************/
/***************** VIRTUAL MEMORY INSTRUCTIONS ****************/
/**************************************************************/
NEW_INSTRUCTION(FISC, LPDP, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, SPDP, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, LPFLA, RF,
{
	return FISC_RET_OK;
});

#endif