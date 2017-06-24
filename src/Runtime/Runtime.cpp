#include <fvm/Runtime.h>
#include <fvm/TargetRegistry.h>
#include <fvm/Pass.h>
#include <algorithm>

#define POLLRATE_NS 0 /* The rate at which the passes are polled and joined, in nanoseconds */

uint32_t Runtime::liveThreads = 0;
bool Runtime::systemHealthy = true;

Runtime::Runtime(TargetRegistry * theTarget)
: theTarget(theTarget)
{

}

static void runErrorDebug(std::string errorMessage, TargetRegistry * theTarget, Pass * theFailingPass)
{
    theFailingPass->setStatus(PASS_STATUS_COMPLETEDWITHERRORS);
    theFailingPass->DEBUG(DERROR, errorMessage.c_str(), theTarget->targetName.c_str(), theFailingPass->passName.c_str());
}

static void runtimeLauncher(void * runtimeThreadArgs)
{
    runtimeLaunchCommandPacket_t * runCmd = (runtimeLaunchCommandPacket_t*)runtimeThreadArgs;
    if(runCmd && runCmd->theRuntimePass) {
        runCmd->retval = runCmd->theRuntimePass->run();
        runCmd->hasReturned = true;
    } else {
        /* Setting this to null will indicate the packet is invalid */
        runCmd->retval = PASS_RET_NULL;
    }
}

static std::string getPassListAsString(std::vector<Pass*> & passList)
{
    std::string retStr = "";
    for (size_t i = 0; i < passList.size(); i++)
        retStr += passList[i]->passName + " (" + std::to_string(passList[i]->priority) + ")" + (i < passList.size() - 1 ? ", " : "");
    return retStr;
}

