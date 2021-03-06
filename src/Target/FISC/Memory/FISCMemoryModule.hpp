/*                                                                                
                  __  __           _       _                               
                 |  \/  |         | |     | |     _                        
                 | \  / | ___   __| |_   _| | ___(_)                       
                 | |\/| |/ _ \ / _` | | | | |/ _ \                         
                 | |  | | (_) | (_| | |_| | |  __/_                        
                 |_|  |_|\___/ \__,_|\__,_|_|\___(_)                       
  __  __                                   __  __           _       _      
 |  \/  |                                 |  \/  |         | |     | |     
 | \  / | ___ _ __ ___   ___  _ __ _   _  | \  / | ___   __| |_   _| | ___ 
 | |\/| |/ _ | '_ ` _ \ / _ \| '__| | | | | |\/| |/ _ \ / _` | | | | |/ _ \
 | |  | |  __| | | | | | (_) | |  | |_| | | |  | | (_) | (_| | |_| | |  __/
 |_|  |_|\___|_| |_| |_|\___/|_|   \__, | |_|  |_|\___/ \__,_|\__,_|_|\___|
                                    __/ |                                  
                                   |___/                                   
*/

/*----------------------------------------------------
- FILE NAME: FISCMemoryModule.cpp
- MODULE NAME: Memory Module
- PURPOSE: To define the behaviour of the Main Memory
- AUTHOR: MIGUEL SANTOS
------------------------------------------------------*/

#pragma once
#include <fvm/Pass.h>
#include <fvm/Utils/IO/File.h>
#include <fvm/Utils/ELFLoader.h>
#include <fvm/Utils/Bit.h>
#include <fvm/TargetRegistry.h>
#include "FISCMemoryConfigurator.hpp"
#include "../IO/FISCIOMachineConfigurator.hpp"
#include "../CPU/ISA/FISCISA.h"

namespace FISC {

static mutex glob_memorymodule_mutex;

class MemoryModule : public RunPass {
#pragma region REGION 1: THE MEMORY CONFIGURATION DATA
private:
    /* Pass properties */
    #define MEMORY_MODULE_PRIORITY 2 /* The execution priority of this module */

    /* List of permissions for external Passes that want to use the resources of this Pass */
    #define WHITELIST_MEM_MOD {DECL_WHITELIST_ALL(CPUModule)}

    #define MEMORY_MODULE_CPUPOLLRATE_NS 1000000 /* The rate at which the Memory Module checks if the CPU is still running, in nanoseconds */

