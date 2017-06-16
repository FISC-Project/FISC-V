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
#include <algorithm>

namespace FISC {

uint64_t CPUModule::readRegister(unsigned registerIndex)
{
    uint8_t bank = cconf->cpsr.mode;

    /* Assumptions: mode/bank is never going to hold a value different than 0 .. 5 (inclusive).
    The CPU would have triple faulted long before then. */

    if(bank == FISC_CPU_MODE_KERNEL || bank == FISC_CPU_MODE_USER || registerIndex == XZR) {
        /* We're setting the bank to kernel for no particular reason. 
           The goal is to have both kernel and user modes to share the same bank.
           Also, if we're reading from XZR, we don't care if the registers are banked, so
           we set the bank value anyways for that case. */
        bank = FISC_CPU_MODE_KERNEL;
    }

    /* Special case: if we're in IRQ/SIRQ mode, the registers X0..X15 are not banked. Because of this, we'll force the bank number to be the same of the User/Kernel */
    if((bank == FISC_CPU_MODE_IRQ || bank == FISC_CPU_MODE_SIRQ) && registerIndex >= 0 && registerIndex <= 15)
        bank = FISC_CPU_MODE_KERNEL;

    /* Special case: if we're in Exception/Undefined mode, the registers X0..X27 are not banked. Because of this, we'll force the bank number to be the same of the User/Kernel */
    if ((bank == FISC_CPU_MODE_EXCEPTION || bank == FISC_CPU_MODE_UNDEFINED) && registerIndex >= 0 && registerIndex <= 27)
        bank = FISC_CPU_MODE_KERNEL;

    if(registerIndex < FISC_REGISTER_COUNT) {
        if(registerIndex == XZR)
            return 0;
        else
            return cconf->x[bank][registerIndex];
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
    uint8_t bank = cconf->cpsr.mode;

    /* Assumptions: mode/bank is never going to hold a value different than 0 .. 5 (inclusive).
    The CPU would have triple faulted long before then. */

    if (bank == FISC_CPU_MODE_KERNEL || bank == FISC_CPU_MODE_USER || registerIndex == XZR) {
        /* We're setting the bank to kernel for no particular reason. 
           The goal is to have both kernel and user modes to share the same bank.
           Also, if we're writing to XZR, we don't care if the registers are banked, so
           we set the bank value anyways for that case. */
        bank = FISC_CPU_MODE_KERNEL;
    }

    /* Special case: if we're in IRQ/SIRQ mode, the registers X0..X15 are not banked. Because of this, we'll force the bank number to be the same of the User/Kernel */
    if ((bank == FISC_CPU_MODE_IRQ || bank == FISC_CPU_MODE_SIRQ) && registerIndex >= 0 && registerIndex <= 15)
        bank = FISC_CPU_MODE_KERNEL;

    /* Special case: if we're in Exception/Undefined mode, the registers X0..X27 are not banked. Because of this, we'll force the bank number to be the same of the User/Kernel */
    if ((bank == FISC_CPU_MODE_EXCEPTION || bank == FISC_CPU_MODE_UNDEFINED) && registerIndex >= 0 && registerIndex <= 27)
        bank = FISC_CPU_MODE_KERNEL;

    if (registerIndex < FISC_REGISTER_COUNT) {
        if(registerIndex != XZR)
            cconf->x[bank][registerIndex] = data;
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

uint64_t CPUModule::mmu_read(uint32_t address, enum FISC_DATATYPE dataType, bool forceAlign, bool debug)
{
    if (cconf->cpsr.pg) {
        /* Paging is enabled. We must access the memory using the value inside register
           PDP, which contains a pointer to a page directory.
           Using this virtual address, we can access the page directory to look for the
           real physical memory address. Only then we can really access the memory module */
        uint32_t physicalAddress = (uint32_t)-1;
        if(mmu_translate(physicalAddress, address) != FISC_RET_OK) {
            /* The current cpu mode does not have access privileges over this page. 
               Calling the page fault ISR ... */
            triggerSoftException(EXC_PAGEFAULT);
            return (uint64_t)-1;
        }

        return memory->read(physicalAddress, dataType, forceAlign, cconf->cpsr.pg, debug);
    }
    else {
        /* Paging is disabled */
        return memory->read(address, dataType, forceAlign, cconf->cpsr.pg, debug);
    }
}

enum FISC_RETTYPE CPUModule::mmu_write(uint64_t data, uint32_t address, enum FISC_DATATYPE dataType, bool forceAlign, bool debug)
{
    if (cconf->cpsr.pg) {
        /* Paging is enabled. We must access the memory using the value inside register
           PDP, which contains a pointer to a page directory.
           Using this virtual address, we can access the page directory to look for the
           real physical memory address. Only then we can really access the memory module */
        uint32_t physicalAddress = (uint32_t)-1;
        if (mmu_translate(physicalAddress, address) != FISC_RET_OK) {
            /* The current cpu mode does not have access privileges over this page.
               Calling the page fault ISR ... */
            return triggerSoftException(EXC_PAGEFAULT);
        }

        return memory->write(data, physicalAddress, dataType, forceAlign, cconf->cpsr.pg, debug) ? FISC_RET_OK : FISC_RET_ERROR;
    }
    else {
        /* Paging is disabled */
        return memory->write(data, address, dataType, forceAlign, cconf->cpsr.pg, debug) ? FISC_RET_OK : FISC_RET_ERROR;
    }
}

enum FISC_RETTYPE CPUModule::triggerSoftInterrupt(unsigned intCode)
{
    generatedInterrupt = true;
    return interruptCPU(intCode, false, true);
}

enum FISC_RETTYPE CPUModule::triggerHardInterrupt(unsigned intCode)
{
    generatedExternalInterrupt = true;
    return interruptCPU(intCode, false, false);
}

enum FISC_RETTYPE CPUModule::triggerSoftException(unsigned excCode)
{
    generatedException = true;
    return interruptCPU(excCode, true, true);
}

enum FISC_RETTYPE CPUModule::triggerHardException(unsigned excCode)
{
    generatedExternalException = true;
    return interruptCPU(excCode, true, false);
}

enum FISC_RETTYPE CPUModule::intExcReturn(uint32_t retAddr)
{
    enum FISC_RETTYPE ret;
    if (cconf->cpsr.mode == FISC_CPU_MODE_USER || (!isInsideException || !isInsideInterrupt)) {
        /* The user / operating system attempted to return from an interrupt handler, while not being in one.
           We shall trigger a double fault exception */
        if ((ret = triggerSoftException(EXC_DOUBLEFAULT)) != FISC_RET_OK) {
            /* Triple fault! We shall now explode the CPU on the user's face for trying to execute garbage code */
            return triggerSoftException(EXC_TRIPLEFAULT);
        } else {
            /* We must prevent the instruction from completing its execution even if the double fault handler executed properly */
            return ret;
        }
    }

    if ((ret = restoreContext()) != FISC_RET_OK) {
        /* Couldn't restore context... Triggering a triple fault exception */
        return triggerSoftException(EXC_TRIPLEFAULT);
    }

    /* We are oficially outside an exception/interrupt handler */
    if(isInsideException) {
        enableExceptions();
        isInsideException = false;
    }
    else {
        enableInterrupts();
        isInsideInterrupt = false;
    }

    return ret;
}

enum FISC_RETTYPE CPUModule::enterISR(uint32_t interruptVectorPtr, unsigned isrID)
{
    isInsideInterrupt = true;
    /*  Branch to the interrupt handler using the interruptVectorPtr as base address and isrID as offset */
    return branch(interruptVectorPtr + (isrID * sizeof(uint32_t)), false);
}

enum FISC_RETTYPE CPUModule::enterEXC(uint32_t exceptionVectorPtr, unsigned excID)
{
    isInsideException = true;
    /* Branch to the exception handler using the interruptVectorPtr as base address and excID as offset */
    return branch(exceptionVectorPtr + (excID * sizeof(uint32_t)), false);
}

enum FISC_RETTYPE CPUModule::switchContext(enum FISC_CPU_MODE newMode)
{
    /* What to save on a Context Switch:
        1- Store PC into ELR
        2- Save CPSR into current SPSR
        3- Change CPU mode to 'newMode'
        4- Load newMode's SPSR register into CPSR
    */
    
    /* Save first */
    writeRegister(SPECIAL_ELR, readRegister(SPECIAL_PC) + 4, false, 0, 0, 0); /* Save PC */
    cconf->spsr[cconf->cpsr.mode] = cconf->cpsr; /* Save current CPSR */
    oldCPUMode = cconf->cpsr.mode; /* Update the old CPU mode */

    /* Load new mode */
    cconf->cpsr = cconf->spsr[newMode]; /* Restore the CPSR for this new mode */
    cconf->cpsr.mode = cconf->spsr[newMode].mode = newMode; /* Forcefully change the mode for both CPSR and SPSR of new mode */

    return FISC_RET_OK;
}

enum FISC_RETTYPE CPUModule::restoreContext()
{
    /* What to restore on a Context Restoration:
        1- Save current CPSR into current mode's SPSR
        2- Restore CPU mode back to 'oldCPUMode'
        3- Load CPSR from old CPU mode's SPSR
        4- Load PC from ELR
    */

    /* Save current mode first */
    cconf->spsr[cconf->cpsr.mode] = cconf->cpsr;
    
    /* Toggle the oldCPUMode. We're toggling this value just so we know 
       from which handler we came from. For example, was it an exception? 
       an interrupt? software interrupt? */
    unsigned int oldCPUModeCopy = cconf->cpsr.mode;
    oldCPUMode = cconf->cpsr.mode;

    /* Restore old mode back */
    cconf->cpsr = cconf->spsr[oldCPUModeCopy]; /* Restore the CPSR for the old mode */
    cconf->cpsr.mode = cconf->spsr[oldCPUModeCopy].mode = oldCPUModeCopy; /* Forcefully restore the mode for both CPSR and SPSR of old mode */

    writeRegister(SPECIAL_PC, readRegister(SPECIAL_ELR), false, 0, 0, 0); /* Restore PC */

    return FISC_RET_OK;
}

bool CPUModule::areInterruptsEnabled()
{
    return ((cconf->cpsr.ien & 2) >> 1) ? true : false;
}

bool CPUModule::areExceptionsEnabled()
{
    return cconf->cpsr.ien & 1;
}

enum FISC_RETTYPE CPUModule::disableInterrupts()
{
    if(!areInterruptsEnabled())
        return FISC_RET_ERROR; /* Interrupts are already disabled */
    BIT_WRITE(0, cconf->cpsr.ien, 1);
    return FISC_RET_OK;
}

enum FISC_RETTYPE CPUModule::enableInterrupts()
{
    if (areInterruptsEnabled())
        return FISC_RET_ERROR; /* Interrupts are already enabled */
    BIT_WRITE(1, cconf->cpsr.ien, 1);
    return FISC_RET_OK;
}

enum FISC_RETTYPE CPUModule::disableExceptions()
{
    if (!areExceptionsEnabled())
        return FISC_RET_ERROR; /* Exceptions are already disabled */
    BIT_WRITE(0, cconf->cpsr.ien, 0);
    return FISC_RET_OK;
}

enum FISC_RETTYPE CPUModule::enableExceptions()
{
    if (areExceptionsEnabled())
        return FISC_RET_ERROR; /* Exceptions are already enabled */
    BIT_WRITE(1, cconf->cpsr.ien, 0);
    return FISC_RET_OK;
}

enum FISC_RETTYPE CPUModule::interruptCPU(unsigned code, bool isException, bool isInternal)
{
    enum FISC_RETTYPE ret = FISC_RET_ERROR;
    uint32_t jumpAddress;

    if (cconf->cpsr.mode == FISC_CPU_MODE_USER) {
        /* The user just tried to execute an interrupt.
           We will only give him permission if the interrupt code is exactly 0xFFF  */
        if (code != FISC_USER_SYSCALL_CODE) {
            /* Oh no, it's not 0xFFF. We must enter a PERMFAULT exception
               (because the user does not have permissions to trigger interrupts) */
            return triggerSoftException(EXC_PERMFAULT);
        }

        /* Else just let him call the ISR... */
    }
    
    if (isException) {
        if(code == (unsigned)EXC_TRIPLEFAULT) {
            /* This is not good... The CPU has crashed... 
               We could do something here, for example call another exception handler,
               but since the previous exception already caused an exception on itself, we
               will have to reboot the system */
            DEBUG(DERROR,"      >>>> !! PANIC: TRIPLE FAULT !! <<<<      ");
            return FISC_RET_ERROR;
        }

        /* Disable exceptions first */
        disableExceptions();
        
        if (isInsideException && code != (unsigned)EXC_DOUBLEFAULT) {
            /* We're already inside an exception!
               This means an exception caused an exception (also called double fault)
               The CPU will execute the double fault handler */
            return triggerSoftException(EXC_DOUBLEFAULT);
        }

        /* Get the exception vector's pointer in memory */
        if(code == (unsigned)EXC_PAGEFAULT)
            jumpAddress = (uint32_t)readRegister(SPECIAL_PFLA);
        else
            jumpAddress = (uint32_t)readRegister(SPECIAL_EVP);

        if ((ret = switchContext(FISC_CPU_MODE_EXCEPTION)) != FISC_RET_OK) {
            /* We were unable to switch context... This is VERY bad. 
               It's so bad that the CPU will kill itself.
               This is your code's fault... */
            return triggerSoftException(EXC_TRIPLEFAULT);
        }

        /* Finally, enter exception handler */
        return enterEXC(jumpAddress, code);
    }
    else {
        /* Disable interrupts first */
        disableInterrupts();
        
        if (isInsideInterrupt) {
            /* We're already running an interrupt! */
            if (isInternal) {
                /* An interrupt service on its own attempted to call another interrupt, thus causing a 'double interrupt' exception */
                if ((ret = triggerSoftException(EXC_DOUBLEINTERRUPT)) == FISC_RET_ERROR) {
                    /* God damn, the exception handler caused an exception!
                       There's no way the CPU can survive this.
                       Killing CPU now... */
                    /* DISCLAIMER: the handler itself must return a non error value in any defined predictable behaviour case */
                    return triggerSoftException(EXC_TRIPLEFAULT);
                }
                /* The interrupt was serviced successfully */
                return ret;
            }
            else {
                /* An external device is requesting resources from the CPU via an IRQ.
                   We must tell the device that it must wait
                   for the CPU to finish servicing the current IRQ */
                return FISC_RET_WAIT;
            }
        }

        /* Get the interrupt vector's pointer in memory */
        jumpAddress = (uint32_t)readRegister(SPECIAL_IVP);

        if(isInternal)
            ret = switchContext(FISC_CPU_MODE_SIRQ);
        else
            ret = switchContext(FISC_CPU_MODE_IRQ);

        if (ret != FISC_RET_OK) {
            /* We were unable to switch context... This is VERY bad. 
               It's so bad that the CPU will kill itself.
               This is your code's fault... */
            return triggerSoftException(EXC_TRIPLEFAULT);
        }

        /* Finally, enter interrupt handler */
        return enterISR(jumpAddress, code);
    }
}

bool CPUModule::detectOverflow(uint64_t operand1, uint64_t operand2, char operation)
{
    switch (operation) {
    case '+': case '&': return (operand2 > 0) && (operand1 > INT_MAX - operand2);
    case '-': return (operand2 < 0) && (operand1 > INT_MAX + operand2);
    default: /* Unknown/Invalid operation */ return false;
    }
}

bool CPUModule::detectCarry(uint64_t operand1, uint64_t operand2, char operation)
{
    switch (operation) {
    case '+': case '&': {
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
       decode by opcode size manually.
    */

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

enum FISC_RETTYPE CPUModule::enterUndefMode()
{
    cconf->cpsr.mode = FISC_CPU_MODE_UNDEFINED;
    return FISC_RET_OK;
}

enum FISC_RETTYPE CPUModule::mmu_translate(uint32_t & retVal, uint32_t virtualAddr)
{
    /* Get the physical address of the page directory */
    uint32_t pageDirectoryAddress = (uint32_t)readRegister(SPECIAL_PDP);
    uint64_t memVal = (uint64_t)-1;
    if (pageDirectoryAddress >= MEMORY_DEPTH) {
        DEBUG(DERROR, "The PDP (Page Directory Pointer) register holds a value that points beyond the memory size boundary.");
        return FISC_RET_ERROR;
    }

    /* Calculate indices from the Virtual Address */
    uint32_t tableIdx = INDEX_FROM_BIT((virtualAddr)  / FISC_PAGE_SIZE, FISC_PAGES_PER_TABLE);
    uint32_t pageIdx  = OFFSET_FROM_BIT((virtualAddr) / FISC_PAGE_SIZE, FISC_PAGES_PER_TABLE);
    
    /* Calculate the physical address of the table and table entry */
    uint32_t tableAddress = pageDirectoryAddress + (tableIdx * sizeof(page_table_t));
    uint32_t tableEntryAddress = pageDirectoryAddress + (FISC_TABLES_PER_DIR * sizeof(page_table_t)) + (tableIdx * sizeof(page_table_entry_t));

    memVal = memory->read(tableEntryAddress, FISC_SZ_32, false, cconf->cpsr.pg, false);
    page_table_entry_t * pageTableEntry = (page_table_entry_t*)(*(uint64_t*)&memVal);
    
    /* See if the table is mapped */
    if (!pageTableEntry->present)
        return triggerSoftException(EXC_PAGEFAULT);

    /* If current CPU mode is not allowed in this table entry, generate exception */
    if (!pageTableEntry->user && cconf->cpsr.mode == FISC_CPU_MODE_USER)
        return triggerSoftException(EXC_PAGEFAULT);

    /* Calculate the physical address of the page */
    uint32_t pageAddress = tableAddress + (pageIdx * sizeof(page_t));
    memVal = memory->read(pageAddress, FISC_SZ_32, false, cconf->cpsr.pg, false);
    page_t * pageEntry = (page_t*)(*(uint64_t*)&memVal);

    /* See if the page is mapped */
    if (!pageEntry->present)
        return triggerSoftException(EXC_PAGEFAULT);

    /* If current CPU mode is not allowed in this page, generate exception */
    if (!pageEntry->user && cconf->cpsr.mode == FISC_CPU_MODE_USER)
        return triggerSoftException(EXC_PAGEFAULT);

    /* Finally, get the physical address */
    retVal = (pageEntry->phys_addr << 12) | (virtualAddr & 0xFFF);
    return FISC_RET_OK;
}

void CPUModule::dumpWarning(std::string problematicArg, std::string fullArg)
{
    DEBUG(DWARN, "The provided argument '%s' in '%s' is not a valid integer", problematicArg.c_str(), fullArg.c_str());
}

void CPUModule::dumpInternals()
{
    if (!cmdHasOpt("dump")) return;

    std::string dumpWhat;

    for(unsigned i = 0; cmdHasOpt("dump", i); i++) {
        if ((dumpWhat = strTolower(cmdQuery("dump", i).second)) == NULLSTR) {
            DEBUG(DWARN, "--debug flag with no argument. Possible options: reg | mem.startaddr.<.endaddr|.datawidth|.endaddr.datawidth> (ex: mem.0..10.43)");
            continue;
        }
        else {
            if (dumpWhat.substr(0, 3) == "reg") {
                /* Supported argument formats:
                    reg
                    reg.x   (x = how many registers will be dumped) */
                std::string dumpRegCountStr = NULLSTR;
                uint32_t dumpRegCount = FISC_TOTAL_REGISTER_COUNT;
                size_t nextDotPos = dumpWhat.find('.');
                if (nextDotPos != -1) {
                    if (!strIsNumber(dumpRegCountStr = dumpWhat.substr(nextDotPos + 1, dumpWhat.size() - nextDotPos))) {
                        dumpWarning(dumpRegCountStr, dumpWhat);
                        continue;
                    }
                    dumpRegCount = std::stol(dumpRegCountStr);
                }
                    
                /* Dump registers */
                DEBUG(DNORMALH, "\n");
                DEBUG(DINFO2, "Dumping register contents (count: %d)", dumpRegCount >= FISC_TOTAL_REGISTER_COUNT ? FISC_TOTAL_REGISTER_COUNT : dumpRegCount);
                for (uint16_t i = 0; i < dumpRegCount && i < FISC_TOTAL_REGISTER_COUNT; i++) {
                    if (i == SPECIAL_PC || i == SPECIAL_ESR || (i >= SPECIAL_CPSR && i <= SPECIAL_SPSR5))
                        DEBUG(DINFO, "|%d| %s\t= 0x%X", i, disassembleRegister(i).c_str(), readRegister(i));
                    else
                        DEBUG(DINFO, "|%d| %s\t= 0x%I64X", i, disassembleRegister(i).c_str(), readRegister(i));
                }
                DEBUG(DINFO2, "Dump completed");
                DEBUG(DNORMALH, "\n");
            }
            else if (dumpWhat.substr(0, 4) == "mem.") {
                /* We're gonna parse the following argument format: x.y.z
                For instance, the user would use this argument the following way:
                    --dump mem.0..10.8
                This argument dumps the memory from addresses 0 to 10 (inclusive) with 8 bits of alignment.
                This can be simplified:
                    --dump mem.0
                This prints the first address of memory and assumes 8 bits of alignment.
                The other possible options are:
                    --dump mem.0.16      | Dumps address 0 with 16 bits of alignment
                    --dump mem.0..100    | Dumps all addresses from 0 to 100 with 8 bits of alignment (assumed)
                    --dump mem.0..500.64 | Dumps all addresses from 0 to 500 with 64 bits of alignment
                Possible alignments are:
                    8  (bits)
                    16 (bits)
                    32 (bits)
                    64 (bits)
                */
                std::string memRange = dumpWhat.substr(4, dumpWhat.size() - 4);
                std::string memStartPos = NULLSTR;
                std::string memEndPos = NULLSTR;
                std::string memDumpDataWidth = NULLSTR;

                size_t nextDotPos = memRange.find('.');
                if (nextDotPos != -1) {
                    if (!strIsNumber(memStartPos = memRange.substr(0, nextDotPos))) {
                        dumpWarning(memStartPos, memRange);
                        continue;
                    }
                    nextDotPos = memRange.find('.', nextDotPos) + 1;
                    if (nextDotPos > 0) {
                        /* We found at least 1 dot after the 1st dot */
                        size_t dot1 = nextDotPos;
                        nextDotPos = memRange.find('.', nextDotPos) + 1;
                        if (nextDotPos > 0) {
                            /* We found a range */
                            size_t dot2 = nextDotPos;
                            nextDotPos = memRange.find('.', nextDotPos) + 1;
                            if (nextDotPos > 0) {
                                /* We found the dump data width with a range */
                                if (!strIsNumber(memEndPos = memRange.substr(dot2, nextDotPos - dot2 - 1))) {
                                    dumpWarning(memEndPos, memRange);
                                    continue;
                                }
                                if (!strIsNumber(memDumpDataWidth = memRange.substr(nextDotPos, memRange.size() - nextDotPos))) {
                                    dumpWarning(memDumpDataWidth, memRange);
                                    continue;
                                }
                            }
                            else {
                                /* No data width found, this means we'll just fetch the range */
                                if (!strIsNumber(memEndPos = memRange.substr(dot2, memRange.size() - dot2))) {
                                    dumpWarning(memEndPos, memRange);
                                    continue;
                                }
                            }
                        }
                        else {
                            /* We found the dump data width with no range */
                            if (!strIsNumber(memDumpDataWidth = memRange.substr(dot1, memRange.size() - dot1))) {
                                dumpWarning(memDumpDataWidth, memRange);
                                continue;
                            }
                        }
                    }
                }
                else {
                    /* A number with no dots was provided */
                    if (!strIsNumber(memStartPos = memRange.substr(0, memRange.size()))) {
                        dumpWarning(memStartPos, memRange);
                        continue;
                    }
                }
            
                /* Now we can finally dump the memory */
                if (memStartPos != NULLSTR) {
                    DEBUG(DNORMALH, "\n");
                    DEBUG(DINFO2, "Dumping memory contents (dump arguments: %s)", memRange.c_str());
                    uint32_t memStartPosInt = std::stol(memStartPos);
                    uint32_t memDumpDataWidthInt = memDumpDataWidth != NULLSTR ? std::stol(memDumpDataWidth) : FISC_SZ_8;
                    switch (memDumpDataWidthInt) {
                        case 8:  memDumpDataWidthInt = FISC_SZ_8;  break;
                        case 16: memDumpDataWidthInt = FISC_SZ_16; break;
                        case 32: memDumpDataWidthInt = FISC_SZ_32; break;
                        case 64: memDumpDataWidthInt = FISC_SZ_64; break;
                        default: memDumpDataWidthInt = FISC_SZ_8;  break;
                    }
                
                    uint32_t memEndPosInt = memEndPos != NULLSTR ? (std::stoul(memEndPos) + 1) << (memDumpDataWidthInt - 1) : 0;
                    uint32_t incVal = 1 << (memDumpDataWidthInt - 1);
                    uint32_t ctr = 1;

                    if (memDumpDataWidthInt == FISC_SZ_64)
                        for (uint32_t i = memStartPosInt << (memDumpDataWidthInt - 1); i < memEndPosInt; i += incVal)
                            DEBUG(DINFO, "|%d| M[0x%X]\t= 0x%I64X", ctr++, i, memory->read(i, (enum FISC_DATATYPE)memDumpDataWidthInt, false, false, false));
                    else
                        for (uint32_t i = memStartPosInt << (memDumpDataWidthInt - 1); i < memEndPosInt; i += incVal)
                            DEBUG(DINFO, "|%d| M[0x%X]\t= 0x%X", ctr++, i, memory->read(i, (enum FISC_DATATYPE)memDumpDataWidthInt, false, false, false));

                    DEBUG(DINFO2, "Dump completed");
                    DEBUG(DNORMALH, "\n");
                }
            }
        }
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
    isInsideException = false;
    isInsideInterrupt = false;
    generatedException = false;
    generatedExternalException = false;
    generatedInterrupt = false;
    generatedExternalInterrupt = false;

    oldCPUMode = cconf->cpsr.mode;
    
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
    while ((instruction = (uint32_t)mmu_read((pc_copy = (uint32_t)readRegister(SPECIAL_PC)), FISC_SZ_32, false, false)) != (uint32_t)-1)
    {
        /* 2 - Decode instruction */
        Instruction * decodedInstruction = decode(instruction);
        if(decodedInstruction == nullptr || !decodedInstruction->initialized) {
            DEBUG(DERROR, "Unhandled exception: instruction 0x%X (opcode 0x%X, @PC 0x%X) is undefined. Terminating.", instruction, OPCODE_MASK(instruction), pc_copy);
            enterUndefMode();
            triggerSoftException(EXC_INVALOPC);
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
                DEBUG(DERROR, "Unhandled exception: execution of instruction 0x%X (opcode 0x%X, @PC 0x%X) failed. Terminating.", instruction, OPCODE_MASK(instruction), pc_copy);
                enterUndefMode();
                triggerSoftException(EXC_INVALOPC);
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
        if(!isBranching && !generatedException && !generatedExternalException && !generatedExternalInterrupt && !generatedInterrupt)
            writeRegister(SPECIAL_PC, pc_copy + FISC_INSTRUCTION_SZ / 8, false, 0, 0, 0);

        isBranching = false;
        generatedException = false;
        generatedInterrupt = false;
        generatedExternalInterrupt = false;
    }

    DEBUG(DNORMALH, "\n");
    DEBUG(DGOOD, "DONE EXECUTING (%d instructions executed)", instructionsExecuted - 1);

    dumpInternals();

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