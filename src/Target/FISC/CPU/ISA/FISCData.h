#ifndef FISC_DATA_H_
#define FISC_DATA_H_

#include "../FISCCPUModule.h"
#include "FISCISA.h"

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
