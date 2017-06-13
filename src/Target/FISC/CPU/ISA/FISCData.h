#ifndef FISC_DATA_H_
#define FISC_DATA_H_

#include <fvm/Utils/String.h>
#include "FISCISA.h"

/**************************************************************/
/***************** MOVZ AND MOVK INSTRUCTIONS *****************/
/**************************************************************/
NEW_INSTRUCTION(FISC, MOVZ, IWF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, MOVK, IWF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, MOVRZ, IWF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, MOVRK, IWF,
{
	return FISC_RET_OK;
});

/**************************************************************/
/********************** LDPC INSTRUCTION **********************/
/**************************************************************/
NEW_INSTRUCTION(FISC, LDPC, RF,
{
	return FISC_RET_OK;
});

/**************************************************************/
/********************* LOAD INSTRUCTIONS **********************/
/**************************************************************/
NEW_INSTRUCTION(FISC, LDR, DF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, LDRB, DF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, LDRH, DF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, LDRSW, DF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, LDXR, DF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, LDRR, DF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, LDRBR, DF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, LDRHR, DF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, LDRSWR, DF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, LDXRR, DF,
{
	return FISC_RET_OK;
});

/**************************************************************/
/********************* STORE INSTRUCTIONS *********************/
/**************************************************************/
NEW_INSTRUCTION(FISC, STR_, DF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, STRB, DF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, STRH, DF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, STRW, DF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, STXR, DF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, STRR, DF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, STRBR, DF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, STRHR, DF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, STRWR, DF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, STXRR, DF,
{
	return FISC_RET_OK;
});

#endif
