#ifndef FISC_FPU_H_
#define FISC_FPU_H_

#include <fvm/Utils/String.h>
#include "FISCISA.h"

/**************************************************************/
/********************** FADD INSTRUCTIONS *********************/
/**************************************************************/
NEW_INSTRUCTION(FISC, FADDS, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, FADDD, RF,
{
	return FISC_RET_OK;
});

/**************************************************************/
/********************** FSUB INSTRUCTIONS *********************/
/**************************************************************/
NEW_INSTRUCTION(FISC, FSUBS, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, FSUBD, RF,
{
	return FISC_RET_OK;
});

/**************************************************************/
/********************** FCMP INSTRUCTIONS *********************/
/**************************************************************/
NEW_INSTRUCTION(FISC, FCMPS, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, FCMPD, RF,
{
	return FISC_RET_OK;
});

/**************************************************************/
/********************** FMUL INSTRUCTIONS *********************/
/**************************************************************/
NEW_INSTRUCTION(FISC, FMULS, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, FMULD, RF,
{
	return FISC_RET_OK;
});

/**************************************************************/
/********************** FDIV INSTRUCTIONS *********************/
/**************************************************************/
NEW_INSTRUCTION(FISC, FDIVS, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, FDIVD, RF,
{
	return FISC_RET_OK;
});

/**************************************************************/
/******************** FPU LOAD INSTRUCTIONS *******************/
/**************************************************************/
NEW_INSTRUCTION(FISC, LDRS, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, LDRD, RF,
{
	return FISC_RET_OK;
});

/**************************************************************/
/******************** FPU STORE INSTRUCTIONS *******************/
/**************************************************************/
NEW_INSTRUCTION(FISC, STRS, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, STRD, RF,
{
	return FISC_RET_OK;
});

#endif
