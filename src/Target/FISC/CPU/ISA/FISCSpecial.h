#ifndef FISC_SPECIAL_H_
#define FISC_SPECIAL_H_

#include <fvm/Utils/String.h>
#include "FISCISA.h"

/**************************************************************/
/***************** STATUS CONTROL INSTRUCTIONS ****************/
/**************************************************************/
NEW_INSTRUCTION(FISC, MSR, RF, /* Operation: CPSR/SPSR_x? = R[Rn] (Note: the _x field is in R[Rd])*/
{
	/* 1- Read the actual contents we will write into the destination register */
	uint64_t rnReg = _cpu_->readRegister(_this_->ifmt_r->rn);

	/* 2- Get the CPSR/SPSR field to which we will be writing into */
	uint16_t cpsr_field = _cpu_->readRegister(_this_->ifmt_r->rd) & 0x1F;
	
	/* 3- Read CPSR */
	uint64_t cpsrRegVal = _cpu_->readRegister(SPECIAL_CPSR);
	
	/* 4- Read SPSR */
	uint64_t spsrRegVal = _cpu_->readRegister(SPECIAL_SPSR0 + (cpsrRegVal & 7));
	
	/* 5- Now we will use 1 single pointer to write either to CPSR or SPSR */
	uint64_t * psrRegValPtr = nullptr;
	cpsr_t * psrPtr = nullptr;

	enum SPECIAL_REGISTERS regIndex; /* Index of the destination register */

	bool cpsr_or_spsr = cpsr_field & 16 ? true : false; /* 0: Write to CPSR. 1: Write to SPSR */
	
	if(!cpsr_or_spsr) {
		psrRegValPtr = &cpsrRegVal; /* Point to the CPSR */
		regIndex = SPECIAL_CPSR;
	} else {
		psrRegValPtr = &spsrRegVal; /* Point to the current SPSR instead of the CPSR */
		regIndex = (enum SPECIAL_REGISTERS)(SPECIAL_SPSR0 + (cpsrRegVal & 7));
	}

	/* 6- Point the cpsr_t pointer to the selected integer pointer */
	psrPtr = (cpsr_t*)psrRegValPtr;

	/* 7- Check if the current CPU mode has permissions to write to a particular field in the CPSR */
	if(psrPtr->mode == FISC_CPU_MODE_USER) {
		switch (cpsr_field) {
		case 0: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13:
			/* The user does not have permission to read these selected CPSR fields! */
			return _cpu_->triggerSoftException(EXC_PERMFAULT);
		}
	}

	/* 8- We have permission. We shall now write into the CPSR register */
	switch (cpsr_field) {
	case 0: {
		/* ALERT: WE MIGHT CHANGE CPU MODE HERE */
		if((rnReg & 7) >= FISC_CPU_MODE_USER && (rnReg & 7) <= FISC_CPU_MODE_EXCEPTION) {
			*psrRegValPtr = rnReg;
		} else {
			/* This CPU mode is invalid! Entering Undefined CPU mode and triggering PERMFAULT exception */
			psrPtr->mode = FISC_CPU_MODE_UNDEFINED;
			_cpu_->writeRegister(regIndex, *psrRegValPtr, false, 0, 0, 0);
			return _cpu_->triggerSoftException(EXC_PERMFAULT);
		}
		break;
	}
	case 1:
		psrPtr->c =  rnReg & 1;
		psrPtr->v = (rnReg & 2) >> 1;
		psrPtr->z = (rnReg & 4) >> 2;
		psrPtr->n = (rnReg & 8) >> 3;
		break;
	case 2:  psrPtr->n   = rnReg & 1; break;
	case 3:  psrPtr->z   = rnReg & 1; break;
	case 4:  psrPtr->v   = rnReg & 1; break;
	case 5:  psrPtr->c   = rnReg & 1; break;
	case 6:  psrPtr->ae  = rnReg & 3; break;
	case 7:  BIT_WRITE(rnReg & 1, psrPtr->ae, 0); break;
	case 8:  BIT_WRITE(rnReg & 1, psrPtr->ae, 1); break;
	case 9:  psrPtr->pg  = rnReg & 1; break;
	case 10: psrPtr->ien = rnReg & 3; break;
	case 11: BIT_WRITE(rnReg & 1, psrPtr->ien, 0); break;
	case 12: BIT_WRITE(rnReg & 1, psrPtr->ien, 1); break;
	case 13: {
		/* ALERT: WE'RE CHANGING CPU MODE HERE */
		if ((rnReg & 7) >= FISC_CPU_MODE_USER && (rnReg & 7) <= FISC_CPU_MODE_EXCEPTION) {
			psrPtr->mode = rnReg & 7;
		}
		else {
			/* This CPU mode is invalid! Entering Undefined CPU mode and triggering PERMFAULT exception */
			psrPtr->mode = FISC_CPU_MODE_UNDEFINED;
			_cpu_->writeRegister(regIndex, *psrRegValPtr, false, 0, 0, 0);
			return _cpu_->triggerSoftException(EXC_PERMFAULT);
		}
		break;
	}
	default: /* Invalid CPSR/SPSR field */ return FISC_RET_ERROR;
	}

	/* 9- Finally, write to the CPSR/SPSR register */
	return _cpu_->writeRegister(regIndex, *psrRegValPtr, false, 0, 0, 0);
});

