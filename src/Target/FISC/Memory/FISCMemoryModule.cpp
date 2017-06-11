/*---------------------------------------------------------------
                     __  __          ___                        |
                |\/|/  \|  \|  ||   |__ .                       |
                |  |\__/|__/\__/|___|___.                       |
          ___     __  __            __  __          ___         |
     |\/||__ |\/|/  \|__)\ /   |\/|/  \|  \|  ||   |__          |
     |  ||___|  |\__/|  \ |    |  |\__/|__/\__/|___|___         |
                                                                |
---------------------------------------------------------------*/

/*----------------------------------------------------
- FILE NAME: FISCMemoryModule.cpp
- MODULE NAME: Memory Module
- PURPOSE: To define the behaviour of the Main Memory
- AUTHOR: MIGUEL SANTOS
------------------------------------------------------*/

#pragma once
#include <fvm/Pass.h>
#include <fvm/Utils/IO/File.h>
#include <fvm/Utils/Bit.h>
#include <fvm/TargetRegistry.h>
#include "FISCMemoryConfigurator.cpp"
#include "../Pipeline/ISA/FISCISA.h"
#include <stdio.h>

class MemoryModule : public RunPass {
#pragma region REGION 1: THE MEMORY CONFIGURATION DATA
private:
    /* Pass properties */
    #define MEMORY_MODULE_PRIORITY 1 /* The execution priority of this module */

    /* List of permissions for external Passes that want to use the resources of this Pass */
    #define WHITELIST_MEM_MOD {DECL_WHITELIST_ALL(PipelineModule)}
#pragma endregion

#pragma region REGION 2: THE MEMORY STRUCTURE DEFINITION (IMPL. SPECIFIC)
private:
    /* External Pass handles */
    MemoryConfigurator * mconf;
#pragma endregion

#pragma region REGION 3: THE MEMORY BEHAVIOUR (IMPL. SPECIFIC)


public:
    uint64_t read(uint32_t address, enum FISC_DATATYPE dataType, bool align) {
        /* Align (or not) the address */
        if(align)
            alignAddress(address, dataType);

        /* Check if address is valid */
        if(!isAddressValid(address, dataType))
            return (uint64_t)-1;

        /* Fetch the memory */
        switch (dataType) {
        case FISC_SZ_8:
            return mconf->theMemory[address].to_ulong();
        case FISC_SZ_16:
            return (mconf->theMemory[address].to_ulong() << 8) |
                mconf->theMemory[address + 1].to_ulong();
        case FISC_SZ_32:
            return  (mconf->theMemory[address].to_ulong() << 24) |
                (mconf->theMemory[address + 1].to_ulong() << 16) |
                (mconf->theMemory[address + 2].to_ulong() << 8) |
                mconf->theMemory[address + 3].to_ulong();
        case FISC_SZ_64:
            return ((uint64_t)(mconf->theMemory[address].to_ulong()) << 56) |
                ((uint64_t)mconf->theMemory[address + 1].to_ulong() << 48) |
                ((uint64_t)mconf->theMemory[address + 2].to_ulong() << 40) |
                ((uint64_t)mconf->theMemory[address + 3].to_ulong() << 32) |
                ((uint64_t)mconf->theMemory[address + 4].to_ulong() << 24) |
                ((uint64_t)mconf->theMemory[address + 5].to_ulong() << 16) |
                ((uint64_t)mconf->theMemory[address + 6].to_ulong() << 8) |
                (uint64_t)mconf->theMemory[address + 7].to_ulong();
        default: return (uint64_t)-1;
        }
        return (uint64_t)-1;
    }