bool Runtime::run(TargetRegistry * theTarget)
{
    std::vector<Pass*> sublistPassInitFinit;
    std::vector<Pass*> sublistPassConfig;
    std::vector<Pass*> sublistPassRun;

    /* Split the passList into different categories */
    for (auto pass : theTarget->passList)
    {
        switch (pass->type) {
        case PASS_RUNTIME_INIT_FINIT:
            sublistPassInitFinit.push_back(pass);
            break;
        case PASS_CONFIG:
            sublistPassConfig.push_back(pass);
            break;
        case PASS_RUNTIME:
            sublistPassRun.push_back(pass);
            break;
        }

        /* While we're at it, set the parent target context
           so that the passes can access each other */
        pass->setParentTargetContext(theTarget);
    }

    /* Sort each category by priority */
    std::sort(sublistPassInitFinit.begin(), sublistPassInitFinit.end(), [](const Pass*lhs, const Pass*rhs) {
        return lhs->priority < rhs->priority;
    });
    std::sort(sublistPassConfig.begin(), sublistPassConfig.end(), [](const Pass*lhs, const Pass*rhs) {
        return lhs->priority < rhs->priority;
    });
    std::sort(sublistPassRun.begin(), sublistPassRun.end(), [](const Pass*lhs, const Pass*rhs) {
        return lhs->priority < rhs->priority;
    });

    if(isDebuggingEnabled())
        debugTargetInformation(theTarget, sublistPassInitFinit, sublistPassConfig, sublistPassRun);

    theTarget->runContext.running = true;

    /* First, run global target initilization passes serially 
       (the 2 foreach loops were intentional) */
    for (auto initFinitPass : sublistPassInitFinit) {
        DEBUG(DINFO, "Initializing target", theTarget->targetName.c_str());
        initFinitPass->setStatus(PASS_STATUS_RUNNING);
        if (initFinitPass->init() != PASS_RET_OK) {
            runErrorDebug("Could not globally initialize target %s@%s", theTarget, initFinitPass);
            return false;
        }
    }
    for (auto initFinitPass : sublistPassInitFinit) {
        /* Initialize the machine at the implementation level */
        DEBUG(DINFO, "Initializing top-level implementation", theTarget->targetName.c_str());
        if (initFinitPass->run() != PASS_RET_OK) {
            runErrorDebug("Could not globally initialize the running parameters of target %s@%s", theTarget, initFinitPass);
            return false;
        }
        initFinitPass->setStatus(PASS_STATUS_PAUSED);
    }

    /* Now run all the config passes (also serially) */
    for (auto configPass : sublistPassConfig)
    {
        DEBUG(DINFO, "Running configuration pass %s", configPass->passName.c_str());
        configPass->setStatus(PASS_STATUS_RUNNING);
        if (configPass->init() != PASS_RET_OK) {
            runErrorDebug("Could not initialize the configurator of target %s@%s", theTarget, configPass);
            return false;
        }
        if (configPass->run() != PASS_RET_OK) {
            runErrorDebug("Could not configure the target %s@%s", theTarget, configPass);
            return false;
        }
        if (configPass->finit() != PASS_RET_OK) {
            runErrorDebug("Could not terminate configuration for target %s@%s", theTarget, configPass);
            return false;
        }
        configPass->setStatus(PASS_STATUS_COMPLETED);
    }

    /* Initialize all machine implementations serially */
    for (auto runPass : sublistPassRun) {
        DEBUG(DINFO, "Initializing low level core implementation of pass %s", runPass->passName.c_str());
        runPass->setStatus(PASS_STATUS_RUNNING);
        if (runPass->init() != PASS_RET_OK) {
            runErrorDebug("Could not initialize implementation of target %s@%s", theTarget, runPass);
            return false;
        }
    }

    /* Execute all machine implementations all in separate threads */
    for (auto runPass : sublistPassRun) {
        /* First, create an execution context */
        std::unique_ptr<runtimeThreadContext_t> runtimeThrd(new runtimeThreadContext_t);
        runtimeThrd->runCmd.theRuntimePass = runPass; /* Store the pass */
        runtimeThrd->runCmd.retval = PASS_RET_NULL; /* If this stays null then we were unable to launch the runtime pass */
        runtimeThrd->runCmd.hasReturned = false;
        runtimeThrd->alreadyJoined = false;
        
        /* Finally launch the target's implementation */
        DEBUG(DGOOD, "Launching runtime pass: %s", runPass->passName.c_str());
        
        runtimeThrd->theThread = std::unique_ptr<thread>(new thread(runtimeLauncher, (void*)&runtimeThrd->runCmd));
        
        /* One more thread running */
        liveThreads++;

        /* And of course, store its runtime context */
        theTarget->runContext.runtimeThreads.push_back(std::move(runtimeThrd));
    }

    DEBUG(DNORMALH, "\n");

    /* The main thread will now stay here polling each
       Watchdog pass, serving system resources to each passe and 
       will also be waiting for any close request
       from any pass */

    while (systemHealthy && liveThreads > 0) {
        /* Poll all of the runtime passes and their watchdog functions and
        service them shared functionality / resources */
        for (auto & runtimeThrd : theTarget->runContext.runtimeThreads) {
            enum RuntimeServiceRetcode retcode = pollRuntimePass(runtimeThrd->runCmd.theRuntimePass);
            if (retcode == RUNTIME_SERV_FATAL) {
                /* Something catastrophic happened */
                selfDestruct(RUNTIME_PANIC_SEVERITY_MAX, "(FATAL) Could not provide system resources to the target %s@%s", theTarget, runtimeThrd->runCmd.theRuntimePass);
                return false;
            }

            if (retcode == RUNTIME_SERV_WATCHDOG_EXPIRED) {
                /* The runtime's watchdog was unable to keep up. We must handle this */
                selfDestruct(RUNTIME_PANIC_SEVERITY_0, "The target %s@%s was unresponsive and did not fulfill its duty (watchdog expired). Bailing...", theTarget, runtimeThrd->runCmd.theRuntimePass);
                return false;
            }
            else if (retcode != RUNTIME_SERV_OK) {
                /* Something went wrong. Not necessarily serious, but we should check anyways. */
                runErrorDebug("Service polling returned with non-fatal errors on target %s@%s", theTarget, runtimeThrd->runCmd.theRuntimePass);
                runtimeThrd->runCmd.theRuntimePass->setStatus(retcode == RUNTIME_SERV_WARNING ? PASS_STATUS_RUNNINGWITHWARNINGS : retcode == RUNTIME_SERV_ERROR ? PASS_STATUS_RUNNINGWITHERRORS : PASS_STATUS_NULL);
            }
        }

        /* See if we need to join any of the passes' threads */
        for (auto & runtimeThrd : theTarget->runContext.runtimeThreads) {
            if (!runtimeThrd->alreadyJoined) {
                if (runtimeThrd->runCmd.hasReturned) {
                    runtimeThrd->theThread->join();
                    runtimeThrd->alreadyJoined = true;

                    if (liveThreads == 0) {
                        /* What? How? Why? When? How is this 0?
                           Well, one thing is certain, this thing is gonna crash! */
                        selfDestruct(RUNTIME_PANIC_SEVERITY_MAX, "(FATAL) Unable to join runtime's execution thread at target %s@%s", theTarget, runtimeThrd->runCmd.theRuntimePass);
                        return false;
                    }
                    
                    /* One thread down */
                    liveThreads--;

                    /* Check if we managed to launch the runtime pass at all */
                    if (runtimeThrd->runCmd.retval == PASS_RET_NULL && runtimeThrd->runCmd.hasReturned == false) {
                        selfDestruct(RUNTIME_PANIC_SEVERITY_MAX, "(FATAL) - The function runtimeLauncher failed to launch target %s@%s", theTarget, runtimeThrd->runCmd.theRuntimePass);
                        return false;
                    }

                    /* Check if this Runtime Pass returned an error code */
                    if (runtimeThrd->runCmd.retval == PASS_RET_FATAL) {
                        /* This particular Runtime Pass is attempting 
                           to force join / kill all of the other Runtime Threads */
                        selfDestruct(RUNTIME_PANIC_SEVERITY_2, "(FATAL) Runtime pass %s@%s requested global system shutdown", theTarget, runtimeThrd->runCmd.theRuntimePass);
                        return false;
                    } else if (runtimeThrd->runCmd.retval != PASS_RET_OK && runtimeThrd->runCmd.retval != PASS_RET_NOTHINGTODO) {
                        selfDestruct(RUNTIME_PANIC_SEVERITY_1, "Execution of target %s@%s finished with errors", theTarget, runtimeThrd->runCmd.theRuntimePass);
                        return false;
                    }
                    else {
                        runtimeThrd->runCmd.theRuntimePass->setStatus(PASS_STATUS_COMPLETED);
                        DEBUG(DGOOD, "Returned from %s@%s with retval %d", 
                            theTarget->targetName.c_str(), 
                            runtimeThrd->runCmd.theRuntimePass->passName.c_str(), 
                            runtimeThrd->runCmd.retval);
                    }
                }
            }
        }

#if POLLRATE_NS > 0
        /* We shall now sleep for a certain amount of time */
        this_thread::sleep_for(chrono::nanoseconds(POLLRATE_NS));
#endif
    }

    DEBUG(DNORMALH, "\n-------------------------------------------");

    /* Close and cleanup all machine implementations in reverse order and serially */
    for (int i = sublistPassRun.size() - 1; i >= 0; i--) {
        DEBUG(DINFO, "Terminating pass %s", sublistPassRun[i]->passName.c_str());
        if (sublistPassRun[i]->finit() != PASS_RET_OK) {
            runErrorDebug("Could not terminate implementation of target %s@%s", theTarget, sublistPassRun[i]);
            return false;
        }
        sublistPassRun[i]->setStatus(PASS_STATUS_COMPLETED);
    }
    
    /* Finally, run finit passes serially */
    for (auto initFinitPass : sublistPassInitFinit) {
        DEBUG(DINFO, "Terminating target %s", theTarget->targetName.c_str());
        initFinitPass->setStatus(PASS_STATUS_RUNNING);
        if (initFinitPass->finit() != PASS_RET_OK) {
            runErrorDebug("Could not globally terminate target %s@%s", theTarget, initFinitPass);
            return false;
        }
        initFinitPass->setStatus(PASS_STATUS_COMPLETED);
    }

    theTarget->runContext.running = false;
    return true;
}

