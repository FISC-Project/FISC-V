#include <fvm/TargetRegistry.h>

/******************************************************************/
/****** EACH INCLUDE IS A MODULAR PASS THAT WILL BE EXECUTED ******/
/******************************************************************/

/******************************************************************
    MODULE NAME - 
        FISC INIT/FINIT

    DESCRIPTION - 
        Initializes the target at a higher level (in respect to the host VM) */
#include "InitFinit/FISCInitFinit.cpp"
/******************************************************************/

/******************************************************************
    MODULE NAME -
        FISC ERROR 

    DESCRIPTION -
        Handles all of the non-implementation errors of the target at the VM level.
        It could also handle some implementation errors if the programmer wishes
        to do so. */
#include "Error/FISCTargetError.cpp"
#include "Error/FISCMachineError.cpp"
/******************************************************************/

/******************************************************************
    MODULE NAME -
        FISC IO

    DESCRIPTION -
        Defines the structure and behaviour of the IO system / VirtualMotherboard of our target. */
#include "IO/FISCIOMachineConfigurator.cpp"
#include "IO/FISCIOMachineModule.cpp"
/******************************************************************/

/******************************************************************
    MODULE NAME -
        FISC MEMORY
    
    DESCRIPTION -
        Defines the structure and behaviour of the Main Memory / RAM of our target. */
#include "Memory/FISCMemoryConfigurator.cpp"
#include "Memory/FISCMemoryModule.cpp"
/******************************************************************/

/******************************************************************
    MODULE NAME -
        FISC CPU
    
    DESCRIPTION -
        Defines the structure and behaviour of the CPU of our target. This is the most important module. */
#include "CPU/FISCCPUConfigurator.cpp"
#include "CPU/FISCCPUModule.cpp"
/******************************************************************/

/*******R*E*G*I*S*T*E*R*****T*H*E****T*A*R*G*E*T*****H*E*R*E*******/
REGISTER_TARGET(FISC, FISC - Flexible Instruction Set Computer, Miguel Santos,
    InitFinit,
    IOMachineConfigurator, IOMachineModule,
    MemoryConfigurator,    MemoryModule,
    CPUConfigurator,       CPUModule
);
/*******R*E*G*I*S*T*E*R*****T*H*E****T*A*R*G*E*T*****H*E*R*E*******/
