/*                                                                       
          __  __           _       _                     
         |  \/  |         | |     | |     _              
         | \  / | ___   __| |_   _| | ___(_)             
         | |\/| |/ _ \ / _` | | | | |/ _ \               
         | |  | | (_) | (_| | |_| | |  __/_              
         |_|  |_|\___/ \__,_|\__,_|_|\___(_)             
   _____ _____  _    _   __  __           _       _      
  / ____|  __ \| |  | | |  \/  |         | |     | |     
 | |    | |__) | |  | | | \  / | ___   __| |_   _| | ___ 
 | |    |  ___/| |  | | | |\/| |/ _ \ / _` | | | | |/ _ \
 | |____| |    | |__| | | |  | | (_) | (_| | |_| | |  __/
  \_____|_|     \____/  |_|  |_|\___/ \__,_|\__,_|_|\___|
                                                             
*/

/*------------------------------------------------------------------------
- FILE NAME: FISCCPUModule.cpp
- MODULE NAME: CPU Module
- PURPOSE: To define the behaviour of the Central Processing Unit - CPU
- AUTHOR: MIGUEL SANTOS
-------------------------------------------------------------------------*/

#pragma once
#include <fvm/Pass.h>
#include "FISCCPUConfigurator.cpp"
#include "../Memory/FISCMemoryModule.cpp"
#include "FISCCPUModule.h"
#include <iomanip>

namespace FISC {

uint64_t CPUModule::readRegister(unsigned registerIndex)
{
    if(registerIndex < FISC_REGISTER_COUNT) {
        if(registerIndex == XZR)
            return 0;
        else
            return cconf->x[registerIndex];
    }
    else {
        switch (registerIndex) {
            case SPECIAL_PC:    return cconf->pc;
            case SPECIAL_ESR:   return cconf->esr;
            case SPECIAL_ELR:   return cconf->elr;
            case SPECIAL_CPSR:  return *((uint64_t*)&cconf->cpsr);
            case SPECIAL_SPSR0: return *((uint64_t*)&cconf->spsr[0]);
            case SPECIAL_SPSR1: return *((uint64_t*)&cconf->spsr[1]);
            case SPECIAL_SPSR2: return *((uint64_t*)&cconf->spsr[2]);
            case SPECIAL_SPSR3: return *((uint64_t*)&cconf->spsr[3]);
            case SPECIAL_SPSR4: return *((uint64_t*)&cconf->spsr[4]);
            case SPECIAL_SPSR5: return *((uint64_t*)&cconf->spsr[5]);
            case SPECIAL_IVP:   return cconf->ivp;
            case SPECIAL_EVP:   return cconf->evp;
            case SPECIAL_PDP:   return cconf->pdp;
            case SPECIAL_PFLA:  return cconf->pfla;
            default:            return (uint64_t)-1;
        }
    }
}

enum FISC_RETTYPE CPUModule::writeRegister(unsigned registerIndex,
                                           uint64_t data,
                                           bool setFlags,
                                           uint64_t operand1, uint64_t operand2,
                                           char operation)
{
    if (registerIndex < FISC_REGISTER_COUNT) {
        if(registerIndex != XZR)
            cconf->x[registerIndex] = data;
    }
    else {
        switch (registerIndex) {
            case SPECIAL_PC:    cconf->pc      = (uint32_t)data;  break;
            case SPECIAL_ESR:   cconf->esr     = (uint32_t)data;  break;
            case SPECIAL_ELR:   cconf->elr     = data;            break;
            case SPECIAL_CPSR:  cconf->cpsr    = *(cpsr_t*)&data; break;
            case SPECIAL_SPSR0: cconf->spsr[0] = *(cpsr_t*)&data; break;
            case SPECIAL_SPSR1: cconf->spsr[1] = *(cpsr_t*)&data; break;
            case SPECIAL_SPSR2: cconf->spsr[2] = *(cpsr_t*)&data; break;
            case SPECIAL_SPSR3: cconf->spsr[3] = *(cpsr_t*)&data; break;
            case SPECIAL_SPSR4: cconf->spsr[4] = *(cpsr_t*)&data; break;
            case SPECIAL_SPSR5: cconf->spsr[5] = *(cpsr_t*)&data; break;
            case SPECIAL_IVP:   cconf->ivp     = data;            break;
            case SPECIAL_EVP:   cconf->evp     = data;            break;
            case SPECIAL_PDP:   cconf->pdp     = data;            break;
            case SPECIAL_PFLA:  cconf->pfla    = data;            break;
            default: return FISC_RET_ERROR;
        }
    }

    if(setFlags) {
        cconf->cpsr.n = (((int64_t)data) < 0) ? 1 : 0;
        cconf->cpsr.z = data == 0 ? 1 : 0;
        cconf->cpsr.v = detectOverflow(operand1, operand2, operation) ? 1 : 0;
        cconf->cpsr.c = detectCarry(operand1, operand2, operation) ? 1 : 0;
    }

    return FISC_RET_OK;
}

enum FISC_RETTYPE CPUModule::branch(uint32_t new_addr, bool isPCRel)
{
    enum FISC_RETTYPE success = FISC_RET_ERROR;
    if(isPCRel)
        success = writeRegister(SPECIAL_PC, cconf->pc + new_addr, false, 0, 0, 0);
    else
        success = writeRegister(SPECIAL_PC, new_addr, false, 0, 0, 0);
    
