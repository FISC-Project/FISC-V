#ifndef FISC_DATA_H_
#define FISC_DATA_H_

#include "../FISCCPUModule.h"
#include "FISCISA.h"

#define ENABLE_LOAD_MEMORYACCESS_DEBUGGING  true
#define ENABLE_STORE_MEMORYACCESS_DEBUGGING true

/**************************************************************/
/***************** MOVZ AND MOVK INSTRUCTIONS *****************/
/**************************************************************/
NEW_INSTRUCTION(FISC, MOVZ, IWF, /* Operation: R[Rt](quadrant) = MOVImm (Clears register first) */
{
    switch (_this_->ifmt_iw->quadrant) {
    case 0:
        return _cpu_->writeRegister(_this_->ifmt_iw->rt, 
                                    _this_->ifmt_iw->mov_immediate & 0xFFFF, 
                                    false, 0,0,0);
    case 1:
        return _cpu_->writeRegister(_this_->ifmt_iw->rt, 
                                   (uint64_t)(_this_->ifmt_iw->mov_immediate & 0xFFFF) << 16, 
                                    false, 0,0,0);
    case 2:
        return _cpu_->writeRegister(_this_->ifmt_iw->rt,
                                   (uint64_t)(_this_->ifmt_iw->mov_immediate & 0xFFFF) << 32,
                                    false, 0, 0, 0);
    case 3:
        return _cpu_->writeRegister(_this_->ifmt_iw->rt, 
                                   (uint64_t)(_this_->ifmt_iw->mov_immediate & 0xFFFF) << 48, 
                                    false, 0,0,0);
    }
    return FISC_RET_ERROR;
});

NEW_INSTRUCTION(FISC, MOVK, IWF, /* Operation: R[Rt](quadrant) = MOVImm */
{
    uint64_t dstRegVal = _cpu_->readRegister(_this_->ifmt_iw->rt);

    switch (_this_->ifmt_iw->quadrant) {
        case 0:
            dstRegVal &= 0xFFFFFFFFFFFF0000;
            dstRegVal |= _this_->ifmt_iw->mov_immediate & 0xFFFF;
            break;
        case 1:
            dstRegVal &= 0xFFFFFFFF0000FFFF;
            dstRegVal |= (_this_->ifmt_iw->mov_immediate & 0xFFFF) << 16;
            break;
        case 2:
            dstRegVal &= 0xFFFF0000FFFFFFFF;
            dstRegVal |= (uint64_t)((_this_->ifmt_iw->mov_immediate & 0xFFFF)) << 32;
            break;
        case 3:
            dstRegVal &= 0x0000FFFFFFFFFFFF;
            dstRegVal |= (uint64_t)((_this_->ifmt_iw->mov_immediate & 0xFFFF)) << 48;
            break;
        default: return FISC_RET_ERROR;
    }

    return _cpu_->writeRegister(_this_->ifmt_iw->rt, dstRegVal, false, 0, 0, 0);
});

NEW_INSTRUCTION(FISC, MOVRZ, IWF, /* Operation: R[Rt](quadrant) = PC + MOVImm (Clears register first) */
{
    uint64_t pcVal = _cpu_->readRegister(SPECIAL_PC);

    switch (_this_->ifmt_iw->quadrant) {
    case 0:
        return _cpu_->writeRegister(_this_->ifmt_iw->rt, 
                                    (pcVal & 0xFFFF) + (_this_->ifmt_iw->mov_immediate & 0xFFFF),
                                    false, 0,0,0);
    case 1:
        return _cpu_->writeRegister(_this_->ifmt_iw->rt, 
                                    ((pcVal & 0xFFFF) << 16) + ((uint64_t)(_this_->ifmt_iw->mov_immediate & 0xFFFF) << 16),
                                    false, 0,0,0);
    case 2:
        return _cpu_->writeRegister(_this_->ifmt_iw->rt,
                                    ((uint64_t)(_this_->ifmt_iw->mov_immediate & 0xFFFF) << 32),
                                    false, 0, 0, 0);
    case 3:
        return _cpu_->writeRegister(_this_->ifmt_iw->rt, 
                                    ((uint64_t)(_this_->ifmt_iw->mov_immediate & 0xFFFF) << 48),
                                    false, 0,0,0);
    }
    return FISC_RET_ERROR;
});