void Runtime::selfDestruct(enum RuntimePanicSeverity severity, std::string lastWords, TargetRegistry * theTarget, Pass* responsiblePass)
{
    /* Kill the system */
    systemHealthy = false; /* There's no coming back after setting this to false. The system WILL shut down */
    liveThreads = 0;
    enableDebugging(); /* Force debugging messages to show up */
    runErrorDebug(lastWords, theTarget, responsiblePass);
    if(severity >= RUNTIME_PANIC_SEVERITY_MAX)
        responsiblePass->setStatus(PASS_STATUS_COMPLETEDWITHFATALERRORS);
    panic(severity, theTarget);
}

void Runtime::panic(enum RuntimePanicSeverity severity, TargetRegistry * theTarget)
{
    DEBUG(DERROR, "                                                      ");
    DEBUG(DERROR, "            ! S Y S T E M    P A N I C !              ");
    DEBUG(DERROR, "                                                      ");
    DEBUG(DERROR, "Severity level: %d", severity);

    /* Close all system and runtime threads */
    for (auto & runtimeThrd : theTarget->runContext.runtimeThreads)
        runtimeThrd->theThread->detach();
}

bool Runtime::launchTarget(std::string targetName)
{
    for (auto target : TargetRegistry::TheTargetList)
        if (strTolower(target->targetName) == strTolower(targetName)) {
            bool success = run(target);
            target->runContext.running = false;
            return success;
        }

    /* Target not found */
    DEBUG(DERROR, "Could not find target '%s'!", targetName.c_str());
    return false;
}

