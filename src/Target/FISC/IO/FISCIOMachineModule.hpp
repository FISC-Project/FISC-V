/*-------------------------------------------------------------------------
- FILE NAME: FISCIOMachineModule.cpp
- MODULE NAME: I/O (Input/Output) Machine Module
- PURPOSE: To define the behaviour of the IO system / Virtual Motherboard
- AUTHOR: MIGUEL SANTOS
--------------------------------------------------------------------------*/

#pragma once
#include <fvm/Pass.h>
#include "../CPU/FISCCPUModule.h"

namespace FISC {

class IOMachineModule : public RunPass {
#pragma region REGION 1: THE IO MACHINE CONFIGURATION DATA
public:
    /* Pass properties */
    #define IOMACH_MODULE_PRIORITY 1 /* The execution priority of this module */

    /* List of permissions for external Passes that want to use the resources of this Pass */
    #define WHITELIST_IOMACH_MOD {DECL_WHITELIST_ALL(MemoryModule)}
#pragma endregion

#pragma region REGION 2: THE IO MACHINE STRUCTURE DEFINITION (IMPL. SPECIFIC)
public:

#pragma endregion

#pragma region REGION 3: THE IO MACHINE BEHAVIOUR IMPLEMENTATION (IMPL SPECIFIC)
private:
    bool pollGlobalIO()
    {
        /* TODO */
        return true;
    }
#pragma endregion

#pragma region REGION 4: THE IO MACHINE BEHAVIOUR (GENERIC VM FUNCTIONS)
public:

    IOMachineModule() : RunPass(IOMACH_MODULE_PRIORITY)
    {
        setWhitelist(WHITELIST_IOMACH_MOD);
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
        enum PassStatus CPUModulePassStatus = PASS_STATUS_NULL;

        while (1)
        {
            CPUModulePassStatus = getTarget()->getPassStatus(this, "CPUModule");

            if (CPUModulePassStatus == PASS_STATUS_RUNNING             ||
                CPUModulePassStatus == PASS_STATUS_RUNNINGWITHWARNINGS ||
                CPUModulePassStatus == PASS_STATUS_RUNNINGWITHERRORS   ||
                CPUModulePassStatus == PASS_STATUS_PAUSED              ||
                CPUModulePassStatus == PASS_STATUS_NOTSTARTED)
            {
                /* The CPU is running / initializing.
                   As long as the CPU is live, we will keep polling all
                   of the connected devices.  */
                pollGlobalIO();
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

#include "VirtualMotherboard/VMobo.hpp"

};
}