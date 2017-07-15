#ifndef FISCIOMACHINEMODULE_H_
#define FISCIOMACHINEMODULE_H_

#include <fvm/Pass.h>

namespace FISC {

class CPUModule;
class IOMachineConfigurator;

class IOMachineModule : public RunPass {
#pragma region REGION 1: THE IO MACHINE CONFIGURATION DATA
public:
    /* Pass properties */
    #define IOMACH_MODULE_PRIORITY 1 /* The execution priority of this module */

    #define IOMACH_MODULE_CPUPOLLRATE_NS 1000000 /* The rate at which the IO Machine Module checks if the CPU is still running, in nanoseconds */
#pragma endregion

#pragma region REGION 2: THE IO MACHINE STRUCTURE DEFINITION (IMPL. SPECIFIC)
private:
    IOMachineConfigurator * ioconf; /* The configuration + structure of the IO Controller and Virtual Motherboard */
    CPUModule * cpu;                /* The handle for the CPU itself                                              */
    uint32_t liveThreads;
    bool isIOLive;
#pragma endregion

#pragma region REGION 3: THE IO MACHINE BEHAVIOUR IMPLEMENTATION (IMPL SPECIFIC)
public:
    bool isLive();
private:
    enum DevRetcode pollGlobalIO();
    enum PassRetcode collectDevices();
#pragma endregion

#pragma region REGION 4: THE IO MACHINE BEHAVIOUR (GENERIC VM FUNCTIONS)
public:
    IOMachineModule();
    enum PassRetcode init();
    enum PassRetcode finit();
    enum PassRetcode run();
    enum PassRetcode watchdog();
#pragma endregion

};
}
#endif