NEW_INSTRUCTION(FISC, MOVRK, IWF, /* Operation: R[Rt](quadrant) = PC + MOVImm */
{
    uint64_t dstRegVal = _cpu_->readRegister(_this_->ifmt_iw->rt);
    uint64_t pcVal = _cpu_->readRegister(SPECIAL_PC);

    switch (_this_->ifmt_iw->quadrant) {
        case 0:
            dstRegVal &= 0xFFFFFFFFFFFF0000;
            dstRegVal |= (pcVal & 0xFFFF) + (_this_->ifmt_iw->mov_immediate & 0xFFFF);
            break;
        case 1:
            dstRegVal &= 0xFFFFFFFF0000FFFF;
            dstRegVal |= ((pcVal & 0xFFFF) << 16) + ((_this_->ifmt_iw->mov_immediate & 0xFFFF) << 16);
            break;
        case 2:
            dstRegVal &= 0xFFFF0000FFFFFFFF;
            dstRegVal |= ((uint64_t)((_this_->ifmt_iw->mov_immediate & 0xFFFF)) << 32);
            break;
        case 3:
            dstRegVal &= 0x0000FFFFFFFFFFFF;
            dstRegVal |= ((uint64_t)((_this_->ifmt_iw->mov_immediate & 0xFFFF)) << 48);
            break;
        default: return FISC_RET_ERROR;
    }

    return _cpu_->writeRegister(_this_->ifmt_iw->rt, dstRegVal, false, 0, 0, 0);
});

/**************************************************************/
/********************** LDPC INSTRUCTION **********************/
/**************************************************************/
NEW_INSTRUCTION(FISC, LDPC, RF, /* Operation: R[LR] = PC */
{
    return _cpu_->writeRegister(LR, _cpu_->readRegister(SPECIAL_PC), false, 0, 0, 0);
});

/**************************************************************/
/********************* LOAD INSTRUCTIONS **********************/
/**************************************************************/
NEW_INSTRUCTION(FISC, LDR, DF, /* Operation: R[Rt] = M[R[Rn] + DTAddr] (64 bits wide) */
{
    uint64_t base = _cpu_->readRegister(_this_->ifmt_d->rn);
    int64_t offset = _this_->ifmt_d->dt_address;
    if(offset & (1<<(9-1))) offset = -((~offset + 1) & 0x1FF); /* Fix non 64-bit number signedness */
    
    /* Align (or not) the base and unscaled offset */
    uint64_t cpsrVal = _cpu_->readRegister(SPECIAL_CPSR);
    if(((cpsr_t*)&cpsrVal)->ae & 1)
        base = ALIGN_BASE(base, _this_->ifmt_d->op);
    if(((cpsr_t*)&cpsrVal)->ae & 2)
        offset = ALIGN_DTADDR(offset, _this_->ifmt_d->op);

    /* Read memory contents (M[R[Rn] + DTAddr]) */
    uint64_t memVal = _cpu_->mmu_read((uint32_t)(base + offset), (enum FISC_DATATYPE)(_this_->ifmt_d->op + 1), false, ENABLE_LOAD_MEMORYACCESS_DEBUGGING);

    /* Load those contents into the register (R[Rt] = MemVal) */
    return _cpu_->writeRegister(_this_->ifmt_d->rt, memVal, false, 0, 0, 0);
});

NEW_INSTRUCTION(FISC, LDRB, DF, /* Operation: R[Rt] = M[R[Rn] + DTAddr] (8 bits wide) */
{
    uint64_t base = _cpu_->readRegister(_this_->ifmt_d->rn);
    int64_t offset = _this_->ifmt_d->dt_address;
    if(offset & (1<<(9-1))) offset = -((~offset + 1) & 0x1FF); /* Fix non 64-bit number signedness */
    
    /* Align (or not) the base and unscaled offset */
    uint64_t cpsrVal = _cpu_->readRegister(SPECIAL_CPSR);
    if(((cpsr_t*)&cpsrVal)->ae & 1)
        base = ALIGN_BASE(base, _this_->ifmt_d->op);
    if(((cpsr_t*)&cpsrVal)->ae & 2)
        offset = ALIGN_DTADDR(offset, _this_->ifmt_d->op);

    /* Read memory contents (M[R[Rn] + DTAddr]) */
    uint64_t memVal = _cpu_->mmu_read((uint32_t)(base + offset), (enum FISC_DATATYPE)(_this_->ifmt_d->op + 1), false, ENABLE_LOAD_MEMORYACCESS_DEBUGGING);

    /* Load those contents into the register (R[Rt] = MemVal) */
    return _cpu_->writeRegister(_this_->ifmt_d->rt, memVal, false, 0, 0, 0);
});

