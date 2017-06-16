#ifndef FISC_SPECIAL_H_
#define FISC_SPECIAL_H_

#include <fvm/Utils/String.h>
#include "FISCISA.h"

/**************************************************************/
/***************** STATUS CONTROL INSTRUCTIONS ****************/
/**************************************************************/
NEW_INSTRUCTION(FISC, MSR, RF, /* Operation: CPSR_x? = R[Rn] (Note: the _x field is in R[Rd])*/
{
	uint64_t rnReg = _cpu_->readRegister(_this_->ifmt_r->rn);
	uint64_t cpsrRegVal = _cpu_->readRegister(SPECIAL_CPSR);
	cpsr_t * cpsr = (cpsr_t*)&cpsrRegVal;
	uint16_t cpsr_field = _cpu_->readRegister(_this_->ifmt_r->rd) & 0x1F;

	/* Check if the current CPU mode has permissions to write to a particular field in the CPSR */
	if(cpsr->mode == FISC_CPU_MODE_USER) {
		switch (cpsr_field) {
		case 0: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13:
			/* The user does not have permission to read these selected CPSR fields! */
			return _cpu_->triggerSoftException(EXC_PERMFAULT);
		}
	}

	/* We have permission. We shall now write into the CPSR register */
	switch (cpsr_field) {
	case 0: {
		/* ALERT: WE MIGHT CHANGE CPU MODE HERE */
		if((rnReg & 7) >= FISC_CPU_MODE_USER && (rnReg & 7) <= FISC_CPU_MODE_EXCEPTION) {
			cpsrRegVal = rnReg;
		} else {
			/* This CPU mode is invalid! Entering Undefined CPU mode and triggering PERMFAULT exception */
			cpsr->mode = FISC_CPU_MODE_UNDEFINED;
			_cpu_->writeRegister(SPECIAL_CPSR, cpsrRegVal, false, 0, 0, 0);
			return _cpu_->triggerSoftException(EXC_PERMFAULT);
		}
		break;
	}
	case 1:
		cpsr->c =  rnReg & 1;
		cpsr->v = (rnReg & 2) >> 1;
		cpsr->z = (rnReg & 4) >> 2;
		cpsr->n = (rnReg & 8) >> 3;
		break;
	case 2:  cpsr->n   = rnReg & 1; break;
	case 3:  cpsr->z   = rnReg & 1; break;
	case 4:  cpsr->v   = rnReg & 1; break;
	case 5:  cpsr->c   = rnReg & 1; break;
	case 6:  cpsr->ae  = rnReg & 3; break;
	case 7:  BIT_WRITE(rnReg & 1, cpsr->ae, 0); break;
	case 8:  BIT_WRITE(rnReg & 1, cpsr->ae, 1); break;
	case 9:  cpsr->pg  = rnReg & 1; break;
	case 10: cpsr->ien = rnReg & 3; break;
	case 11: BIT_WRITE(rnReg & 1, cpsr->ien, 0); break;
	case 12: BIT_WRITE(rnReg & 1, cpsr->ien, 1); break;
	case 13: {
		/* ALERT: WE'RE CHANGING CPU MODE HERE */
		if ((rnReg & 7) >= FISC_CPU_MODE_USER && (rnReg & 7) <= FISC_CPU_MODE_EXCEPTION) {
			cpsr->mode = rnReg & 7;
		}
		else {
			/* This CPU mode is invalid! Entering Undefined CPU mode and triggering PERMFAULT exception */
			cpsr->mode = FISC_CPU_MODE_UNDEFINED;
			_cpu_->writeRegister(SPECIAL_CPSR, cpsrRegVal, false, 0, 0, 0);
			return _cpu_->triggerSoftException(EXC_PERMFAULT);
		}
		break;
	}
	default: /* Invalid CPSR field */ return FISC_RET_ERROR;
	}

	return _cpu_->writeRegister(SPECIAL_CPSR, cpsrRegVal, false, 0, 0, 0);
});

