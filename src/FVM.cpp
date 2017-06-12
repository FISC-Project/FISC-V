#include <fvm/TargetRegistry.h>
#include <fvm/Utils/Cmdline.h>
#include <stdio.h>

std::vector<TargetRegistry*> TargetRegistry::TheTargetList;

#include <Target/TargetList.h>

static std::string launchTargetName = NULLSTR;
static bool enableHeaderFooter = true;

void setCmdlineFlags()
{
    if (cmdHasOpt('d') || cmdHasOpt("debug")) {
        enableDebugging();
        DEBUG(DINFO, "Debugging is enabled");
        std::string debugLevel = cmdQuery('d').second;
        if (debugLevel == NULLSTR) {
            debugLevel = cmdQuery("debug").second;
            if (debugLevel != NULLSTR) {
                try {
                    DEBUG(DINFO, "Debug level: %s", debugLevel.c_str());
                    if (!setDebuggingLevel((enum DEBUG_LEVEL)std::stoi(debugLevel)))
                        throw - 1;
                }
                catch (...) {
                    DEBUG(DWARN, "Debug level '%s' is invalid", debugLevel.c_str());
                }
            }
        }
        else {
            try {
                DEBUG(DINFO, "Debug level: %s", debugLevel.c_str());
                if (!setDebuggingLevel((enum DEBUG_LEVEL)std::stoi(debugLevel)))
                    throw - 1;
            }
            catch (...) {
                DEBUG(DERROR, "Debug level '%s' is invalid", debugLevel.c_str());
            }
        }
    }
    
    if (cmdHasOpt('t'))
        launchTargetName = cmdQuery('t').second;
    if (cmdHasOpt("target"))
        launchTargetName = cmdQuery("target").second;
}

void showProgramHeader()
{
    std::string header = 
"                                            \n\
           FISC Virtual Machine             \n\
                                            \n";
    PRINTC(DINFO2, header.c_str());
}

int main(int argc, char ** argv)
{
    bool success = false;

    /* Parse the command line */
    cmdlineParse(argc, argv);

    if(cmdHasOpt('c') || cmdHasOpt("nocolor"))
        debugEnableDisableColor(false);

    if(enableHeaderFooter = !((cmdHasOpt('n') || cmdHasOpt("noheader"))))
        showProgramHeader();

    /****************************/
    /* Setup command line flags */
    /****************************/
    setCmdlineFlags();
    
    if ((success = (launchTargetName == NULLSTR))) {
        DEBUG(DERROR, "You must provide the flag -t <target> or --target <target>");
    } else {
        DEBUG(DGOOD, "Launching %s target . . .", launchTargetName.c_str());
        DEBUG(DNORMALH, "\n-------------------------------------------");
        if (!(success = TargetRegistry::launchTarget(launchTargetName))) {
            DEBUG(DERROR, "Execution of target %s failed", launchTargetName.c_str());
        }
    }
    
    if (enableHeaderFooter) {
        DEBUG(DNORMALH, "\n-------------------------------------------\n> ");
        PRINTC(DINFO2, "Finished executing Virtual Machine %s", success ? "(SUCCESS)" : "(FAILED) ");
    }
    return 0;
}