NEW_INSTRUCTION(FISC, LDRH, DF, /* Operation: R[Rt] = M[R[Rn] + DTAddr] (16 bits wide) */
{
    uint64_t base = _cpu_->readRegister(_this_->ifmt_d->rn);
    int64_t offset = _this_->ifmt_d->dt_address;
    if(offset & (1<<(9-1))) offset = -((~offset + 1) & 0x1FF); /* Fix non 64-bit number signedness */
    
    /* Align (or not) the base and unscaled offset */
    uint64_t cpsrVal = _cpu_->readRegister(SPECIAL_CPSR);
    if(((cpsr_t*)&cpsrVal)->ae & 1)
        base = ALIGN_BASE(base, _this_->ifmt_d->op);
    if(((cpsr_t*)&cpsrVal)->ae & 2)
        offset = ALIGN_DTADDR(offset, _this_->ifmt_d->op);

    /* Read memory contents (M[R[Rn] + DTAddr]) */
    uint64_t memVal = _cpu_->mmu_read((uint32_t)(base + offset), (enum FISC_DATATYPE)(_this_->ifmt_d->op + 1), false, ENABLE_LOAD_MEMORYACCESS_DEBUGGING);

    /* Load those contents into the register (R[Rt] = MemVal) */
    return _cpu_->writeRegister(_this_->ifmt_d->rt, memVal, false, 0, 0, 0);
});

NEW_INSTRUCTION(FISC, LDRSW, DF, /* Operation: R[Rt] = M[R[Rn] + DTAddr] (32 bits wide) */
{
    uint64_t base = _cpu_->readRegister(_this_->ifmt_d->rn);
    int64_t offset = _this_->ifmt_d->dt_address;
    if(offset & (1<<(9-1))) offset = -((~offset + 1) & 0x1FF); /* Fix non 64-bit number signedness */
    
    /* Align (or not) the base and unscaled offset */
    uint64_t cpsrVal = _cpu_->readRegister(SPECIAL_CPSR);
    if(((cpsr_t*)&cpsrVal)->ae & 1)
        base = ALIGN_BASE(base, _this_->ifmt_d->op);
    if(((cpsr_t*)&cpsrVal)->ae & 2)
        offset = ALIGN_DTADDR(offset, _this_->ifmt_d->op);

    /* Read memory contents (M[R[Rn] + DTAddr]) */
    uint64_t memVal = _cpu_->mmu_read((uint32_t)(base + offset), (enum FISC_DATATYPE)(_this_->ifmt_d->op + 1), false, ENABLE_LOAD_MEMORYACCESS_DEBUGGING);

    /* Load those contents into the register (R[Rt] = MemVal) */
    return _cpu_->writeRegister(_this_->ifmt_d->rt, memVal, false, 0, 0, 0);
});

NEW_INSTRUCTION(FISC, LDXR, DF, /* Operation: R[Rt] = M[R[Rn] + DTAddr] (64 bits wide) */
{
    /* TODO: ATOMIC */
    uint64_t base = _cpu_->readRegister(_this_->ifmt_d->rn);
    int64_t offset = _this_->ifmt_d->dt_address;
    if(offset & (1<<(9-1))) offset = -((~offset + 1) & 0x1FF); /* Fix non 64-bit number signedness */
    
    /* Align (or not) the base and unscaled offset */
    uint64_t cpsrVal = _cpu_->readRegister(SPECIAL_CPSR);
    if(((cpsr_t*)&cpsrVal)->ae & 1)
        base = ALIGN_BASE(base, _this_->ifmt_d->op);
    if(((cpsr_t*)&cpsrVal)->ae & 2)
        offset = ALIGN_DTADDR(offset, _this_->ifmt_d->op);

    /* Read memory contents (M[R[Rn] + DTAddr]) */
    uint64_t memVal = _cpu_->mmu_read((uint32_t)(base + offset), (enum FISC_DATATYPE)(_this_->ifmt_d->op + 1), false, ENABLE_LOAD_MEMORYACCESS_DEBUGGING);

    /* Load those contents into the register (R[Rt] = MemVal) */
    return _cpu_->writeRegister(_this_->ifmt_d->rt, memVal, false, 0, 0, 0);
});

