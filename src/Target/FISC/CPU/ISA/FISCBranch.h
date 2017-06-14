#ifndef FISC_BRANCH_H_
#define FISC_BRANCH_H_

#include <fvm/Utils/String.h>
#include "FISCISA.h"

/**************************************************************/
/******************** B AND BL INSTRUCTIONS *******************/
/**************************************************************/
NEW_INSTRUCTION(FISC, B, BF, /* Operation: PC = PC + br_address */
{
	int32_t addr = _this_->ifmt_b->br_address;
	if(addr & (1<<(26-1))) addr = -((~addr+1) & 0x3FFFFFF); /* Fix non 64-bit number signedness */
	return _cpu_->branch(addr, true);
});

NEW_INSTRUCTION(FISC, BL, BF, /* Operation: R[LR] = PC + 4; PC = PC + br_address */
{
	int32_t addr = _this_->ifmt_b->br_address;
	if(addr & (1<<(26-1))) addr = -((~addr+1) & 0x3FFFFFF); /* Fix non 64-bit number signedness */
	_cpu_->writeRegister(LR, _cpu_->readRegister(SPECIAL_PC) + 4, false, 0, 0, 0);
	return _cpu_->branch(addr, true);
});

/**************************************************************/
/*********************** BR INSTRUCTION ***********************/
/**************************************************************/
NEW_INSTRUCTION(FISC, BR, RF, /* Operation: PC = R[Rt] */
{
	return _cpu_->branch((uint32_t)_cpu_->readRegister(_this_->ifmt_r->rd), false);
});

/**************************************************************/
/************** CONDITIONAL BRANCH INSTRUCTIONS ***************/
/**************************************************************/
NEW_INSTRUCTION(FISC, BCOND, CBF, /* Operation: if(FLAGS == cond) PC += CondBranchAddr */
{
	int64_t addr = _this_->ifmt_cb->cond_br_address;
	if (addr & (1 << (19 - 1))) addr = -((~addr + 1) & 0x7FFFF); /* Fix non 64-bit number signedness */

	uint64_t cpsrVal = _cpu_->readRegister(SPECIAL_CPSR);
	cpsr_t cpsr = *(cpsr_t*)&cpsrVal;

	switch(_this_->ifmt_cb->rt) {
		case 0: /* BEQ */
			if(cpsr.z) return _cpu_->branch((int32_t)addr, true);
			break;
		case 1: /* BNE */
			if(!cpsr.z) return _cpu_->branch((int32_t)addr, true);
			break;
		case 2: /* BLT */
			if(cpsr.n ^ cpsr.v) return _cpu_->branch((int32_t)addr, true);
			break;
		case 3: /* BLE */
			if(!(!cpsr.z & !(cpsr.n ^ cpsr.v))) return _cpu_->branch((int32_t)addr, true);
			break;
		case 4: /* BGT */
			if((!cpsr.z & !(cpsr.n ^ cpsr.v))) return _cpu_->branch((int32_t)addr, true);
			break;
		case 5: /* BGE */
			if(!(cpsr.n ^ cpsr.v)) return _cpu_->branch((int32_t)addr, true);
			break;
		case 6: /* BLO */
			if(!(cpsr.c)) return _cpu_->branch((int32_t)addr, true);
			break;
		case 7: /* BLS */
			if(!(!cpsr.z & cpsr.z)) return _cpu_->branch((int32_t)addr, true);
			break;
		case 8: /* BHI */
			if((!cpsr.z & cpsr.c)) return _cpu_->branch((int32_t)addr, true);
			break;
		case 9: /* BHS */
			if(cpsr.c) return _cpu_->branch((int32_t)addr, true);
			break;
		case 10: /* BMI */
			if(cpsr.n) return _cpu_->branch((int32_t)addr, true);
			break;
		case 11: /* BPL */
			if(!cpsr.n) return _cpu_->branch((int32_t)addr, true);
			break;
		case 12: /* BVS */
			if(cpsr.v) return _cpu_->branch((int32_t)addr, true);
			break;
		case 13: /* BVC */
			if(!cpsr.v) return _cpu_->branch((int32_t)addr, true);
			break;
		default: /* Invalid Conditional Branch type */ return FISC_RET_ERROR;
		}
	/* The branch was not taken */
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, CBNZ, CBF, /* Operation: if(R[Rt] != 0) PC += CondBranchAddr */
{
	int64_t addr = _this_->ifmt_cb->cond_br_address;
	if(addr & (1<<(19-1))) addr = -((~addr+1) & 0x7FFFF); /* Fix non 64-bit number signedness */
	if(_cpu_->readRegister(_this_->ifmt_cb->rt) != 0)
		return _cpu_->branch((int32_t)addr, true); /* The branch was taken */
	/* The branch was not taken */
	return FISC_RET_OK;
});

NEW_INSTRUCTION(FISC, CBZ, CBF, /* Operation: if(R[Rt] == 0) PC += CondBranchAddr */
{
	int64_t addr = _this_->ifmt_cb->cond_br_address;
	if(addr & (1<<(19-1))) addr = -((~addr+1) & 0x7FFFF); /* Fix non 64-bit number signedness */
	if(_cpu_->readRegister(_this_->ifmt_cb->rt) == 0)
		return _cpu_->branch((int32_t)addr, true); /* The branch was taken */
	/* The branch was not taken */
	return FISC_RET_OK;
});

#endif
