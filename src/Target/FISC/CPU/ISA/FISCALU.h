#ifndef FISC_ALU_H_
#define FISC_ALU_H_

#include <fvm/Utils/String.h>
#include "../FISCCPUModule.h"
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
		_cpu_->readRegister(_this_->ifmt_r->rm),  /* R[Rm]   */
		false, 0,0,0); /* Doesn't set ALU flags */
});

NEW_INSTRUCTION(FISC, ADDI, IF, /* Operation: R[Rd] = R[Rn] + ALUImm */
{
	int64_t aluimm = _this_->ifmt_i->alu_immediate;
	if(aluimm & (1<<(12-1))) aluimm = -((~aluimm + 1) & 0xFFF); /* Fix non 64-bit number signedness */

	return
		_cpu_->writeRegister(_this_->ifmt_i->rd,  /* R[Rd] = */
		_cpu_->readRegister(_this_->ifmt_i->rn)   /* R[Rn]   */
		+                                         /*   +     */
		aluimm,                                   /* ALUImm  */
		false, 0, 0, 0); /* Doesn't set ALU flags */
});

NEW_INSTRUCTION(FISC, ADDIS, IF, /* Operation: R[Rd],Flags = R[Rn] + ALUImm */
{
	uint64_t op1 = _cpu_->readRegister(_this_->ifmt_i->rn);
	uint64_t op2 = (uint64_t)_this_->ifmt_i->alu_immediate;
	if (op2 & (1 << (12 - 1))) op2 = -1 * ((~op2 + 1) & 0xFFF); /* Fix non 64-bit number signedness */

	return
		_cpu_->writeRegister(_this_->ifmt_i->rd,  /* R[Rd] = */
		op1                                       /* R[Rn]   */
		+                                         /*   +     */
	    op2,                                      /* ALUImm  */
		true, op1, op2, '+'); /* Sets ALU flags */
});

NEW_INSTRUCTION(FISC, ADDS, RF, /* Operation: R[Rd],Flags = R[Rn] + R[Rm] */
{
	uint64_t op1 = _cpu_->readRegister(_this_->ifmt_r->rn);
	uint64_t op2 = _cpu_->readRegister(_this_->ifmt_r->rm);

	return
		_cpu_->writeRegister(_this_->ifmt_r->rd,  /* R[Rd] = */
		op1                                       /* R[Rn]   */
		+                                         /*   +     */
	    op2,                                      /* R[Rm]   */
		true, op1, op2, '+'); /* Sets ALU flags */
});

/**************************************************************/
/********************** SUB INSTRUCTIONS **********************/
/**************************************************************/
NEW_INSTRUCTION(FISC, SUB, RF, /* Operation: R[Rd] = R[Rn] - R[Rm] */
{
	return 
		_cpu_->writeRegister(_this_->ifmt_r->rd,  /* R[Rd] = */
		_cpu_->readRegister(_this_->ifmt_r->rn)   /* R[Rn]   */
		-                                         /*   -     */
		_cpu_->readRegister(_this_->ifmt_r->rm),  /* R[Rm]   */
		false, 0,0,0); /* Doesn't set ALU flags */
});

NEW_INSTRUCTION(FISC, SUBI, IF, /* Operation: R[Rd] = R[Rn] - ALUImm */
{
	int64_t aluimm = _this_->ifmt_i->alu_immediate;
	if(aluimm & (1<<(12-1))) aluimm = -1 * ((~aluimm + 1) & 0xFFF); /* Fix non 64-bit number signedness */

	return
		_cpu_->writeRegister(_this_->ifmt_i->rd,  /* R[Rd] = */
		_cpu_->readRegister(_this_->ifmt_i->rn)   /* R[Rn]   */
		-                                         /*   -     */
		aluimm,                                   /* ALUImm  */
		false, 0, 0, 0); /* Doesn't set ALU flags */
});