    bool write(uint64_t data, uint32_t address, enum FISC_DATATYPE dataType, bool align) {
        /* Align (or not) the address */
        if (align)
            alignAddress(address, dataType);

        /* Check if address is valid */
        if(!isAddressValid(address, dataType))
            return false;

        /* Write to memory */
        switch (dataType) {
        case FISC_SZ_8:
            mconf->theMemory[address] = (uint8_t)data;
            break;
        case FISC_SZ_16:
            mconf->theMemory[address] = (uint8_t)((data & 0xFF00) >> 8);
            mconf->theMemory[address + 1] = (uint8_t)data & 0xFF;
            break;
        case FISC_SZ_32:
            mconf->theMemory[address] = (uint8_t)((data & 0xFF000000) >> 24);
            mconf->theMemory[address + 1] = (uint8_t)((data & 0xFF0000) >> 16);
            mconf->theMemory[address + 2] = (uint8_t)((data & 0xFF00) >> 8);
            mconf->theMemory[address + 3] = (uint8_t)data & 0xFF;
            break;
        case FISC_SZ_64:
            mconf->theMemory[address] = (uint8_t)((data & 0xFF00000000000000) >> 56);
            mconf->theMemory[address + 1] = (uint8_t)((data & 0xFF000000000000) >> 48);
            mconf->theMemory[address + 2] = (uint8_t)((data & 0xFF0000000000) >> 40);
            mconf->theMemory[address + 3] = (uint8_t)((data & 0xFF00000000) >> 32);
            mconf->theMemory[address + 4] = (uint8_t)((data & 0xFF000000) >> 24);
            mconf->theMemory[address + 5] = (uint8_t)((data & 0xFF0000) >> 16);
            mconf->theMemory[address + 6] = (uint8_t)((data & 0xFF00) >> 8);
            mconf->theMemory[address + 7] = (uint8_t)data & 0xFF;
            break;
        default: return false;
        }
        return true;
    }

private:
    uint32_t alignAddress(uint32_t & address, enum FISC_DATATYPE dataType) {
        switch (dataType) {
            case FISC_SZ_8: /* No need to align */       break;
            case FISC_SZ_16: address = ALIGN16(address); break;
            case FISC_SZ_32: address = ALIGN32(address); break;
            case FISC_SZ_64: address = ALIGN64(address); break;
            default: return (uint32_t)-1;
        }
        return address;
    }

    bool isAddressValid(uint32_t address, enum FISC_DATATYPE dataType) {
        switch (dataType) {
            case FISC_SZ_8: /* Intentional fallthrough */
            case FISC_SZ_16: 
            case FISC_SZ_32:
            case FISC_SZ_64: return address < mconf->theMemory.size();
            default: return false;
        }
    }
#pragma endregion

#pragma region REGION 4: THE MEMORY BEHAVIOUR (GENERIC VM FUNCTIONS)
public:
    MemoryModule() : RunPass(MEMORY_MODULE_PRIORITY)
    {
        setWhitelist(WHITELIST_MEM_MOD);
    }

    enum PassRetcode init() {
        enum PassRetcode success = PASS_RET_OK;
        /* Fetch Memory Configurator Pass */
        if (!(mconf = GET_PASS(MemoryConfigurator))) {
            /* TODO: We were unable to find a MemoryConfigurator pass!
               We cannot continue the execution of this pass */
            printf("\n>> ERROR: Could not fetch the Memory Configurator Pass!\n\n");
            success = PASS_RET_ERR;
        }

        if (success == PASS_RET_OK) {
            /* Load up the memory */
            mconf->programFile.open();
            std::string tmpstr;
            for (unsigned int i = MEMORY_LOADLOC; i < mconf->getMemSize() && i < mconf->programFile.fileSize(); i++) {
                tmpstr = mconf->programFile.read(MEMORY_WIDTH / 8);
                if (tmpstr.size() == 1) {
                    mconf->theMemory[i] = std::bitset<MEMORY_WIDTH>(tmpstr[0]);
                    mconf->loadedProgramSize++;
                } else {
                    break; /* We've reached EOF */
                }
            }
            mconf->programFile.close();
            printf("- Loaded %d bytes into memory\n", (unsigned int)mconf->loadedProgramSize);
        }
        return success;
    }
    
    enum PassRetcode finit() {
        printf("- Terminating Memory\n");
        return PASS_RET_OK;
    }

    enum PassRetcode run() {
        /* TODO: For now we don't want to keep anything running on this thread.
           We're keeping it relatively simple (for now!!) */
        return PASS_RET_OK;
    }

    enum PassRetcode watchdog() {
        return PASS_RET_OK;
    }
#pragma endregion
};
/*---------------------------------------------------------------
     ___     __     __  ___        __  __          ___          |
    |__ |\ ||  \   /  \|__    |\/|/  \|  \|  ||   |__ .         |
    |___| \||__/   \__/|      |  |\__/|__/\__/|___|___.         |
          ___     __  __            __  __          ___         |
     |\/||__ |\/|/  \|__)\ /   |\/|/  \|  \|  ||   |__          |
     |  ||___|  |\__/|  \ |    |  |\__/|__/\__/|___|___         |
                                                                |
----------------------------------------------------------------*/