NEW_INSTRUCTION(FISC, LDRR, DF, /* Operation: R[Rt] = M[PC + R[Rn] + DTAddr] (64 bits wide) */
{
    uint64_t base = _cpu_->readRegister(_this_->ifmt_d->rn);
    int64_t offset = _this_->ifmt_d->dt_address;
    if(offset & (1<<(9-1))) offset = -((~offset + 1) & 0x1FF); /* Fix non 64-bit number signedness */
    
    /* Align (or not) the base and unscaled offset */
    uint64_t cpsrVal = _cpu_->readRegister(SPECIAL_CPSR);
    if(((cpsr_t*)&cpsrVal)->ae & 1)
        base = ALIGN_BASE(base, _this_->ifmt_d->op);
    if(((cpsr_t*)&cpsrVal)->ae & 2)
        offset = ALIGN_DTADDR(offset, _this_->ifmt_d->op);
    
    /* Add the PC value into the offset */
    offset += _cpu_->readRegister(SPECIAL_PC);

    /* Read memory contents (M[PC + R[Rn] + DTAddr]) */
    uint64_t memVal = _cpu_->mmu_read((uint32_t)(base + offset), (enum FISC_DATATYPE)(_this_->ifmt_d->op + 1), false, ENABLE_LOAD_MEMORYACCESS_DEBUGGING);

    /* Load those contents into the register (R[Rt] = MemVal) */
    return _cpu_->writeRegister(_this_->ifmt_d->rt, memVal, false, 0, 0, 0);
});

NEW_INSTRUCTION(FISC, LDRBR, DF, /* Operation: R[Rt] = M[PC + R[Rn] + DTAddr] (8 bits wide) */
{
    uint64_t base = _cpu_->readRegister(_this_->ifmt_d->rn);
    int64_t offset = _this_->ifmt_d->dt_address;
    if(offset & (1<<(9-1))) offset = -((~offset + 1) & 0x1FF); /* Fix non 64-bit number signedness */
    
    /* Align (or not) the base and unscaled offset */
    uint64_t cpsrVal = _cpu_->readRegister(SPECIAL_CPSR);
    if(((cpsr_t*)&cpsrVal)->ae & 1)
        base = ALIGN_BASE(base, _this_->ifmt_d->op);
    if(((cpsr_t*)&cpsrVal)->ae & 2)
        offset = ALIGN_DTADDR(offset, _this_->ifmt_d->op);
    
    /* Add the PC value into the offset */
    offset += _cpu_->readRegister(SPECIAL_PC);

    /* Read memory contents (M[PC + R[Rn] + DTAddr]) */
    uint64_t memVal = _cpu_->mmu_read((uint32_t)(base + offset), (enum FISC_DATATYPE)(_this_->ifmt_d->op + 1), false, ENABLE_LOAD_MEMORYACCESS_DEBUGGING);

    /* Load those contents into the register (R[Rt] = MemVal) */
    return _cpu_->writeRegister(_this_->ifmt_d->rt, memVal, false, 0, 0, 0);
});

NEW_INSTRUCTION(FISC, LDRHR, DF, /* Operation: R[Rt] = M[PC + R[Rn] + DTAddr] (16 bits wide) */
{
    uint64_t base = _cpu_->readRegister(_this_->ifmt_d->rn);
    int64_t offset = _this_->ifmt_d->dt_address;
    if(offset & (1<<(9-1))) offset = -((~offset + 1) & 0x1FF); /* Fix non 64-bit number signedness */
    
    /* Align (or not) the base and unscaled offset */
    uint64_t cpsrVal = _cpu_->readRegister(SPECIAL_CPSR);
    if(((cpsr_t*)&cpsrVal)->ae & 1)
        base = ALIGN_BASE(base, _this_->ifmt_d->op);
    if(((cpsr_t*)&cpsrVal)->ae & 2)
        offset = ALIGN_DTADDR(offset, _this_->ifmt_d->op);
    
    /* Add the PC value into the offset */
    offset += _cpu_->readRegister(SPECIAL_PC);

    /* Read memory contents (M[PC + R[Rn] + DTAddr]) */
    uint64_t memVal = _cpu_->mmu_read((uint32_t)(base + offset), (enum FISC_DATATYPE)(_this_->ifmt_d->op + 1), false, ENABLE_LOAD_MEMORYACCESS_DEBUGGING);

    /* Load those contents into the register (R[Rt] = MemVal) */
    return _cpu_->writeRegister(_this_->ifmt_d->rt, memVal, false, 0, 0, 0);
});