    if(success == FISC_RET_OK)
        isBranching = true;

    return success;
}

bool CPUModule::detectOverflow(uint64_t operand1, uint64_t operand2, char operation)
{
    switch (operation) {
    case '+': return (operand2 > 0) && (operand1 > INT_MAX - operand2);
    case '-': return (operand2 < 0) && (operand1 > INT_MAX + operand2);
    default: /* Unknown/Invalid operation */ return false;
    }
}

bool CPUModule::detectCarry(uint64_t operand1, uint64_t operand2, char operation)
{
    switch (operation) {
    case '+': {
        int64_t res = operand1 + operand2;
        return 1 & (((operand1 & operand2 & ~res) | (~operand1 & ~operand2 & res)) >> 63);
    }
    case '-': {
        int64_t res = operand1 - operand2;
        return 1 & (((operand1 & operand2 & ~res) | (~operand1 & ~operand2 & res)) >> 63);
    }
    default: /* Unknown/Invalid operation */ return false;
    }
}

Instruction * CPUModule::decode(uint32_t instruction)
{
    Instruction * result = nullptr;

    /* At this point, we don't know the width of the opcode.
        Could be 11, 10, 9, 8 or even 6 bits wide. We must
        decode by opcode size manually. */

    /* Try to decode instruction as a 11 bit opcode */
    if((result = cconf->instruction_list[OPCODE_MASK(instruction)]) != nullptr) {
        /* Found it! */
        result->instruction = instruction;
        if(result->format == IFMT_R)
            result->ifmt_r = INSTR_TO_IFMT_R(result->instruction);
        else if(result->format == IFMT_D)
            result->ifmt_d = INSTR_TO_IFMT_D(result->instruction);
        return result;
    }

    /* Try to decode instruction as a 10 bit opcode */
    if ((result = cconf->instruction_list[OPCODE_MASK(instruction) >> 1]) != nullptr) {
        /* Found it! */
        result->instruction = instruction;
        result->ifmt_i = INSTR_TO_IFMT_I(result->instruction);
        return result;
    }

    /* Try to decode instruction as a 9 bit opcode */
    if ((result = cconf->instruction_list[OPCODE_MASK(instruction) >> 2]) != nullptr) {
        /* Found it! */
        result->instruction = instruction;
        result->ifmt_iw = INSTR_TO_IFMT_IW(result->instruction);
        return result;
    }

    /* Try to decode instruction as a 8 bit opcode */
    if ((result = cconf->instruction_list[OPCODE_MASK(instruction) >> 3]) != nullptr) {
        /* Found it! */
        result->instruction = instruction;
        result->ifmt_cb = INSTR_TO_IFMT_CB(result->instruction);
        return result;
    }

    /* Try to decode instruction as a 6 bit opcode */
    if ((result = cconf->instruction_list[OPCODE_MASK(instruction) >> 5]) != nullptr) {
        /* Found it! */
        result->instruction = instruction;
        result->ifmt_b = INSTR_TO_IFMT_B(result->instruction);
        return result;
    }

    /* If we get to this point, then we did not successfully
        decode the instruction ... */
    return nullptr;	
}

std::string CPUModule::disassembleConstant(unsigned val)
{
    return std::to_string(val);
}

std::string CPUModule::disassembleRegister(unsigned registerIndex)
{
    switch (registerIndex) {
        case IP0:           return "IP0";
        case IP1:           return "IP1";
        case SP:            return "SP";
        case FP:            return "FP";
        case LR:            return "LR";
        case XZR:           return "XZR";
        case SPECIAL_PC:    return "PC";
        case SPECIAL_ESR:   return "ESR";
        case SPECIAL_ELR:   return "ELR";
        case SPECIAL_CPSR:  return "CPSR";
        case SPECIAL_SPSR0: return "SPSR0";
        case SPECIAL_SPSR1: return "SPSR1";
        case SPECIAL_SPSR2: return "SPSR2";
        case SPECIAL_SPSR3: return "SPSR3";
        case SPECIAL_SPSR4: return "SPSR4";
        case SPECIAL_SPSR5: return "SPSR5";
        case SPECIAL_IVP:   return "IVP";
        case SPECIAL_EVP:   return "EVP";
        case SPECIAL_PDP:   return "PDP";
        case SPECIAL_PFLA:  return "PFLA";
    }

    if(registerIndex >= 0 && registerIndex < FISC_REGISTER_COUNT)
        return "X" + std::to_string(registerIndex);
    else
        return "XNIL";
}

std::string CPUModule::disassembleBCC(unsigned cc)
{
    switch (cc) {
    case BEQ: return "BEQ";
    case BNE: return "BNE";
    case BLT: return "BLT";
    case BLE: return "BLE";
    case BGT: return "BGT";
    case BGE: return "BGE";
    case BLO: return "BLO";
    case BLS: return "BLS";
    case BHI: return "BHI";
    case BHS: return "BHS";
    case BMI: return "BMI";
    case BPL: return "BPL";
    case BVS: return "BVS";
    case BVC: return "BVC";
    default: return "<NIL>";
    }
}

std::string CPUModule::disassemble(Instruction * instruction)
{
    if(instruction->opcodeStr == "BCOND")
        instruction->opcodeStr = disassembleBCC(instruction->ifmt_cb->rt);

    std::string stringBuild = instruction->opcodeStr + " ";
    switch (instruction->format) {
        case IFMT_R:
            if(instruction->ifmt_r->rd == XZR && instruction->ifmt_r->rn == XZR && instruction->ifmt_r->rm == XZR) {
                stringBuild = "NOP";
            } else {
                stringBuild += disassembleRegister(instruction->ifmt_r->rd);
                if(instruction->opcode != BR)
                    stringBuild += ", " + disassembleRegister(instruction->ifmt_r->rn) + ", " + disassembleRegister(instruction->ifmt_r->rm);
            }
            break;
        case IFMT_I:
            stringBuild += disassembleRegister(instruction->ifmt_i->rd) + ", " + disassembleRegister(instruction->ifmt_i->rn) + ", " + disassembleConstant(instruction->ifmt_i->alu_immediate);
            break;
        case IFMT_D:
            stringBuild += disassembleRegister(instruction->ifmt_d->rt) + ", [" + disassembleRegister(instruction->ifmt_d->rn) + ", " + disassembleConstant(instruction->ifmt_d->dt_address) + "]";
            break;
        case IFMT_B:
            stringBuild += disassembleConstant(instruction->ifmt_b->br_address);
            break;
        case IFMT_CB:
            stringBuild += disassembleConstant(instruction->ifmt_cb->cond_br_address);
            break;
        case IFMT_IW:
            stringBuild += disassembleRegister(instruction->ifmt_iw->rt) + ", " + disassembleConstant(instruction->ifmt_iw->mov_immediate) + ", LSL " + disassembleConstant(instruction->ifmt_iw->quadrant);
            break;
    }

    return stringBuild;
}

std::string CPUModule::getCurrentCPUModeStr()
{
    switch (readRegister(SPECIAL_CPSR) & 0b111) {
    case FISC_CPU_MODE_UNDEFINED: return "UNDEFINED";
    case FISC_CPU_MODE_USER:      return "USER";
    case FISC_CPU_MODE_KERNEL:    return "KERNEL";
    case FISC_CPU_MODE_IRQ:       return "IRQ";
    case FISC_CPU_MODE_SIRQ:      return "SIRQ";
    case FISC_CPU_MODE_EXCEPTION: return "EXCEPTION";
    default:                      return NULLSTR;
    }
}

CPUModule::CPUModule() : RunPass(2)
{
        
}

enum PassRetcode CPUModule::init()
{
    /* Fetch CPU Configurator Pass */
    if (!(cconf = GET_PASS(CPUConfigurator))) {
        /* We were unable to find a CPUConfigurator pass!
            We cannot continue the execution of this pass */
        DEBUG(DERROR, "Could not fetch the CPU Configurator Pass!");
        return PASS_RET_ERR;
    }
        
