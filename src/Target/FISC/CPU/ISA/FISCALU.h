#ifndef FISC_ALU_H_
#define FISC_ALU_H_

#include <fvm/Utils/String.h>
#include "FISCISA.h"

/**************************************************************/
/********************** ADD INSTRUCTIONS **********************/
/**************************************************************/
NEW_INSTRUCTION(FISC, ADD, RF, /* Operation: R[Rd] = R[Rn] + R[Rm] */
{
	return 
		_cpu_->writeRegister(_this_->ifmt_r->rd,  /* R[Rd] = */
		_cpu_->readRegister(_this_->ifmt_r->rn)   /* R[Rn]   */
		+                                         /*   +     */
		_cpu_->readRegister(_this_->ifmt_r->rm)); /* R[Rm]   */
});

NEW_INSTRUCTION(FISC, ADDI, IF, /* Operation: R[Rd] = R[Rn] + ALUImm */
{
	return
		_cpu_->writeRegister(_this_->ifmt_i->rd,  /* R[Rd] = */
		_cpu_->readRegister(_this_->ifmt_i->rn)   /* R[Rn]   */
		+                                         /*   +     */
		_this_->ifmt_i->alu_immediate);           /* ALUimm  */
});

NEW_INSTRUCTION(FISC, ADDIS, IF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, ADDS, RF,
{
	return FISC_RET_OK;
});

/**************************************************************/
/********************** SUB INSTRUCTIONS **********************/
/**************************************************************/
NEW_INSTRUCTION(FISC, SUB, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, SUBI, IF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, SUBIS, IF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, SUBS, RF,
{
	return FISC_RET_OK;
});

/**************************************************************/
/********************** MUL INSTRUCTIONS **********************/
/**************************************************************/
NEW_INSTRUCTION(FISC, MUL, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, SMULH, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, UMULH, RF,
{
	return FISC_RET_OK;
});

/**************************************************************/
/********************** DIV INSTRUCTIONS **********************/
/**************************************************************/
NEW_INSTRUCTION(FISC, SDIV, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, UDIV, RF,
{
	return FISC_RET_OK;
});

/**************************************************************/
/********************** AND INSTRUCTIONS **********************/
/**************************************************************/
NEW_INSTRUCTION(FISC, AND, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, ANDI, IF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, ANDIS, IF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, ANDS, RF,
{
	return FISC_RET_OK;
});

/**************************************************************/
/****************** ORR AND EOR INSTRUCTIONS ******************/
/**************************************************************/
NEW_INSTRUCTION(FISC, ORR, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, ORRI, IF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, EOR, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, EORI, IF,
{
	return FISC_RET_OK;
});

/**************************************************************/
/****************** NEG AND NOT INSTRUCTIONS ******************/
/**************************************************************/
NEW_INSTRUCTION(FISC, NEG, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, NEGI, IF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, NOT, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, NOTI, IF,
{
	return FISC_RET_OK;
});

/**************************************************************/
/****************** LSL AND LSR INSTRUCTIONS ******************/
/**************************************************************/
NEW_INSTRUCTION(FISC, LSL, RF,
{
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, LSR, RF,
{
	return FISC_RET_OK;
});

#endif