NEW_INSTRUCTION(FISC, LDRSWR, DF, /* Operation: R[Rt] = M[PC + R[Rn] + DTAddr] (32 bits wide) */
{
    uint64_t base = _cpu_->readRegister(_this_->ifmt_d->rn);
    int64_t offset = _this_->ifmt_d->dt_address;
    if(offset & (1<<(9-1))) offset = -((~offset + 1) & 0x1FF); /* Fix non 64-bit number signedness */
    
    /* Align (or not) the base and unscaled offset */
    uint64_t cpsrVal = _cpu_->readRegister(SPECIAL_CPSR);
    if(((cpsr_t*)&cpsrVal)->ae & 1)
        base = ALIGN_BASE(base, _this_->ifmt_d->op);
    if(((cpsr_t*)&cpsrVal)->ae & 2)
        offset = ALIGN_DTADDR(offset, _this_->ifmt_d->op);
    
    /* Add the PC value into the offset */
    offset += _cpu_->readRegister(SPECIAL_PC);

    /* Read memory contents (M[PC + R[Rn] + DTAddr]) */
    uint64_t memVal = _cpu_->mmu_read((uint32_t)(base + offset), (enum FISC_DATATYPE)(_this_->ifmt_d->op + 1), false, ENABLE_LOAD_MEMORYACCESS_DEBUGGING);

    /* Load those contents into the register (R[Rt] = MemVal) */
    return _cpu_->writeRegister(_this_->ifmt_d->rt, memVal, false, 0, 0, 0);
});

NEW_INSTRUCTION(FISC, LDXRR, DF, /* Operation: R[Rt] = M[PC + R[Rn] + DTAddr] (64 bits wide) */
{
    /* TODO: ATOMIC */
    uint64_t base = _cpu_->readRegister(_this_->ifmt_d->rn);
    int64_t offset = _this_->ifmt_d->dt_address;
    if(offset & (1<<(9-1))) offset = -((~offset + 1) & 0x1FF); /* Fix non 64-bit number signedness */
    
    /* Align (or not) the base and unscaled offset */
    uint64_t cpsrVal = _cpu_->readRegister(SPECIAL_CPSR);
    if(((cpsr_t*)&cpsrVal)->ae & 1)
        base = ALIGN_BASE(base, _this_->ifmt_d->op);
    if(((cpsr_t*)&cpsrVal)->ae & 2)
        offset = ALIGN_DTADDR(offset, _this_->ifmt_d->op);
    
    /* Add the PC value into the offset */
    offset += _cpu_->readRegister(SPECIAL_PC);

    /* Read memory contents (M[PC + R[Rn] + DTAddr]) */
    uint64_t memVal = _cpu_->mmu_read((uint32_t)(base + offset), (enum FISC_DATATYPE)(_this_->ifmt_d->op + 1), false, ENABLE_LOAD_MEMORYACCESS_DEBUGGING);

    /* Load those contents into the register (R[Rt] = MemVal) */
    return _cpu_->writeRegister(_this_->ifmt_d->rt, memVal, false, 0, 0, 0);
});

/**************************************************************/
/********************* STORE INSTRUCTIONS *********************/
/**************************************************************/
NEW_INSTRUCTION(FISC, STR_, DF, /* Operation: M[R[Rn] + DTAddr] = R[Rt] (64 bits wide) */
{
    uint64_t base = _cpu_->readRegister(_this_->ifmt_d->rn);
    int64_t offset = _this_->ifmt_d->dt_address;
    if(offset & (1<<(9-1))) offset = -((~offset + 1) & 0x1FF); /* Fix non 64-bit number signedness */
    
    /* Align (or not) the base and unscaled offset */
    uint64_t cpsrVal = _cpu_->readRegister(SPECIAL_CPSR);
    if(((cpsr_t*)&cpsrVal)->ae & 1)
        base = ALIGN_BASE(base, _this_->ifmt_d->op);
    if(((cpsr_t*)&cpsrVal)->ae & 2)
        offset = ALIGN_DTADDR(offset, _this_->ifmt_d->op);
    
    /* Read register value (R[Rt]) */
    uint64_t regVal = _cpu_->readRegister(_this_->ifmt_d->rt);

    /* Write register value into the memory (M[R[Rn] + DTAddr] = R[Rt]) */
    return _cpu_->mmu_write(regVal, (uint32_t)(base + offset), (enum FISC_DATATYPE)(_this_->ifmt_d->op + 1), false, ENABLE_STORE_MEMORYACCESS_DEBUGGING);
});