    /* Fetch Memory Module Pass */
    if (!(memory = GET_PASS(MemoryModule))) {
        /* We were unable to find a MemoryModule pass!
            We cannot continue the execution of this pass */
        DEBUG(DERROR, "Could not fetch the Memory Module Pass!");
        return PASS_RET_ERR;
    }
        
    /* Set up the context for all of the instructions 
        (this should be done in CPUConfigurator, but we
        needed a convenient and quick way to grab the 
        pointer to this class) */
    for (auto it = cconf->instruction_list.begin(); it != cconf->instruction_list.end(); it++)
        it->second->passOwner = this;
        
    /* Initialize Program Counter */
    writeRegister(SPECIAL_PC, 0, false, 0, 0, 0);

    isBranching = false;
    
    /* We're good to go */
    return PASS_RET_OK;
}

enum PassRetcode CPUModule::finit()
{
    DEBUG(DGOOD, "Terminating CPU");
    return PASS_RET_OK;
}

enum PassRetcode CPUModule::run()
{
    DEBUG(DGOOD,"EXECUTING (mode: %s)...\n", getCurrentCPUModeStr().c_str());
    
    /* 
    -- CPU ALGORITHM --
        1-Fetch
        2-Decode
        3-Execute
        4-Memory Access
        5-Writeback

        * Repeat *
    */