bool Runtime::launchTarget(unsigned int targetIndex)
{
    if (targetIndex >= 0 && targetIndex <  TargetRegistry::TheTargetList.size()) {
        bool success = run(TargetRegistry::TheTargetList[targetIndex]);
        TargetRegistry::TheTargetList[targetIndex]->runContext.running = false;
        return success;
    }
    else {
        /* Target not found */
        DEBUG(DERROR, "Could not find the selected target with index %d!", targetIndex);
        return false;
    }
}

enum RuntimeServiceRetcode Runtime::pollRuntimePass(Pass * runtimePass)
{
    /* TODO */
    return RUNTIME_SERV_OK;
}

void Runtime::debugTargetInformation(TargetRegistry * theTarget,
                                     std::vector<Pass*> & sublistPassInitFinit, 
                                     std::vector<Pass*> & sublistPassConfig, 
                                     std::vector<Pass*> & sublistPassRun)
{
    DEBUG(DINFO, " -- Target information --");

    DEBUG(DINFO, "Developed by: %s", theTarget->targetOwnerDescription.c_str());
    DEBUG(DINFO, "Full target name: %s", theTarget->targetNameLong.c_str());
    DEBUG(DINFO, "Total amount of target passes: %d", theTarget->passList.size());
    DEBUG(DINFO, "  Initialization and Termination passes - total: %d | %s", sublistPassInitFinit.size(), getPassListAsString(sublistPassInitFinit).c_str());
    DEBUG(DINFO, "  Configuration passes - total: %d | %s", sublistPassConfig.size(), getPassListAsString(sublistPassConfig).c_str());
    DEBUG(DINFO, "  Runtime passes - total: %d | %s", sublistPassRun.size(), getPassListAsString(sublistPassRun).c_str());

    DEBUG(DINFO, " -- Target information -- End");
    DEBUG(DNORMALH, "\n-------------------------------------------");
}