    #define MEMORY_MODULE_ENABLE_RUN (0) /* Does the memory run() function keep waiting for the CPU to finish (1), or does it exit immediately? (0) */
public:
    bool showExecution;
#pragma endregion

#pragma region REGION 2: THE MEMORY STRUCTURE DEFINITION (IMPL. SPECIFIC)
private:
    /* External Pass handles */
    MemoryConfigurator * mconf;
    IOMachineConfigurator * ioconf;
#pragma endregion

#pragma region REGION 3: THE MEMORY BEHAVIOUR (IMPL. SPECIFIC)
public:
    uint64_t read(uint32_t address, enum FISC_DATATYPE dataType, bool forceAlign, bool isMMUOn, bool isLittleEndian, bool debug)
    {
        /* Align (or not) the address */
        if(forceAlign)
            alignAddress(address, dataType);

        if(debug && showExecution)
            DEBUG(DNORMALH, " (mrd @0x%X/%s al=%d vm=%d", address, 
                dataType == FISC_SZ_8 ? "8bit" : dataType == FISC_SZ_16 ? "16bit" : dataType == FISC_SZ_32 ? "32bit" : dataType == FISC_SZ_64 ? "64bit" : "INVAL", 
                forceAlign, isMMUOn);

        /* Check if address is valid */
        if(!isAddressValid(address, dataType))
            return (uint64_t)-1;

        /* Check if this address falls inside IO Space */
        Device * dev;
        if ((dev = ioconf->isAddressIO(address)) != nullptr) {
            /* Redirect the read request into the IO Controller */
            if (debug && showExecution)
                DEBUG(DNORMALH, ": @IODEV)");
            
            uint64_t ioval = (uint64_t)-1;
            enum DevRetcode ioret = DEV_RET_ERROR;
            if ((ioret = dev->read(ioval, address - IOMEMLOC - ioconf->getDeviceOffset(dev), dataType, debug)) != DEV_RET_OK) {
                DEBUG(DERROR, "Could not read from IO device at target %s@%s@%s@%s. Retval: %d", getTarget()->targetName.c_str(), passName.c_str(), dev->deviceName.c_str(), __func__, ioret);
                return (uint64_t)-1;
            }
            return ioval;
        }

        /* Fetch the memory */
        uint64_t memVal = (uint64_t)-1;
        switch (dataType) {
        case FISC_SZ_8:
            memVal = mconf->theMemory[address].to_ulong();
            break;
        case FISC_SZ_16:
            if(isLittleEndian)
                memVal = (mconf->theMemory[address + 1].to_ulong() << 8) |
                          mconf->theMemory[address].to_ulong();
            else
                memVal = (mconf->theMemory[address].to_ulong() << 8) |
                          mconf->theMemory[address + 1].to_ulong();
            break;
        case FISC_SZ_32:
            if (isLittleEndian)
                memVal = (mconf->theMemory[address + 3].to_ulong() << 24) |
                         (mconf->theMemory[address + 2].to_ulong() << 16) |
                         (mconf->theMemory[address + 1].to_ulong() << 8)  |
                          mconf->theMemory[address].to_ulong();
            else
                memVal = (mconf->theMemory[address].to_ulong()     << 24) |
                         (mconf->theMemory[address + 1].to_ulong() << 16) |
                         (mconf->theMemory[address + 2].to_ulong() << 8)  |
                          mconf->theMemory[address + 3].to_ulong();
            break;
        case FISC_SZ_64:
            if (isLittleEndian)
                memVal = ((uint64_t)(mconf->theMemory[address + 7].to_ulong()) << 56) |
                         ((uint64_t) mconf->theMemory[address + 6].to_ulong()  << 48) |
                         ((uint64_t) mconf->theMemory[address + 5].to_ulong()  << 40) |
                         ((uint64_t) mconf->theMemory[address + 4].to_ulong()  << 32) |
                         ((uint64_t) mconf->theMemory[address + 3].to_ulong()  << 24) |
                         ((uint64_t) mconf->theMemory[address + 2].to_ulong()  << 16) |
                         ((uint64_t) mconf->theMemory[address + 1].to_ulong()  << 8)  |
                          (uint64_t) mconf->theMemory[address].to_ulong();
            else
                memVal = ((uint64_t)(mconf->theMemory[address].to_ulong())    << 56) |
                         ((uint64_t) mconf->theMemory[address + 1].to_ulong() << 48) |
                         ((uint64_t) mconf->theMemory[address + 2].to_ulong() << 40) |
                         ((uint64_t) mconf->theMemory[address + 3].to_ulong() << 32) |
                         ((uint64_t) mconf->theMemory[address + 4].to_ulong() << 24) |
                         ((uint64_t) mconf->theMemory[address + 5].to_ulong() << 16) |
                         ((uint64_t) mconf->theMemory[address + 6].to_ulong() << 8)  |
                          (uint64_t) mconf->theMemory[address + 7].to_ulong();
            break;
        default: /* Invalid data width */ 
            if(debug && showExecution)
                DEBUG(DNORMALH, " INVAL SZ)");
            return memVal;
        }
        if (debug && showExecution)
            DEBUG(DNORMALH, ": 0x%X)", memVal);
        return memVal;
    }