NEW_INSTRUCTION(FISC, MRS, RF, /* Operation: R[Rd] = CPSR/SPSR_x? (Note: the _x field is in R[Rn])*/
{
	/* 1-This variable will hold the resultant value which will be stored in the register R[Rd] */
	uint64_t rdReg = (uint64_t)-1;

	/* 2- Get the CPSR/SPSR field to which we will be reading from */
	uint16_t cpsr_field = _cpu_->readRegister(_this_->ifmt_r->rn) & 0x1F;

	/* 3- Read CPSR (and cast it into a pointer for ease of use) */
	uint64_t cpsrRegVal = _cpu_->readRegister(SPECIAL_CPSR);
	
	/* 4- Read SPSR (and cast it into a pointer for ease of use) */
	uint64_t spsrRegVal = _cpu_->readRegister(SPECIAL_SPSR0 + (cpsrRegVal & 7));
	
	/* 5- Now we will use 1 single pointer to write either to CPSR or SPSR */
	uint64_t * psrRegValPtr = nullptr;
	cpsr_t * psrPtr = nullptr;

	enum SPECIAL_REGISTERS regIndex; /* Index of the destination register */

	bool cpsr_or_spsr = cpsr_field & 16 ? true : false; /* 0: Write to CPSR. 1: Write to SPSR */

	if (!cpsr_or_spsr) {
		psrRegValPtr = &cpsrRegVal; /* Point to the CPSR */
		regIndex = SPECIAL_CPSR;
	}
	else {
		psrRegValPtr = &spsrRegVal; /* Point to the current SPSR instead of the CPSR */
		regIndex = (enum SPECIAL_REGISTERS)(SPECIAL_SPSR0 + (cpsrRegVal & 7));
	}

	/* 6- Point the cpsr_t pointer to the selected integer pointer */
	psrPtr = (cpsr_t*)psrRegValPtr;

	/* 7- Check if the current CPU mode has permissions to read from a particular field in the CPSR */
	if (psrPtr->mode == FISC_CPU_MODE_USER) {
		switch (cpsr_field) {
		case 0: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13:
			/* The user does not have permission to read these selected CPSR fields! */
			return _cpu_->triggerSoftException(EXC_PERMFAULT);
		}
	}

	/* 8- We have permission. We shall now read from the CPSR register */
	switch (cpsr_field) {
	case 0: rdReg = *psrRegValPtr; break;
	case 1: 
		rdReg |= psrPtr->c;
		rdReg |= psrPtr->v << 1;
		rdReg |= psrPtr->z << 2;
		rdReg |= psrPtr->n << 3;
		break;
	case 2:  rdReg = psrPtr->n;       break;
	case 3:  rdReg = psrPtr->z;       break;
	case 4:  rdReg = psrPtr->v;       break;
	case 5:  rdReg = psrPtr->c;       break;
	case 6:  rdReg = psrPtr->ae;      break;
	case 7:  rdReg = psrPtr->ae & 1;  break;
	case 8:  rdReg = (psrPtr->ae & 2) >> 1; break;
	case 9:  rdReg = psrPtr->pg;      break;
	case 10: rdReg = psrPtr->ien;     break;
	case 11: rdReg = psrPtr->ien & 1; break;
	case 12: rdReg = (psrPtr->ien & 2) >> 1; break;
	case 13: rdReg = psrPtr->mode;    break;
	default: /* Invalid CPSR field */ return FISC_RET_ERROR;
	}

	/* 9- Finally, write to the destination General Purpose register */
	return _cpu_->writeRegister(_this_->ifmt_r->rd, rdReg, false, 0, 0, 0);
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

NEW_INSTRUCTION(FISC, LPFLA, RF, /* Operation: R[Rd] = PDP */
{
	return _cpu_->writeRegister(_this_->ifmt_r->rd, _cpu_->readRegister(SPECIAL_PFLA), false, 0, 0, 0);
});

#endif