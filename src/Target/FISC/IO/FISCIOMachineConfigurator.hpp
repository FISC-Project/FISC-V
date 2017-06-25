/*-------------------------------------------------------------------------
- FILE NAME: FISCIOMachineConfigurator.cpp
- MODULE NAME: I/O (Input/Output) Machine Configurator
- PURPOSE: To define the structure of the IO system / Virtual Motherboard
- AUTHOR: MIGUEL SANTOS
--------------------------------------------------------------------------*/

#pragma once
#include <fvm/Pass.h>
#include <vector>

namespace FISC {

#include "VirtualMotherboard/MoboDevice.h"

static Device ** device_list_realloc = nullptr;
static unsigned int device_list_size = 0;

class IOMachineConfigurator : public ConfigPass {
#pragma region REGION 1: THE IO MACHINE CONFIGURATION DATA
public:
    /* Pass properties */
    #define IOMACH_CONFIGURATOR_PRIORITY 1 /* The execution priority of this module */

    /* List of permissions for external Passes that want to use the resources of this Pass */
    #define WHITELIST_IOMACH_CONFIG {DECL_WHITELIST_ALL(IOMachineModule)}
#pragma endregion

#pragma region REGION 2: THE IO MACHINE STRUCTURE DEFINITION (IMPL. SPECIFIC)
public:
    std::vector<Device*> device_list;

#pragma endregion

#pragma region REGION 3: THE IO MACHINE CONFIGURATION IMPLEMENTATION (IMPL SPECIFIC)
public:

#pragma endregion

#pragma region REGION 4: THE IO MACHINE CONFIGURATION IMPLEMENTATION (GENERIC VM FUNCTIONS)
public:

    IOMachineConfigurator() : ConfigPass(IOMACH_CONFIGURATOR_PRIORITY)
    {
        setWhitelist(WHITELIST_IOMACH_CONFIG);
    }

    enum PassRetcode init()
    {
        enum PassRetcode success = PASS_RET_OK;
        
        /* "Install" all statically declared devices into a nice safe vector */
        
        if (device_list_size > 0 && device_list_realloc != nullptr) {
            /* Good, we found some devices declared */
            for (unsigned int i = 0; i < device_list_size; i++) {
                if (device_list_realloc[i] == nullptr) {
                    success = PASS_RET_ERR;
                    break;
                }
                device_list_realloc[i]->targetName = getTarget()->targetName;
                device_list.push_back(device_list_realloc[i]);
            }

            if (success == PASS_RET_OK) {
                /* We shall continue initialization */
                DEBUG(DINFO, "Found %d IO devices", device_list_size);
                
                /* Cleanup the unsafe instruction list now */
                free(device_list_realloc);

                /* This variable is of no concern to any unauthorized external Pass, thus we're zeroing it */
                device_list_size = 0;
            }
            else {
                /* Some devices were declared but the device_list_realloc is either
                   corrupt or something else terribly happened to the host computer */
                success = PASS_RET_ERR;
            }
        }

        return PASS_RET_OK;
    }

    enum PassRetcode finit()
    {
        /* Nothing to do for now */
        return PASS_RET_OK;
    }

    enum PassRetcode run()
    {
        /* For now we don't have anything to configure.
           In the future, we might want to read and parse a config file (using init() function and not run())
           and select the desired configurations on the parsed results here.
           These config files could contain information about memory size (no need to hardcode them here),
           memory access speeds, restrictions, and just about any memory related property. */
        return PASS_RET_OK;
    }
    
    enum PassRetcode watchdog()
    {
        return PASS_RET_OK;
    }
#pragma endregion
};

/* Include all of the IO Devices into the system */
#include "VirtualMotherboard/VMoboInc.hpp"

}