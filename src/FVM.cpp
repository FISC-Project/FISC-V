#include <fvm/TargetRegistry.h>
#include <fvm/Runtime.h>
#include <fvm/Utils/Cmdline.h>
#include <stdio.h>
#include <TinyThread++-1.1/tinythread.h>

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
                    enum DEBUG_LEVEL lvl = (enum DEBUG_LEVEL)std::stoi(debugLevel);
                    DEBUG(DINFO, "Debug level: %s", debugLevelToStr(lvl).c_str());
                    if (!setDebuggingLevel(lvl))
                        throw - 1;
                }
                catch (...) {
                    DEBUG(DWARN, "Debug level '%s' is invalid", debugLevel.c_str());
                }
            }
            else {
                /* No debug level was provided. We're assuming maximum debug level  */
                setDebuggingLevel(DALL);
            }
        }
        else {
            try {
                enum DEBUG_LEVEL lvl = (enum DEBUG_LEVEL)std::stoi(debugLevel);
                DEBUG(DINFO, "Debug level: %s", debugLevelToStr(lvl).c_str());
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

void debugHostSystemStatus()
{
    DEBUG(DINFO, " -- System information --");
    DEBUG(DINFO, "Number of CPU cores: %d", thread::hardware_concurrency());
    DEBUG(DINFO, " -- System information -- End");
}

void showProgramHeader()
{
    std::string header = 

"                                _____ ___ ____   ____                                   \n\
                               |  ___|_ _/ ___| / ___|                                  \n\
                               | |_   | |\\___ \\| |                                      \n\
                               |  _|  | | ___) | |___                                   \n\
                               |_|   |___|____/ \\____|                                  \n\
 __     _____ ____ _____ _   _   _    _       __  __    _    ____ _   _ ___ _   _ _____ \n\
 \\ \\   / /_ _|  _ \\_   _| | | | / \\  | |     |  \\/  |  / \\  / ___| | | |_ _| \\ | | ____|\n\
  \\ \\ / / | || |_) || | | | | |/ _ \\ | |     | |\\/| | / _ \\| |   | |_| || ||  \\| |  _|  \n\
   \\ V /  | ||  _ < | | | |_| / ___ \\| |___  | |  | |/ ___ \\ |___|  _  || || |\\  | |___ \n\
    \\_/  |___|_| \\_\\|_|  \\___/_/   \\_\\_____| |_|  |_/_/   \\_\\____|_| |_|___|_| \\_|_____|\n\
                                                                                        \n\
";

//"                                            \n\
//           FISC Virtual Machine             \n\
//                                            \n";
    PRINTC(DINFO2, header.c_str());
}

int main(int argc, char ** argv)
{
    bool success = false;

    /* Parse the command line */
    cmdlineParse(argc, argv);

    if(!(cmdHasOpt('c') || cmdHasOpt("nocolor")))
        debugEnableDisableColor(false);

    if(enableHeaderFooter = !((cmdHasOpt('n') || cmdHasOpt("noheader"))))
        showProgramHeader();

    /****************************/
    /* Setup command line flags */
    /****************************/
    setCmdlineFlags();

    if(isDebuggingEnabled())
        debugHostSystemStatus();

    if ((success = (launchTargetName == NULLSTR))) {
        DEBUG(DERROR, "You must provide the flag -t <target> or --target <target>");
    } else {
        DEBUG(DINFO, "Launching target %s ...", launchTargetName.c_str());
        DEBUG(DNORMALH, "\n-------------------------------------------");
        if (!(success = Runtime::launchTarget(launchTargetName))) {
            DEBUG(DERROR, "Execution of target %s failed", launchTargetName.c_str());
        }
    }
    
    if (enableHeaderFooter) {
        if(getDebuggingLevel() < DALL)
            PRINTC(DINFO2, "\n");
        DEBUG(DNORMALH, "\n-------------------------------------------\n> ");
        PRINTC(DINFO2, "Finished executing Virtual Machine %s", success ? "(SUCCESS)" : "(FAILED) ");
    }
    return 0;
}
