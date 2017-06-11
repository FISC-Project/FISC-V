/*-----------------------------------------------------------------------------
                           __  __          ___                                |
                      |\/|/  \|  \|  ||   |__ .                               |
                      |  |\__/|__/\__/|___|___.                               |
          ___     __  __        __  __      ___ __      __    _____  __       |
     |\/||__ |\/|/  \|__)\ /   /  `/  \|\ ||__|/ _`|  ||__) /\ |/  \|__)      |
     |  ||___|  |\__/|  \ |    \__,\__/| \||  |\__>\__/|  \/~~\|\__/|  \      |
                                                                              |
-----------------------------------------------------------------------------*/

/*----------------------------------------------------
- FILE NAME: FISCMemoryConfigurator.cpp
- MODULE NAME: Memory Configurator
- PURPOSE: To define the structure of the Main Memory
- AUTHOR: MIGUEL SANTOS
------------------------------------------------------*/

#pragma once
#include <fvm/Pass.h>
#include <fvm/Utils/Cmdline.h>
#include <fvm/Utils/String.h>
#include <fvm/Utils/IO/File.h>
#include <fstream>
#include <vector>
#include <bitset>
#include <stdint.h>
#include <stdio.h>

class MemoryConfigurator : public ConfigPass {
#pragma region REGION 1: THE MEMORY CONFIGURATION DATA
public:
    /* Pass properties */
    #define MEMORY_CONFIGURATOR_PRIORITY 1 /* The execution priority of this module */
    #define MEMORY_PROGRAM_FILE_IOS_MODE std::ios::in | std::ios::binary /* The mode the program file will be opened with */

    /* List of permissions for external Passes that want to use the resources of this Pass */
    #define WHITELIST_MEM_CONFIG {DECL_WHITELIST_ALL(MemoryModule)}

    /* Command line flags */
    #define MEMORY_FLAG_BOOT_SHORT 'b'
    #define MEMORY_FLAG_BOOT_LONG "boot"

    /* Implementation properties */
    #define MEMORY_WIDTH   8    /* The width of the memory */
    #define MEMORY_DEPTH   1024 /* Size of memory in bytes */
    #define MEMORY_LOADLOC 0    /* Where to load the program on startup */
#pragma endregion

#pragma region REGION 2: THE MEMORY STRUCTURE DEFINITION (IMPL. SPECIFIC)
public:
    std::vector<std::bitset<MEMORY_WIDTH> > theMemory; /* The actual main memory */
    uint64_t loadedProgramSize; /* Size of the loaded program */
    File programFile; /* The file being loaded into memory */
    /* DISCLAIMER: In the future, we might want to care about a singular
       file being loaded into memory as the absolute program being loaded. 
       In other words, the first program that is loaded might be responsible for
       loading other files, thus rendering this variable useless. */
#pragma endregion

#pragma region REGION 3: THE MEMORY CONFIGURATION IMPLEMENTATION (IMPL SPECIFIC)
public:
    uint64_t getMemSize() {
        return theMemory.size();
    }

    uint64_t getProgSize() {
        return loadedProgramSize;
    }
#pragma endregion

#pragma region REGION 4: THE MEMORY CONFIGURATION IMPLEMENTATION (GENERIC VM FUNCTIONS)
public:
    MemoryConfigurator() : ConfigPass(MEMORY_CONFIGURATOR_PRIORITY),
        loadedProgramSize(0), theMemory(MEMORY_DEPTH, -1), programFile(NULLSTR, 0)
    {
        setWhitelist(WHITELIST_MEM_CONFIG);
    }
    
    enum PassRetcode init() {
        enum PassRetcode success = PASS_RET_ERR;
        printf("- Initializating Memory\n");

        /* Setup program file */
        if (cmdHasOpt(MEMORY_FLAG_BOOT_SHORT)) {
            std::pair<char, std::string> fileName = cmdQuery(MEMORY_FLAG_BOOT_SHORT);
            /* Create file only if it exists */
            if (programFile.create(fileName.second, MEMORY_PROGRAM_FILE_IOS_MODE))
                success = PASS_RET_OK;
        }
        if (cmdHasOpt(MEMORY_FLAG_BOOT_LONG)) {
            std::pair<std::string, std::string> fileName = cmdQuery(MEMORY_FLAG_BOOT_LONG);
            /* Create file only if it exists */
            if (programFile.create(fileName.second, MEMORY_PROGRAM_FILE_IOS_MODE))
                success = PASS_RET_OK;
        }

        if (success == PASS_RET_ERR) {
            /* TODO: If the variable success is equal to PASS_RET_ERR,
            we must tell the top layer (VM) that this module
            will not be able to continue, thus forcing every other
            single module (on this target only) to cancel its current
            operations. */
            printf("\n>> ERROR: Could not initialize the Memory Configurator Pass!\n\n");
        }

        return success;
    }

    enum PassRetcode finit() {
        /* Nothing to do for now */
        return PASS_RET_OK;
    }

    enum PassRetcode run() {
        /* For now we don't have anything to configure.
           In the future, we might want to read and parse a config file (using init() function and not run())
           and select the desired configurations on the parsed results here.
           These config files could contain information about memory size (no need to hardcode them here),
           memory access speeds, restrictions, and just about any memory related property. */
        return PASS_RET_OK;
    }

    enum PassRetcode watchdog() {
        return PASS_RET_OK;
    }
#pragma endregion
};
/*-----------------------------------------------------------------------------
              ___     __     __  ___        __  __          ___               |
             |__ |\ ||  \   /  \|__    |\/|/  \|  \|  ||   |__ .              |
             |___| \||__/   \__/|      |  |\__/|__/\__/|___|___.              |
          ___     __  __        __  __      ___ __      __    _____  __       |
     |\/||__ |\/|/  \|__)\ /   /  `/  \|\ ||__|/ _`|  ||__) /\ |/  \|__)      |
     |  ||___|  |\__/|  \ |    \__,\__/| \||  |\__>\__/|  \/~~\|\__/|  \      |
                                                                              |
-----------------------------------------------------------------------------*/