    std::string disassembledInstruction = NULLSTR;
    uint32_t instructionsExecuted = 1;
    uint32_t instruction = (uint32_t)-1;
    uint32_t pc_copy = (uint32_t)-1;

    /* On every loop: 1 - Fetch instruction */
    while ((instruction = (uint32_t)memory->read((pc_copy = (uint32_t)readRegister(SPECIAL_PC)), FISC_SZ_32, false)) != (uint32_t)-1)
    {
        /* 2 - Decode instruction */
        Instruction * decodedInstruction = decode(instruction);
        if(decodedInstruction == nullptr || !decodedInstruction->initialized) {
            DEBUG(DERROR, "Unhandled exception: instruction 0x%X (opcode 0x%X, @PC 0x%X) is undefined. Terminating.", instruction, OPCODE_MASK(instruction), pc_copy);
            break;
        }
        disassembledInstruction = disassemble(decodedInstruction); /* Also disassemble instruction while we're at it */
        DEBUG(DINFO, "|%d| @PC 0x%X = 0x%X\t|%d| %s", instructionsExecuted, pc_copy, instruction, decodedInstruction->timesExecuted + 1, disassembledInstruction.c_str());
            
        /* 3, 4 and 5 - Execute instruction, Access Memory and Write back to the registers */
        enum FISC_RETTYPE ret = decodedInstruction->operation(decodedInstruction, decodedInstruction->passOwner);
        instructionsExecuted++;
        decodedInstruction->timesExecuted++;
        if (ret != FISC_RET_OK) {
            if(isDebuggingEnabled()) {
                /* Just for pretty output */
                DEBUG(DNORMALH, "\t\t| ");
                if(ret == FISC_RET_ERROR)
                    PRINTC(DERROR, "ERROR: %s", decodedInstruction->retStr.c_str());
                else if(ret == FISC_RET_INFO)
                    PRINTC(DINFO2, "INFO: %s", decodedInstruction->retStr.c_str());
                else if(ret == FISC_RET_WARNING)
                    PRINTC(DWARN, "WARNING: %s", decodedInstruction->retStr.c_str());
            }

            if (ret == FISC_RET_ERROR) {
                DEBUG(DERROR, "Unhandled exception: execution of instruction 0x%X (opcode 0x%X, @PC 0x%X) failed.", instruction, OPCODE_MASK(instruction), pc_copy);
                break;
            }
        }
        else {
            /* Instruction executed successfully */
            if(isDebuggingEnabled()) {
                /* Just for pretty output */
                if (disassembledInstruction.find("NOP") != std::string::npos) {
                    /* I really need to improve the tab alignment code... */
                    if(decodedInstruction->timesExecuted < 10)
                        DEBUG(DNORMALH, "\t\t\t\t| OK");
                    else
                        DEBUG(DNORMALH, "\t\t\t| OK");
                }
                else {
                    DEBUG(DNORMALH, "\t\t| OK");
                }
            }
        }

        /* Now increment the Program Counter value (always aligned by 4 bytes / 32 bits, with or without the AE flag enabled) */
        if(!isBranching)
            writeRegister(SPECIAL_PC, pc_copy + FISC_INSTRUCTION_SZ / 8, false, 0, 0, 0);
        isBranching = false;
    }