NEW_INSTRUCTION(FISC, STRB, DF, /* Operation: M[R[Rn] + DTAddr] = R[Rt] (8 bits wide) */
{
    uint64_t base = _cpu_->readRegister(_this_->ifmt_d->rn);
    int64_t offset = _this_->ifmt_d->dt_address;
    if(offset & (1<<(9-1))) offset = -((~offset + 1) & 0x1FF); /* Fix non 64-bit number signedness */
    
    /* Align (or not) the base and unscaled offset */
    uint64_t cpsrVal = _cpu_->readRegister(SPECIAL_CPSR);
    if(((cpsr_t*)&cpsrVal)->ae & 1)
        base = ALIGN_BASE(base, _this_->ifmt_d->op);
    if(((cpsr_t*)&cpsrVal)->ae & 2)
        offset = ALIGN_DTADDR(offset, _this_->ifmt_d->op);
    
    /* Read register value (R[Rt]) */
    uint64_t regVal = _cpu_->readRegister(_this_->ifmt_d->rt);

    /* Write register value into the memory (M[R[Rn] + DTAddr] = R[Rt]) */
    return _cpu_->mmu_write(regVal, (uint32_t)(base + offset), (enum FISC_DATATYPE)(_this_->ifmt_d->op + 1), false, ENABLE_STORE_MEMORYACCESS_DEBUGGING);
});

NEW_INSTRUCTION(FISC, STRH, DF, /* Operation: M[R[Rn] + DTAddr] = R[Rt] (16 bits wide) */
{
    uint64_t base = _cpu_->readRegister(_this_->ifmt_d->rn);
    int64_t offset = _this_->ifmt_d->dt_address;
    if(offset & (1<<(9-1))) offset = -((~offset + 1) & 0x1FF); /* Fix non 64-bit number signedness */
    
    /* Align (or not) the base and unscaled offset */
    uint64_t cpsrVal = _cpu_->readRegister(SPECIAL_CPSR);
    if(((cpsr_t*)&cpsrVal)->ae & 1)
        base = ALIGN_BASE(base, _this_->ifmt_d->op);
    if(((cpsr_t*)&cpsrVal)->ae & 2)
        offset = ALIGN_DTADDR(offset, _this_->ifmt_d->op);
    
    /* Read register value (R[Rt]) */
    uint64_t regVal = _cpu_->readRegister(_this_->ifmt_d->rt);

    /* Write register value into the memory (M[R[Rn] + DTAddr] = R[Rt]) */
    return _cpu_->mmu_write(regVal, (uint32_t)(base + offset), (enum FISC_DATATYPE)(_this_->ifmt_d->op + 1), false, ENABLE_STORE_MEMORYACCESS_DEBUGGING);
});

NEW_INSTRUCTION(FISC, STRW, DF, /* Operation: M[R[Rn] + DTAddr] = R[Rt] (32 bits wide) */
{
    uint64_t base = _cpu_->readRegister(_this_->ifmt_d->rn);
    int64_t offset = _this_->ifmt_d->dt_address;
    if(offset & (1<<(9-1))) offset = -((~offset + 1) & 0x1FF); /* Fix non 64-bit number signedness */
    
    /* Align (or not) the base and unscaled offset */
    uint64_t cpsrVal = _cpu_->readRegister(SPECIAL_CPSR);
    if(((cpsr_t*)&cpsrVal)->ae & 1)
        base = ALIGN_BASE(base, _this_->ifmt_d->op);
    if(((cpsr_t*)&cpsrVal)->ae & 2)
        offset = ALIGN_DTADDR(offset, _this_->ifmt_d->op);
    
    /* Read register value (R[Rt]) */
    uint64_t regVal = _cpu_->readRegister(_this_->ifmt_d->rt);

    /* Write register value into the memory (M[R[Rn] + DTAddr] = R[Rt]) */
    return _cpu_->mmu_write(regVal, (uint32_t)(base + offset), (enum FISC_DATATYPE)(_this_->ifmt_d->op + 1), false, ENABLE_STORE_MEMORYACCESS_DEBUGGING);
});

