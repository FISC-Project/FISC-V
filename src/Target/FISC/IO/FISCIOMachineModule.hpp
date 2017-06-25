/*-------------------------------------------------------------------------
- FILE NAME: FISCIOMachineModule.hpp
- MODULE NAME: I/O (Input/Output) Machine Module
- PURPOSE: To define the behaviour of the IO system / Virtual Motherboard
- AUTHOR: MIGUEL SANTOS
--------------------------------------------------------------------------*/

#pragma once
#include <fvm/Pass.h>
#include "../CPU/FISCCPUModule.h"
#include "FISCIOMachineConfigurator.hpp"

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
private:
    IOMachineConfigurator * ioconf; /* The configuration + structure of the IO Controller and Virtual Motherboard */
    CPUModule * cpu;                /* The handle for the CPU itself                                              */

#pragma endregion

#pragma region REGION 3: THE IO MACHINE BEHAVIOUR IMPLEMENTATION (IMPL SPECIFIC)
private:
    enum DevRetcode pollGlobalIO()
    {
        enum DevRetcode ret = DEV_RET_OK;

        for (auto & dev : ioconf->device_list) {
            if ((ret = dev->poll()) != DEV_RET_OK)
                return ret; /* Something went wrong with this device */
        }
        return ret;
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
        
        /* Fetch IO Machine Configurator Pass */
        if (!(ioconf = GET_PASS(IOMachineConfigurator))) {
            /* We were unable to find a IOMachineConfigurator pass!
               We cannot continue the execution of this pass */
            DEBUG(DERROR, "Could not fetch the IO Machine Configurator Pass!");
            return PASS_RET_ERR;
        }

        /* Fetch the CPUModule Pass */
        if (!(cpu = GET_PASS(CPUModule))) {
            /* We were unable to find a CPUModule pass!
               We cannot continue the execution of this pass */
            DEBUG(DERROR, "Could not fetch the CPU Module Pass!");
            return PASS_RET_ERR;
        }

        for (auto & dev : ioconf->device_list) {
            DEBUG(DINFO, "Initializing IO device %s@%s@%s", dev->targetName.c_str(), passName.c_str(), dev->deviceName.c_str());
            if (dev->init() != DEV_RET_OK) {
                DEBUG(DERROR, "Could not initialize IO device %s@%s@%s", dev->targetName.c_str(), passName.c_str(), dev->deviceName.c_str());
                return PASS_RET_ERR;
            }
        }

        return PASS_RET_OK;
    }

    enum PassRetcode finit()
    {
        for (auto & dev : ioconf->device_list) {
            DEBUG(DINFO, "Closing IO device %s@%s@%s", dev->targetName.c_str(), passName.c_str(), dev->deviceName.c_str());
            if (dev->finit() != DEV_RET_OK) {
                DEBUG(DERROR, "Could not close IO device %s@%s@%s", dev->targetName.c_str(), passName.c_str(), dev->deviceName.c_str());
                return PASS_RET_ERR;
            }
        }
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
                if (pollGlobalIO() != DEV_RET_OK) {
                    /* A device stopped working */
                    return PASS_RET_ERR;
                }
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