    bool write(uint64_t data, uint32_t address, enum FISC_DATATYPE dataType, bool forceAlign, bool isMMUOn, bool isLittleEndian, bool debug)
    {
        LOCK(glob_memorymodule_mutex);

        /* Align (or not) the address */
        if (forceAlign)
            alignAddress(address, dataType);

        if(debug && showExecution)
            DEBUG(DNORMALH, " (mwr @0x%X/%s al=%d vm=%d", address,
                dataType == FISC_SZ_8 ? "8bit" : dataType == FISC_SZ_16 ? "16bit" : dataType == FISC_SZ_32 ? "32bit" : dataType == FISC_SZ_64 ? "64bit" : "INVAL",
                forceAlign, isMMUOn);

        /* Check if address is valid */
        if(!isAddressValid(address, dataType))
            return false;

        /* Check if this address falls inside IO Space */
        Device * dev;
        if ((dev = ioconf->isAddressIO(address)) != nullptr) {
            /* Redirect the write request into the IO Controller */
            if (debug && showExecution)
                DEBUG(DNORMALH, ": @IODEV)");
            
            enum DevRetcode ioret = DEV_RET_ERROR;
            if ((ioret = dev->write(data, address - IOMEMLOC - ioconf->getDeviceOffset(dev), dataType, debug)) != DEV_RET_OK) {
                DEBUG(DERROR, "Could not write to IO device at target %s@%s@%s@%s. Retval: %d", getTarget()->targetName.c_str(), passName.c_str(), dev->deviceName.c_str(), __func__, ioret);
                return false;
            }
            else {
                return true;
            }
        }
        
        /* Write to memory */
        switch (dataType) {
        case FISC_SZ_8:
            mconf->theMemory[address] = (uint8_t)data;
            break;
        case FISC_SZ_16:
            if (isLittleEndian) {
                mconf->theMemory[address + 1] = (uint8_t)((data & 0xFF00) >> 8);
                mconf->theMemory[address]     = (uint8_t)data & 0xFF;
            } else {
                mconf->theMemory[address]     = (uint8_t)((data & 0xFF00) >> 8);
                mconf->theMemory[address + 1] = (uint8_t)data & 0xFF;
            }
            break;
        case FISC_SZ_32:
            if (isLittleEndian) {
                mconf->theMemory[address + 3] = (uint8_t)((data & 0xFF000000) >> 24);
                mconf->theMemory[address + 2] = (uint8_t)((data & 0xFF0000) >> 16);
                mconf->theMemory[address + 1] = (uint8_t)((data & 0xFF00) >> 8);
                mconf->theMemory[address]     = (uint8_t)data & 0xFF;
            } else {
                mconf->theMemory[address]     = (uint8_t)((data & 0xFF000000) >> 24);
                mconf->theMemory[address + 1] = (uint8_t)((data & 0xFF0000) >> 16);
                mconf->theMemory[address + 2] = (uint8_t)((data & 0xFF00) >> 8);
                mconf->theMemory[address + 3] = (uint8_t)data & 0xFF;
            }
            break;
        case FISC_SZ_64:
            if (isLittleEndian) {
                mconf->theMemory[address + 7] = (uint8_t)((data & 0xFF00000000000000) >> 56);
                mconf->theMemory[address + 6] = (uint8_t)((data & 0xFF000000000000) >> 48);
                mconf->theMemory[address + 5] = (uint8_t)((data & 0xFF0000000000) >> 40);
                mconf->theMemory[address + 4] = (uint8_t)((data & 0xFF00000000) >> 32);
                mconf->theMemory[address + 3] = (uint8_t)((data & 0xFF000000) >> 24);
                mconf->theMemory[address + 2] = (uint8_t)((data & 0xFF0000) >> 16);
                mconf->theMemory[address + 1] = (uint8_t)((data & 0xFF00) >> 8);
                mconf->theMemory[address]     = (uint8_t)data & 0xFF;
            } else {
                mconf->theMemory[address]     = (uint8_t)((data & 0xFF00000000000000) >> 56);
                mconf->theMemory[address + 1] = (uint8_t)((data & 0xFF000000000000) >> 48);
                mconf->theMemory[address + 2] = (uint8_t)((data & 0xFF0000000000) >> 40);
                mconf->theMemory[address + 3] = (uint8_t)((data & 0xFF00000000) >> 32);
                mconf->theMemory[address + 4] = (uint8_t)((data & 0xFF000000) >> 24);
                mconf->theMemory[address + 5] = (uint8_t)((data & 0xFF0000) >> 16);
                mconf->theMemory[address + 6] = (uint8_t)((data & 0xFF00) >> 8);
                mconf->theMemory[address + 7] = (uint8_t)data & 0xFF;
            }
            break;
        default: /* Invalid data width */ 
            if (debug && showExecution)
                DEBUG(DNORMALH, " INVAL SZ)");
            return false;
        }
        if (debug && showExecution)
            DEBUG(DNORMALH, ": 0x%X)", data);
        return true;
    }