    DEBUG(DNORMALH, "\n");
    DEBUG(DGOOD, "DONE EXECUTING (%d instructions executed)", instructionsExecuted - 1);

    if (cmdHasOpt("dump")) {
        std::string dumpWhat = cmdQuery("dump").second;
        if (dumpWhat == NULLSTR) {
            DEBUG(DWARN, "--debug flag with no argument. Possible options: reg | mem <addr | addr interval> | all");
        } else {
            if (strTolower(dumpWhat) == "reg") {
                /* Dump all registers */
                DEBUG(DNORMALH, "\n");
                DEBUG(DINFO2, "Dumping all register contents");
                for (uint16_t i = 0; i < FISC_TOTAL_REGISTER_COUNT; i++)
                    DEBUG(DINFO, "%s\t= 0x%X", disassembleRegister(i).c_str(), readRegister(i));
                DEBUG(DINFO2, "Dump completed");
                DEBUG(DNORMALH, "\n");
            }
        }
    }

    return PASS_RET_OK;
}

enum PassRetcode CPUModule::watchdog()
{
    return PASS_RET_OK;
}

#include "ISA/FISCALU.h"
#include "ISA/FISCFPU.h"
#include "ISA/FISCData.h"
#include "ISA/FISCBranch.h"
#include "ISA/FISCSpecial.h"

}
/*
  ______           _          __   __  __           _       _        
 |  ____|         | |        / _| |  \/  |         | |     | |     _ 
 | |__   _ __   __| |   ___ | |_  | \  / | ___   __| |_   _| | ___(_)
 |  __| | '_ \ / _` |  / _ \|  _| | |\/| |/ _ \ / _` | | | | |/ _ \  
 | |____| | | | (_| | | (_) | |   | |  | | (_) | (_| | |_| | |  __/_ 
 |______|_| |_|\__,_|  \___/|_|   |_|  |_|\___/ \__,_|\__,_|_|\___(_)
        _____ _____  _    _   __  __           _       _             
       / ____|  __ \| |  | | |  \/  |         | |     | |            
      | |    | |__) | |  | | | \  / | ___   __| |_   _| | ___        
      | |    |  ___/| |  | | | |\/| |/ _ \ / _` | | | | |/ _ \       
      | |____| |    | |__| | | |  | | (_) | (_| | |_| | |  __/       
       \_____|_|     \____/  |_|  |_|\___/ \__,_|\__,_|_|\___|       
*/