NEW_INSTRUCTION(FISC, STXR, DF, /* Operation: M[R[Rn] + DTAddr] = R[Rt] (64 bits wide) */
{
    /* TODO: ATOMIC */
    uint64_t base = _cpu_->readRegister(_this_->ifmt_d->rn);
    int64_t offset = _this_->ifmt_d->dt_address;
    if(offset & (1<<(9-1))) offset = -((~offset + 1) & 0x1FF); /* Fix non 64-bit number signedness */
    
    /* Align (or not) the base and unscaled offset */
    uint64_t cpsrVal = _cpu_->readRegister(SPECIAL_CPSR);
    if(((cpsr_t*)&cpsrVal)->ae & 1)
        base = ALIGN_BASE(base, _this_->ifmt_d->op);
    if(((cpsr_t*)&cpsrVal)->ae & 2)
        offset = ALIGN_DTADDR(offset, _this_->ifmt_d->op);
    
    /* Read register value (R[Rt]) */
    uint64_t regVal = _cpu_->readRegister(_this_->ifmt_d->rt);

    /* Write register value into the memory (M[R[Rn] + DTAddr] = R[Rt]) */
    return _cpu_->mmu_write(regVal, (uint32_t)(base + offset), (enum FISC_DATATYPE)(_this_->ifmt_d->op + 1), false, ENABLE_STORE_MEMORYACCESS_DEBUGGING);
});

NEW_INSTRUCTION(FISC, STRR, DF, /* Operation: M[PC + R[Rn] + DTAddr] = R[Rt] (64 bits wide) */
{
    uint64_t base = _cpu_->readRegister(_this_->ifmt_d->rn);
    int64_t offset = _this_->ifmt_d->dt_address;
    if(offset & (1<<(9-1))) offset = -((~offset + 1) & 0x1FF); /* Fix non 64-bit number signedness */
    
    /* Align (or not) the base and unscaled offset */
    uint64_t cpsrVal = _cpu_->readRegister(SPECIAL_CPSR);
    if(((cpsr_t*)&cpsrVal)->ae & 1)
        base = ALIGN_BASE(base, _this_->ifmt_d->op);
    if(((cpsr_t*)&cpsrVal)->ae & 2)
        offset = ALIGN_DTADDR(offset, _this_->ifmt_d->op);

    /* Add the PC value into the offset */
    offset += _cpu_->readRegister(SPECIAL_PC);
    
    /* Read register value (R[Rt]) */
    uint64_t regVal = _cpu_->readRegister(_this_->ifmt_d->rt);

    /* Write register value into the memory (M[R[Rn] + DTAddr] = R[Rt]) */
    return _cpu_->mmu_write(regVal, (uint32_t)(base + offset), (enum FISC_DATATYPE)(_this_->ifmt_d->op + 1), false, ENABLE_STORE_MEMORYACCESS_DEBUGGING);
});

NEW_INSTRUCTION(FISC, STRBR, DF,  /* Operation: M[PC + R[Rn] + DTAddr] = R[Rt] (8 bits wide) */
{
    uint64_t base = _cpu_->readRegister(_this_->ifmt_d->rn);
    int64_t offset = _this_->ifmt_d->dt_address;
    if(offset & (1<<(9-1))) offset = -((~offset + 1) & 0x1FF); /* Fix non 64-bit number signedness */
    
    /* Align (or not) the base and unscaled offset */
    uint64_t cpsrVal = _cpu_->readRegister(SPECIAL_CPSR);
    if(((cpsr_t*)&cpsrVal)->ae & 1)
        base = ALIGN_BASE(base, _this_->ifmt_d->op);
    if(((cpsr_t*)&cpsrVal)->ae & 2)
        offset = ALIGN_DTADDR(offset, _this_->ifmt_d->op);

    /* Add the PC value into the offset */
    offset += _cpu_->readRegister(SPECIAL_PC);
    
    /* Read register value (R[Rt]) */
    uint64_t regVal = _cpu_->readRegister(_this_->ifmt_d->rt);

    /* Write register value into the memory (M[R[Rn] + DTAddr] = R[Rt]) */
    return _cpu_->mmu_write(regVal, (uint32_t)(base + offset), (enum FISC_DATATYPE)(_this_->ifmt_d->op + 1), false, ENABLE_STORE_MEMORYACCESS_DEBUGGING);
});