NEW_INSTRUCTION(FISC, MRS, RF, /* Operation: R[Rd] = CPSR_x? (Note: the _x field is in R[Rn])*/
{
	uint64_t rdRegVal = (uint64_t)-1;
	uint64_t cpsrRegVal = _cpu_->readRegister(SPECIAL_CPSR);
	cpsr_t * cpsr = (cpsr_t*)&cpsrRegVal;
	uint16_t cpsr_field = _cpu_->readRegister(_this_->ifmt_r->rn) & 0x1F;

	/* Check if the current CPU mode has permissions to read from a particular field in the CPSR */
	if (cpsr->mode == FISC_CPU_MODE_USER) {
		switch (cpsr_field) {
		case 0: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13:
			/* The user does not have permission to read these selected CPSR fields! */
			return _cpu_->triggerSoftException(EXC_PERMFAULT);
		}
	}

	/* We have permission. We shall now read from the CPSR register */
	switch (cpsr_field) {
	case 0: rdRegVal = cpsrRegVal; break;
	case 1: 
		rdRegVal |= cpsr->c;
		rdRegVal |= cpsr->v << 1;
		rdRegVal |= cpsr->z << 2;
		rdRegVal |= cpsr->n << 3;
		break;
	case 2:  rdRegVal = cpsr->n;       break;
	case 3:  rdRegVal = cpsr->z;       break;
	case 4:  rdRegVal = cpsr->v;       break;
	case 5:  rdRegVal = cpsr->c;       break;
	case 6:  rdRegVal = cpsr->ae;      break;
	case 7:  rdRegVal = cpsr->ae & 1;  break;
	case 8:  rdRegVal = (cpsr->ae & 2) >> 1; break;
	case 9:  rdRegVal = cpsr->pg;      break;
	case 10: rdRegVal = cpsr->ien;     break;
	case 11: rdRegVal = cpsr->ien & 1; break;
	case 12: rdRegVal = (cpsr->ien & 2) >> 1; break;
	case 13: rdRegVal = cpsr->mode;    break;
	default: /* Invalid CPSR field */ return FISC_RET_ERROR;
	}

	return _cpu_->writeRegister(_this_->ifmt_r->rd, rdRegVal, false, 0, 0, 0);
});

/**************************************************************/
/******************* INTERRUPT INSTRUCTIONS *******************/
/**************************************************************/
NEW_INSTRUCTION(FISC, LIVP, RF, /* Operation: IVP = R[Rd] */
{
	return _cpu_->writeRegister(SPECIAL_IVP, _cpu_->readRegister(_this_->ifmt_r->rd), false, 0, 0, 0);
});

NEW_INSTRUCTION(FISC, SIVP, RF, /* Operation: R[Rd] = IVP */
{
	return _cpu_->writeRegister(_this_->ifmt_r->rd, _cpu_->readRegister(SPECIAL_IVP), false, 0, 0, 0);
});

NEW_INSTRUCTION(FISC, LEVP, RF, /* Operation: EVP = R[Rd] */
{
	return _cpu_->writeRegister(SPECIAL_EVP, _cpu_->readRegister(_this_->ifmt_r->rd), false, 0, 0, 0);
});

NEW_INSTRUCTION(FISC, SEVP, RF, /* Operation:  R[Rd] = EVP */
{
	return _cpu_->writeRegister(_this_->ifmt_r->rd, _cpu_->readRegister(SPECIAL_EVP), false, 0, 0, 0);
});

NEW_INSTRUCTION(FISC, SESR, RF, /* Operation: R[Rd] = ESR */
{
	return _cpu_->writeRegister(_this_->ifmt_r->rd, _cpu_->readRegister(SPECIAL_ESR), false, 0, 0, 0);
});

NEW_INSTRUCTION(FISC, SINT, BF, /* Operation: CPSR_IEN[1] = 0; PC = IVP + INT_ID */
{
	return _cpu_->triggerSoftInterrupt(_this_->ifmt_b->br_address);
});

NEW_INSTRUCTION(FISC, RETI, BF, /* Operation: PC = ELR; CPSR_IEN[1] = 1; */
{
	return _cpu_->intExcReturn(_this_->ifmt_b->br_address);
});

/**************************************************************/
/***************** VIRTUAL MEMORY INSTRUCTIONS ****************/
/**************************************************************/
NEW_INSTRUCTION(FISC, LPDP, RF, /* Operation: PDP = R[Rd] */
{
	return _cpu_->writeRegister(SPECIAL_PDP, _cpu_->readRegister(_this_->ifmt_r->rd), false, 0, 0, 0);
});

NEW_INSTRUCTION(FISC, SPDP, RF, /* Operation: R[Rd] = PDP */
{
	return _cpu_->writeRegister(_this_->ifmt_r->rd, _cpu_->readRegister(SPECIAL_PDP), false, 0, 0, 0);
});

NEW_INSTRUCTION(FISC, LPFLA, RF, /* Operation: R[LR] = PDP */
{
	return _cpu_->writeRegister(LR, _cpu_->readRegister(SPECIAL_PFLA), false, 0, 0, 0);
});

#endif