NEW_INSTRUCTION(FISC, SUBIS, IF, /* Operation: R[Rd],Flags = R[Rn] - ALUImm */
{
	uint64_t op1 = _cpu_->readRegister(_this_->ifmt_i->rn);
	uint64_t op2 = (uint64_t)_this_->ifmt_i->alu_immediate;
	if (op2 & (1 << (12 - 1))) op2 = -1 * ((~op2 + 1) & 0xFFF); /* Fix non 64-bit number signedness */

	return
		_cpu_->writeRegister(_this_->ifmt_i->rd,  /* R[Rd] = */
		op1                                       /* R[Rn]   */
		-                                         /*   -     */
	    op2,                                      /* ALUImm  */
		true, op1, op2, '-'); /* Sets ALU flags */
});

NEW_INSTRUCTION(FISC, SUBS, RF, /* Operation: R[Rd],Flags = R[Rn] - R[Rm] */
{
	uint64_t op1 = _cpu_->readRegister(_this_->ifmt_r->rn);
	uint64_t op2 = _cpu_->readRegister(_this_->ifmt_r->rm);

	return
		_cpu_->writeRegister(_this_->ifmt_r->rd,  /* R[Rd] = */
		op1                                       /* R[Rn]   */
		-                                         /*   -     */
	    op2,                                      /* R[Rm]   */
		true, op1, op2, '-'); /* Sets ALU flags */
});

/**************************************************************/
/********************** MUL INSTRUCTIONS **********************/
/**************************************************************/
NEW_INSTRUCTION(FISC, MUL, RF, /* Operation: R[Rd] = R[Rn] * R[Rm] */
{
	return 
		_cpu_->writeRegister(_this_->ifmt_r->rd,  /* R[Rd] = */
		_cpu_->readRegister(_this_->ifmt_r->rn)   /* R[Rn]   */
		*                                         /*   *     */
		_cpu_->readRegister(_this_->ifmt_r->rm),  /* R[Rm]   */
		false, 0,0,0); /* Doesn't set ALU flags */
});

NEW_INSTRUCTION(FISC, SMULH, RF, /* Operation: R[Rd] = R[Rn] * R[Rm] */
{
	return 
		_cpu_->writeRegister(_this_->ifmt_r->rd,  /* R[Rd] = */
		_cpu_->readRegister(_this_->ifmt_r->rn)   /* R[Rn]   */
		*                                         /*   *     */
		_cpu_->readRegister(_this_->ifmt_r->rm),  /* R[Rm]   */
		false, 0,0,0); /* Doesn't set ALU flags */
});

NEW_INSTRUCTION(FISC, UMULH, RF, /* Operation: R[Rd] = R[Rn] * R[Rm] */
{
	return 
		_cpu_->writeRegister(_this_->ifmt_r->rd,  /* R[Rd] = */
		_cpu_->readRegister(_this_->ifmt_r->rn)   /* R[Rn]   */
		*                                         /*   *     */
		_cpu_->readRegister(_this_->ifmt_r->rm),  /* R[Rm]   */
		false, 0,0,0); /* Doesn't set ALU flags */
});

/**************************************************************/
/********************** DIV INSTRUCTIONS **********************/
/**************************************************************/
NEW_INSTRUCTION(FISC, SDIV, RF, /* Operation: R[Rd] = R[Rn] / R[Rm] */
{
	return 
		_cpu_->writeRegister(_this_->ifmt_r->rd,  /* R[Rd] = */
		_cpu_->readRegister(_this_->ifmt_r->rn)   /* R[Rn]   */
		/                                         /*   /     */
		_cpu_->readRegister(_this_->ifmt_r->rm),  /* R[Rm]   */
		false, 0,0,0); /* Doesn't set ALU flags */
});

NEW_INSTRUCTION(FISC, UDIV, RF, /* Operation: R[Rd] = R[Rn] / R[Rm] */
{
	return 
		_cpu_->writeRegister(_this_->ifmt_r->rd,  /* R[Rd] = */
		_cpu_->readRegister(_this_->ifmt_r->rn)   /* R[Rn]   */
		/                                         /*   /     */
		_cpu_->readRegister(_this_->ifmt_r->rm),  /* R[Rm]   */
		false, 0,0,0); /* Doesn't set ALU flags */
});