NEW_INSTRUCTION(FISC, STRHR, DF,  /* Operation: M[PC + R[Rn] + DTAddr] = R[Rt] (16 bits wide) */
{
    uint64_t base = _cpu_->readRegister(_this_->ifmt_d->rn);
    int64_t offset = _this_->ifmt_d->dt_address;
    if(offset & (1<<(9-1))) offset = -((~offset + 1) & 0x1FF); /* Fix non 64-bit number signedness */
    
    /* Align (or not) the base and unscaled offset */
    uint64_t cpsrVal = _cpu_->readRegister(SPECIAL_CPSR);
    if(((cpsr_t*)&cpsrVal)->ae & 1)
        base = ALIGN_BASE(base, _this_->ifmt_d->op);
    if(((cpsr_t*)&cpsrVal)->ae & 2)
        offset = ALIGN_DTADDR(offset, _this_->ifmt_d->op);

    /* Add the PC value into the offset */
    offset += _cpu_->readRegister(SPECIAL_PC);
    
    /* Read register value (R[Rt]) */
    uint64_t regVal = _cpu_->readRegister(_this_->ifmt_d->rt);

    /* Write register value into the memory (M[R[Rn] + DTAddr] = R[Rt]) */
    return _cpu_->mmu_write(regVal, (uint32_t)(base + offset), (enum FISC_DATATYPE)(_this_->ifmt_d->op + 1), false, ENABLE_STORE_MEMORYACCESS_DEBUGGING);
});

NEW_INSTRUCTION(FISC, STRWR, DF,  /* Operation: M[PC + R[Rn] + DTAddr] = R[Rt] (32 bits wide) */
{
    uint64_t base = _cpu_->readRegister(_this_->ifmt_d->rn);
    int64_t offset = _this_->ifmt_d->dt_address;
    if(offset & (1<<(9-1))) offset = -((~offset + 1) & 0x1FF); /* Fix non 64-bit number signedness */
    
    /* Align (or not) the base and unscaled offset */
    uint64_t cpsrVal = _cpu_->readRegister(SPECIAL_CPSR);
    if(((cpsr_t*)&cpsrVal)->ae & 1)
        base = ALIGN_BASE(base, _this_->ifmt_d->op);
    if(((cpsr_t*)&cpsrVal)->ae & 2)
        offset = ALIGN_DTADDR(offset, _this_->ifmt_d->op);

    /* Add the PC value into the offset */
    offset += _cpu_->readRegister(SPECIAL_PC);
    
    /* Read register value (R[Rt]) */
    uint64_t regVal = _cpu_->readRegister(_this_->ifmt_d->rt);

    /* Write register value into the memory (M[R[Rn] + DTAddr] = R[Rt]) */
    return _cpu_->mmu_write(regVal, (uint32_t)(base + offset), (enum FISC_DATATYPE)(_this_->ifmt_d->op + 1), false, ENABLE_STORE_MEMORYACCESS_DEBUGGING);
});

NEW_INSTRUCTION(FISC, STXRR, DF, /* Operation: M[PC + R[Rn] + DTAddr] = R[Rt] (64 bits wide) */
{
    /* TODO: ATOMIC */
    uint64_t base = _cpu_->readRegister(_this_->ifmt_d->rn);
    int64_t offset = _this_->ifmt_d->dt_address;
    if(offset & (1<<(9-1))) offset = -((~offset + 1) & 0x1FF); /* Fix non 64-bit number signedness */
    
    /* Align (or not) the base and unscaled offset */
    uint64_t cpsrVal = _cpu_->readRegister(SPECIAL_CPSR);
    if(((cpsr_t*)&cpsrVal)->ae & 1)
        base = ALIGN_BASE(base, _this_->ifmt_d->op);
    if(((cpsr_t*)&cpsrVal)->ae & 2)
        offset = ALIGN_DTADDR(offset, _this_->ifmt_d->op);

    /* Add the PC value into the offset */
    offset += _cpu_->readRegister(SPECIAL_PC);
    
    /* Read register value (R[Rt]) */
    uint64_t regVal = _cpu_->readRegister(_this_->ifmt_d->rt);

    /* Write register value into the memory (M[R[Rn] + DTAddr] = R[Rt]) */
    return _cpu_->mmu_write(regVal, (uint32_t)(base + offset), (enum FISC_DATATYPE)(_this_->ifmt_d->op + 1), false, ENABLE_STORE_MEMORYACCESS_DEBUGGING);
});

#endif
