#include <fvm/TargetRegistry.h>

/******************************************************************/
/****** EACH INCLUDE IS A MODULAR PASS THAT WILL BE EXECUTED ******/
/******************************************************************/

/*******************/
/* FISC INIT/FINIT */
/*******************/
#include "InitFinit/FISCInitFinit.cpp"

/**************/
/* FISC ERROR */
/**************/
#include "Error/FISCTargetError.cpp"
#include "Error/FISCMachineError.cpp"

/************/
/* FISC IO */
/***********/
#include "IO/FISCIOMachineConfigurator.cpp"
#include "IO/FISCIOMachineModule.cpp"

/***************/
/* FISC MEMORY */
/***************/
#include "Memory/FISCMemoryConfigurator.cpp"
#include "Memory/FISCMemoryModule.cpp"

/*****************/
/* FISC PIPELINE */
/*****************/
#include "Pipeline/FISCPipelineConfigurator.cpp"
#include "Pipeline/FISCPipelineModule.cpp"

REGISTER_TARGET(FISC, InitFinit, PipelineModule);
