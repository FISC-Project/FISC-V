/*                                                                            
                        __  __           _       _                                   
                       |  \/  |         | |     | |     _                            
                       | \  / | ___   __| |_   _| | ___(_)                           
                       | |\/| |/ _ \ / _` | | | | |/ _ \                             
                       | |  | | (_) | (_| | |_| | |  __/_                            
                       |_|  |_|\___/ \__,_|\__,_|_|\___(_)                           
   _____ _____  _    _    _____             __ _                       _             
  / ____|  __ \| |  | |  / ____|           / _(_)                     | |            
 | |    | |__) | |  | | | |     ___  _ __ | |_ _  __ _ _   _ _ __ __ _| |_ ___  _ __ 
 | |    |  ___/| |  | | | |    / _ \| '_ \|  _| |/ _` | | | | '__/ _` | __/ _ \| '__|
 | |____| |    | |__| | | |___| (_) | | | | | | | (_| | |_| | | | (_| | || (_) | |   
  \_____|_|     \____/   \_____\___/|_| |_|_| |_|\__, |\__,_|_|  \__,_|\__\___/|_|   
                                                  __/ |                              
                                                 |___/                               
*/

 /*------------------------------------------------------------------------
 - FILE NAME: FISCCPUConfigurator.cpp
 - MODULE NAME: CPU Configurator
 - PURPOSE: To define the structure of the Central Processing Unit - CPU
 - AUTHOR: MIGUEL SANTOS
 -------------------------------------------------------------------------*/

#pragma once
#include <fvm/Pass.h>
#include "ISA/FISCISA.h"
#include <map>

namespace FISC {

static Instruction ** instruction_list_realloc = nullptr;
static unsigned int instruction_list_size = 0;
static bool instruction_list_success_declared = true;

class CPUConfigurator : public ConfigPass {
private:
    #define DEFAULT_ALIGN_BASE   (0) /* Base   address alignment is disabled by default */
    #define DEFAULT_ALIGN_OFFSET (0) /* Offset address alignment is disabled by default */

    /* List of permissions for external Passes that want to use the resources of this Pass */
    #define WHITELIST_CPU_CONF {DECL_WHITELIST_ALL(CPUModule)}

public:
    std::map<uint16_t, Instruction*> instruction_list;

    /***********************/
    /* Register Definition */
    /***********************/
    uint64_t x[FISC_REGISTER_COUNT]; /* General purpose 64-bit registers                  */
    uint32_t pc;      /* Program Counter                                                  */
    uint32_t esr;     /* Exception Syndrome Register                                      */
    uint64_t elr;     /* Exception Link / Return Register                                 */
    cpsr_t   cpsr;    /* Current Processor Status Register                                */
    cpsr_t   spsr[6]; /* Saved Processor Status Register (6 of them, 1 for each CPU mode) */
    uint64_t ivp;     /* Interrupt Vector Pointer                                         */
    uint64_t evp;     /* Exception Vector Pointer                                         */
    uint64_t pdp;     /* Page Directory Pointer                                           */
    uint64_t pfla;    /* Page Fault Linear Address                                        */
    
public:
    CPUConfigurator() : ConfigPass(2) 
    {
        setWhitelist(WHITELIST_CPU_CONF);
    }

    enum PassRetcode init()
    {
        enum PassRetcode success = PASS_RET_OK;
        DEBUG(DGOOD, "Initializing CPU");

        /* "Install" all statically declared instructions into a nice safe vector */
        
        if (instruction_list_size > 0 && instruction_list_realloc != nullptr) {
            /* Good, we found some instructions declared (if we didn't it'd be a huge problem) */
            for(unsigned int i = 0; i < instruction_list_size; i++) {
                if (instruction_list_realloc[i] == nullptr) {
                    success = PASS_RET_ERR;
                    break;
                }
                instruction_list_realloc[i]->targetName = getTarget()->targetName;
                instruction_list[instruction_list_realloc[i]->opcodeShifted] = instruction_list_realloc[i];
            }

            if (success == PASS_RET_OK) {
                DEBUG(DGOOD, "CPU supports %d unique instructions", instruction_list_size);

                /* Clear flags */
                cpsr.n = cpsr.z = cpsr.v = cpsr.c = 0;

                /* Set the two base and offset alignment bits */
                cpsr.ae = (DEFAULT_ALIGN_OFFSET << 1) | DEFAULT_ALIGN_BASE;

                /* Paging is disabled by default */
                cpsr.pg = 0;

                /* Interrupts are disabled by default */
                cpsr.ien = 0;

                /* Set default CPU execution mode */
                cpsr.mode = FISC_DEFAULT_EXEC_MODE;

                /* Clear all SPSR registers */
                for(unsigned i = 0; i < 6; i++)
                    spsr[i].ae = spsr[i].c = spsr[i].ien = spsr[i].mode = 
                        spsr[i].n = spsr[i].pg = spsr[i].v = spsr[i].z = 0;
                
                /* Save the CPSR register to the SPSR of the current CPU mode */
                spsr[cpsr.mode] = cpsr;

                /* Cleanup the unsafe instruction list now */
                free(instruction_list_realloc);
                
                /* This variable is of no concern to any unauthorized external Pass, thus we're zeroing it */
                instruction_list_size = 0;
            }
        } else {
            /* Wtf? No instructions were declared? 
               How will the CPU execute instructions without supporting any instruction?... */
            success = PASS_RET_ERR;
        }

        return success;
    }

    enum PassRetcode finit()
    {
        return PASS_RET_OK;
    }

    enum PassRetcode run()
    {
        return PASS_RET_OK;
    }

    enum PassRetcode watchdog()
    {
        return PASS_RET_OK;
    }
};
}
/*
         ______           _          __   __  __           _       _                 
        |  ____|         | |        / _| |  \/  |         | |     | |     _          
        | |__   _ __   __| |   ___ | |_  | \  / | ___   __| |_   _| | ___(_)         
        |  __| | '_ \ / _` |  / _ \|  _| | |\/| |/ _ \ / _` | | | | |/ _ \           
        | |____| | | | (_| | | (_) | |   | |  | | (_) | (_| | |_| | |  __/_          
        |______|_| |_|\__,_|  \___/|_|   |_|  |_|\___/ \__,_|\__,_|_|\___(_)         
   _____ _____  _    _    _____             __ _                       _             
  / ____|  __ \| |  | |  / ____|           / _(_)                     | |            
 | |    | |__) | |  | | | |     ___  _ __ | |_ _  __ _ _   _ _ __ __ _| |_ ___  _ __ 
 | |    |  ___/| |  | | | |    / _ \| '_ \|  _| |/ _` | | | | '__/ _` | __/ _ \| '__|
 | |____| |    | |__| | | |___| (_) | | | | | | | (_| | |_| | | | (_| | || (_) | |   
  \_____|_|     \____/   \_____\___/|_| |_|_| |_|\__, |\__,_|_|  \__,_|\__\___/|_|   
                                                  __/ |                              
                                                 |___/                               
*/