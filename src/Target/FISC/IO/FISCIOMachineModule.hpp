/*-------------------------------------------------------------------------
- FILE NAME: FISCIOMachineModule.hpp
- MODULE NAME: I/O (Input/Output) Machine Module
- PURPOSE: To define the behaviour of the IO system / Virtual Motherboard
- AUTHOR: MIGUEL SANTOS
--------------------------------------------------------------------------*/

#pragma once
#include <fvm/Pass.h>
#include "FISCIOMachineModule.h"
#include "../CPU/FISCCPUModule.h"
#include "FISCIOMachineConfigurator.hpp"

namespace FISC {

#define LOCK(mut) lock_guard<mutex> lock(mut)

static mutex glob_iomodule_mutex;

#include "VirtualMotherboard/MoboDevice.h"

static void iodevRunLauncher(void * runArgs)
{
    runDevLaunchCommandPacket_t * runCmd = (runDevLaunchCommandPacket_t*)runArgs;

    if (runCmd && runCmd->theRunningDevice) {
        runCmd->retval = runCmd->theRunningDevice->run(runCmd);
        runCmd->hasReturned = true;
    }
    else {
        /* Setting this to null will indicate the packet is invalid */
        runCmd->retval = DEV_RET_NULL;
    }
}

bool IOMachineModule::isLive()
{
    LOCK(glob_iomodule_mutex);
    return isIOLive;
}

enum DevRetcode IOMachineModule::pollGlobalIO()
{
    enum DevRetcode ret = DEV_RET_OK;

    for (auto & dev : ioconf->device_list) {
        if ((ret = dev->poll()) != DEV_RET_OK)
            return ret; /* Something went wrong with this device */
    }
    return ret;
}

enum PassRetcode IOMachineModule::collectDevices()
{
    for (auto & dev : ioconf->device_list) {
        if (!dev->alreadyJoined) {
            if (dev->runCmd.hasReturned) {
                dev->runThread->join();
                dev->alreadyJoined = true;

                if (liveThreads == 0) {
                    /* What? How? Why? When? How is this 0?
                       Well, one thing is certain, this thing is gonna crash! */
                    DEBUG(DERROR, "(FATAL) Unable to join device's execution thread at target %s@%s@%s", dev->targetName.c_str(), passName.c_str(), dev->deviceName.c_str());
                    return PASS_RET_FATAL;
                }

                /* One thread down */
                liveThreads--;

                /* Check if we managed to launch the device at all */
                if (dev->runCmd.retval == PASS_RET_NULL && dev->runCmd.hasReturned == false) {
                    DEBUG(DERROR, "(FATAL) - The function iodevRunLauncher failed to launch target %s@%s@%s", dev->targetName.c_str(), passName.c_str(), dev->deviceName.c_str());
                    return PASS_RET_FATAL;
                }

                /* Check if this Runtime Pass returned an error code */
                if (dev->runCmd.retval == DEV_RET_FATAL) {
                    /* This particular Runtime Pass is attempting
                       to force join / kill all of the other Runtime Threads */
                    DEBUG(DERROR, "(FATAL) Runtime pass %s@%s@%s requested global system shutdown", dev->targetName.c_str(), passName.c_str(), dev->deviceName.c_str());
                    return PASS_RET_FATAL;
                }
                else if (dev->runCmd.retval != DEV_RET_OK && dev->runCmd.retval != DEV_RET_NOTHINGTODO) {
                    DEBUG(DERROR, "Execution of target %s@%s@%s finished with errors", dev->targetName.c_str(), passName.c_str(), dev->deviceName.c_str());
                    /* We don't have to necessarily crash and burn the system just because this
                       particular device didn't like to behave */
                    return PASS_RET_OK;
                }
                else {
                    if(dev->runCmd.retval != DEV_RET_NOTHINGTODO)
                        DEBUG(DGOOD, "Returned from %s@%s@%s with retval %d",
                            dev->targetName.c_str(),
                            passName.c_str(),
                            dev->deviceName.c_str(),
                            dev->runCmd.retval);
                    return PASS_RET_OK;
                }
            }
        }
    }
    return PASS_RET_OK;
}

IOMachineModule::IOMachineModule() : RunPass(IOMACH_MODULE_PRIORITY),
liveThreads(0), isIOLive(false)
{

}

enum PassRetcode IOMachineModule::init()
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
        dev->ioContext = this;
        if (dev->init() != DEV_RET_OK) {
            DEBUG(DERROR, "Could not initialize IO device %s@%s@%s", dev->targetName.c_str(), passName.c_str(), dev->deviceName.c_str());
            return PASS_RET_ERR;
        }
    }

    return PASS_RET_OK;
}

enum PassRetcode IOMachineModule::finit()
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

enum PassRetcode IOMachineModule::run()
{
    enum PassRetcode success = PASS_RET_OK;
    enum PassStatus CPUModulePassStatus = PASS_STATUS_NULL;

    isIOLive = true;

    /* Launch all the devices' run() function all in separate threads */
    for (auto & dev : ioconf->device_list) {
        /* First, create an execution context */
        dev->runCmd.theRunningDevice = dev;
        dev->runCmd.retval = DEV_RET_NULL; /* If this stays null then we were unable to launch the device's run method */
        dev->runCmd.hasReturned = false;
        dev->alreadyJoined = false;

        /* Finally launch the device execution thread */
        DEBUG(DGOOD, "Launching IO device: %s", dev->deviceName.c_str());

        dev->runThread = std::unique_ptr<thread>(new thread(iodevRunLauncher, (void*)&dev->runCmd));

        /* One more thread running */
        liveThreads++;
    }

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
                success = PASS_RET_ERR;
                break;
            }

            if ((success = collectDevices()) != PASS_RET_OK) {
                /* Something bad happened while garbage collecting the devices */
                break;
            }
        }
        else
        {
            if (CPUModulePassStatus == PASS_STATUS_NOAUTH)
                success = PASS_RET_FATAL; /* The CPU did not give us permission to read its status. Bailing. */

            if(CPUModulePassStatus == PASS_STATUS_COMPLETED)
                success = PASS_RET_OK; /* The CPU has successfully finished its execution */

            /* At this point, the CPU has finished its execution with errors or warnings.
               We're getting outta here now. */

            if (CPUModulePassStatus == PASS_STATUS_COMPLETEDWITHERRORS)
                success = PASS_RET_ERR;

            if (CPUModulePassStatus == PASS_STATUS_COMPLETEDWITHFATALERRORS)
                success = PASS_RET_FATAL;

            break;
        }
    }

    isIOLive = false;

    /* Close all the devices' threads */
    while (liveThreads > 0) {
        /* Keep forcing the threads to close */
        if((success = collectDevices()) != PASS_RET_OK)
            break;
        /* At this point, not all devices have closed. 
           Not necessarily bad, unless they refuse to close. */
    }

    /* All threads collected */

    return success;
}

enum PassRetcode IOMachineModule::watchdog()
{
    return PASS_RET_OK;
}

/* Include all of the IO Devices into the system */
#include "VirtualMotherboard/VMoboInc.hpp"

}