/**************************************************************/
/********************** AND INSTRUCTIONS **********************/
/**************************************************************/
NEW_INSTRUCTION(FISC, AND, RF, /* Operation: R[Rd] = R[Rn] & R[Rm] */
{
	return 
		_cpu_->writeRegister(_this_->ifmt_r->rd,  /* R[Rd] = */
		_cpu_->readRegister(_this_->ifmt_r->rn)   /* R[Rn]   */
		&                                         /*   &     */
		_cpu_->readRegister(_this_->ifmt_r->rm),  /* R[Rm]   */
		false, 0,0,0); /* Doesn't set ALU flags */
});

NEW_INSTRUCTION(FISC, ANDI, IF, /* Operation: R[Rd] = R[Rn] & ALUImm */
{
	int64_t aluimm = _this_->ifmt_i->alu_immediate;
	if(aluimm & (1<<(12-1))) aluimm = -1 * ((~aluimm + 1) & 0xFFF); /* Fix non 64-bit number signedness */

	return
		_cpu_->writeRegister(_this_->ifmt_i->rd,  /* R[Rd] = */
		_cpu_->readRegister(_this_->ifmt_i->rn)   /* R[Rn]   */
		&                                         /*   &     */
		aluimm,                                   /* ALUImm  */
		false, 0, 0, 0); /* Doesn't set ALU flags */
});

NEW_INSTRUCTION(FISC, ANDIS, IF, /* Operation: R[Rd],Flags = R[Rn] & ALUImm */
{
	uint64_t op1 = _cpu_->readRegister(_this_->ifmt_i->rn);
	uint64_t op2 = (uint64_t)_this_->ifmt_i->alu_immediate;
	if (op2 & (1 << (12 - 1))) op2 = -1 * ((~op2 + 1) & 0xFFF); /* Fix non 64-bit number signedness */

	return
		_cpu_->writeRegister(_this_->ifmt_i->rd,  /* R[Rd] = */
		(uint64_t)
		(op1                                      /* R[Rn]   */
		  &                                       /*   &     */
	     op2),                                    /* ALUImm  */
		true, op1, op2, '&'); /* Sets ALU flags */
});

NEW_INSTRUCTION(FISC, ANDS, RF, /* Operation: R[Rd],Flags = R[Rn] & R[Rm] */
{
	uint64_t op1 = _cpu_->readRegister(_this_->ifmt_r->rn);
	uint64_t op2 = _cpu_->readRegister(_this_->ifmt_r->rm);

	return
		_cpu_->writeRegister(_this_->ifmt_r->rd,  /* R[Rd] = */
		(uint64_t)
		(op1                                      /* R[Rn]   */
		&                                         /*   &     */
	    op2),                                     /* R[Rm]   */
		true, op1, op2, '&'); /* Sets ALU flags */
});

/**************************************************************/
/****************** ORR AND EOR INSTRUCTIONS ******************/
/**************************************************************/
NEW_INSTRUCTION(FISC, ORR, RF, /* Operation: R[Rd] = R[Rn] | R[Rm] */
{
	return 
		_cpu_->writeRegister(_this_->ifmt_r->rd,  /* R[Rd] = */
		_cpu_->readRegister(_this_->ifmt_r->rn)   /* R[Rn]   */
		|                                         /*   |     */
		_cpu_->readRegister(_this_->ifmt_r->rm),  /* R[Rm]   */
		false, 0,0,0); /* Doesn't set ALU flags */
});

NEW_INSTRUCTION(FISC, ORRI, IF, /* Operation: R[Rd] = R[Rn] | ALUImm */
{
	int64_t aluimm = _this_->ifmt_i->alu_immediate;
	if(aluimm & (1<<(12-1))) aluimm = -1 * ((~aluimm + 1) & 0xFFF); /* Fix non 64-bit number signedness */

	return
		_cpu_->writeRegister(_this_->ifmt_i->rd,  /* R[Rd] = */
		_cpu_->readRegister(_this_->ifmt_i->rn)   /* R[Rn]   */
		|                                         /*   |     */
		aluimm,                                   /* ALUImm  */
		false, 0, 0, 0); /* Doesn't set ALU flags */
});

NEW_INSTRUCTION(FISC, EOR, RF, /* Operation: R[Rd] = R[Rn] ^ R[Rm] */
{
	return 
		_cpu_->writeRegister(_this_->ifmt_r->rd,  /* R[Rd] = */
		_cpu_->readRegister(_this_->ifmt_r->rn)   /* R[Rn]   */
		^                                         /*   ^     */
		_cpu_->readRegister(_this_->ifmt_r->rm),  /* R[Rm]   */
		false, 0,0,0); /* Doesn't set ALU flags */
});