    uint32_t size()
    {
        return MEMORY_DEPTH;
    }

    elfsection_list_t get_elfsection_list()
    {
        return mconf->elfsection_list;
    }

private:
    uint32_t alignAddress(uint32_t & address, enum FISC_DATATYPE dataType)
    {
        switch (dataType) {
            case FISC_SZ_8: /* No need to align */       break;
            case FISC_SZ_16: address = ALIGN16(address); break;
            case FISC_SZ_32: address = ALIGN32(address); break;
            case FISC_SZ_64: address = ALIGN64(address); break;
            default: return (uint32_t)-1;
        }
        return address;
    }

    bool isAddressValid(uint32_t address, enum FISC_DATATYPE dataType)
    {
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
    MemoryModule() : RunPass(MEMORY_MODULE_PRIORITY),
        showExecution(false)
    {
        setWhitelist(WHITELIST_MEM_MOD);
    }

    bool loadMemory()
    {
        mconf->programFile.open();
        std::string tmpstr;

        /* Load Bootloader program */
        for (unsigned int i = MEMORY_LOADLOC; i < mconf->getMemSize() && i < mconf->programFile.fileSize(); i++) {
            tmpstr = mconf->programFile.read(MEMORY_WIDTH / 8);
            if (tmpstr.size() == 1) {
                mconf->theBootloaderMemory.push_back(std::bitset<MEMORY_WIDTH>(tmpstr[0]));
                mconf->loadedProgramSize++;
            }
            else {
                break; /* We've reached EOF */
            }
        }
        mconf->programFile.close();

        /* If this is an ELF file instead of a flat binary, then we must parse it and relocate it */
        if (isFileELF(mconf->programFile) && mconf->loadedProgramSize > 0) {
            DEBUG(DINFO, "Program is an ELF object file");
            if ((mconf->loadedProgramSize = elfToFlatBinary(mconf->theBootloaderMemory, mconf->elfsection_list, ENDIANNESS_TEXTSECT, ENDIANNESS_DATASECT)) == 0) {
                DEBUG(DERROR, "Could not load the ELF file into memory");
                return false;
            }
        }

        /* Copy the bootloader memory into the main memory */
        for(unsigned int i = 0; i < mconf->loadedProgramSize; i++)
            mconf->theMemory[i] = mconf->theBootloaderMemory[i];
        DEBUG(DINFO, "Loaded %d bytes / %d words into memory", (unsigned int)mconf->loadedProgramSize, (unsigned int)mconf->loadedProgramSize / 4);
        return true;
    }

    enum PassRetcode init()
    {
        enum PassRetcode success = PASS_RET_OK;
        /* Fetch Memory Configurator Pass */
        if (!(mconf = GET_PASS(MemoryConfigurator))) {
            /* We were unable to find a MemoryConfigurator pass!
               We cannot continue the execution of this pass */
            DEBUG(DERROR, "Could not fetch the Memory Configurator Pass!");
            success = PASS_RET_ERR;
        }

        /* Fetch IO Machine Configurator Pass */
        if (!(ioconf = GET_PASS(IOMachineConfigurator))) {
            /* We were unable to find a IOMachineConfigurator pass!
               We cannot continue the execution of this pass */
            DEBUG(DERROR, "Could not fetch the IO Machine Configurator Pass!");
            success = PASS_RET_ERR;
        }

        if (success == PASS_RET_OK) {
            /* Load up the memory */
            if(!loadMemory())
                success = PASS_RET_ERR;
        }

        showExecution = !cmdHasOpt("nodbgexec");

        return success;
    }
    
    enum PassRetcode finit()
    {
        return PASS_RET_OK;
    }

    enum PassRetcode run()
    {
#if MEMORY_MODULE_ENABLE_RUN == 0
        return PASS_RET_OK;
#endif

        enum PassStatus CPUModulePassStatus = PASS_STATUS_NULL;

        /* For now we don't want to keep anything running on this thread.
           We're keeping it relatively simple (for now!!) */

        while (1)
        {
#if MEMORY_MODULE_CPUPOLLRATE_NS > 0
            this_thread::sleep_for(chrono::nanoseconds(MEMORY_MODULE_CPUPOLLRATE_NS));
#endif

            CPUModulePassStatus = getTarget()->getPassStatus(this, "CPUModule");

            if (CPUModulePassStatus == PASS_STATUS_RUNNING             ||
                CPUModulePassStatus == PASS_STATUS_RUNNINGWITHWARNINGS ||
                CPUModulePassStatus == PASS_STATUS_RUNNINGWITHERRORS   ||
                CPUModulePassStatus == PASS_STATUS_PAUSED              ||
                CPUModulePassStatus == PASS_STATUS_NOTSTARTED)
            {
                /* The CPU is running / initializing. Stay idle doing nothing */
            }
            else
            {
                if (CPUModulePassStatus == PASS_STATUS_NOAUTH)
                    return PASS_RET_FATAL; /* The CPU did not give us permission to read its status. Bailing. */

                if(CPUModulePassStatus == PASS_STATUS_COMPLETED)
                    return PASS_RET_OK; /* The CPU has successfully finished its execution */

                /* At this point, the CPU has finished its execution with errors or warnings.
                   We're getting outta here now. */

                if (CPUModulePassStatus == PASS_STATUS_COMPLETEDWITHERRORS)
                    return PASS_RET_ERR;

                if (CPUModulePassStatus == PASS_STATUS_COMPLETEDWITHFATALERRORS)
                    return PASS_RET_FATAL;

                return PASS_RET_ERR;
            }
        }
        return PASS_RET_ERR;
    }

    enum PassRetcode watchdog()
    {
        return PASS_RET_OK;
    }
#pragma endregion
};
}
/*                                                                                 
     ______           _          __   __  __           _       _           
    |  ____|         | |        / _| |  \/  |         | |     | |     _    
    | |__   _ __   __| |   ___ | |_  | \  / | ___   __| |_   _| | ___(_)   
    |  __| | '_ \ / _` |  / _ \|  _| | |\/| |/ _ \ / _` | | | | |/ _ \     
    | |____| | | | (_| | | (_) | |   | |  | | (_) | (_| | |_| | |  __/_    
    |______|_| |_|\__,_|  \___/|_|   |_|  |_|\___/ \__,_|\__,_|_|\___(_)   
  __  __                                   __  __           _       _      
 |  \/  |                                 |  \/  |         | |     | |     
 | \  / | ___ _ __ ___   ___  _ __ _   _  | \  / | ___   __| |_   _| | ___ 
 | |\/| |/ _ | '_ ` _ \ / _ \| '__| | | | | |\/| |/ _ \ / _` | | | | |/ _ \
 | |  | |  __| | | | | | (_) | |  | |_| | | |  | | (_) | (_| | |_| | |  __/
 |_|  |_|\___|_| |_| |_|\___/|_|   \__, | |_|  |_|\___/ \__,_|\__,_|_|\___|
                                    __/ |                                  
                                   |___/                                   
*/