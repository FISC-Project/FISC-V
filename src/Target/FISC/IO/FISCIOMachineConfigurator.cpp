/*-------------------------------------------------------------------------
- FILE NAME: FISCIOMachineConfigurator.cpp
- MODULE NAME: I/O (Input/Output) Machine Configurator
- PURPOSE: To define the structure of the IO system / Virtual Motherboard
- AUTHOR: MIGUEL SANTOS
--------------------------------------------------------------------------*/

#pragma once
#include <fvm/Pass.h>

namespace FISC {

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
        enum PassRetcode success = PASS_RET_ERR;
        /* TODO */
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
}