NEW_INSTRUCTION(FISC, EORI, IF, /* Operation: R[Rd] = R[Rn] ^ ALUImm */
{
	int64_t aluimm = _this_->ifmt_i->alu_immediate;
	if(aluimm & (1<<(12-1))) aluimm = -1 * ((~aluimm + 1) & 0xFFF); /* Fix non 64-bit number signedness */

	return
		_cpu_->writeRegister(_this_->ifmt_i->rd,  /* R[Rd] = */
		_cpu_->readRegister(_this_->ifmt_i->rn)   /* R[Rn]   */
		^                                         /*   ^     */
		aluimm,                                   /* ALUImm  */
		false, 0, 0, 0); /* Doesn't set ALU flags */
});

/**************************************************************/
/****************** NEG AND NOT INSTRUCTIONS ******************/
/**************************************************************/
NEW_INSTRUCTION(FISC, NEG, RF, /* Operation: R[Rd] = !R[Rn] */
{
	return 
		_cpu_->writeRegister(_this_->ifmt_r->rd,  /* R[Rd] = */
		!_cpu_->readRegister(_this_->ifmt_r->rn), /* !R[Rn]  */
		false, 0,0,0); /* Doesn't set ALU flags */
});

NEW_INSTRUCTION(FISC, NEGI, IF, /* Operation: R[Rd] = !ALUImm */
{
	int64_t aluimm = _this_->ifmt_i->alu_immediate;
	if(aluimm & (1<<(12-1))) aluimm = -1 * ((~aluimm + 1) & 0xFFF); /* Fix non 64-bit number signedness */

	return
		_cpu_->writeRegister(_this_->ifmt_i->rd,  /* R[Rd] = */
		!aluimm,                                  /* !ALUImm  */
		false, 0, 0, 0); /* Doesn't set ALU flags */
});

NEW_INSTRUCTION(FISC, NOT, RF, /* Operation: R[Rd] = ~R[Rn] */
{
	return 
		_cpu_->writeRegister(_this_->ifmt_r->rd,  /* R[Rd] = */
		~_cpu_->readRegister(_this_->ifmt_r->rn), /* ~R[Rn]  */
		false, 0,0,0); /* Doesn't set ALU flags */
});

NEW_INSTRUCTION(FISC, NOTI, IF, /* Operation: R[Rd] = ~ALUImm */
{
	int64_t aluimm = _this_->ifmt_i->alu_immediate;
	if(aluimm & (1<<(12-1))) aluimm = -1 * ((~aluimm + 1) & 0xFFF); /* Fix non 64-bit number signedness */

	return
		_cpu_->writeRegister(_this_->ifmt_i->rd,  /* R[Rd] = */
		~aluimm,                                  /* ~ALUImm  */
		false, 0, 0, 0); /* Doesn't set ALU flags */
});

/**************************************************************/
/****************** LSL AND LSR INSTRUCTIONS ******************/
/**************************************************************/
NEW_INSTRUCTION(FISC, LSL, RF, /* Operation: R[Rd] = R[Rn] << shamt */
{
	return 
		_cpu_->writeRegister(_this_->ifmt_r->rd,  /* R[Rd] = */
		_cpu_->readRegister(_this_->ifmt_r->rn)   /* R[Rn]   */
		<<                                        /*  <<     */
		_this_->ifmt_r->shamt,                    /* shamt   */
		false, 0,0,0); /* Doesn't set ALU flags */
});

NEW_INSTRUCTION(FISC, LSR, RF, /* Operation: R[Rd] = R[Rn] >> shamt */
{
	return 
		_cpu_->writeRegister(_this_->ifmt_r->rd,  /* R[Rd] = */
		_cpu_->readRegister(_this_->ifmt_r->rn)   /* R[Rn]   */
		>>                                        /*  >>     */
		_this_->ifmt_r->shamt,                    /* shamt   */
		false, 0,0,0); /* Doesn't set ALU flags */
});

#endif
