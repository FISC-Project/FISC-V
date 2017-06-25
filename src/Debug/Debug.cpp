#include <fvm/Debug/Debug.h>
#include <fvm/Utils/String.h>
#include <fvm/TargetRegistry.h>
#include <TinyThread++-1.1/tinythread.h>
#include <TinyThread++-1.1/fast_mutex.h>
#include <vector>
#include <iostream>

using namespace tthread;

#define LOCK(mut) lock_guard<mutex> lock(mut)

static mutex glob_debug_mutex;

static bool debugging = false;
static bool isDebuggingInitialized = false;
static bool colorEnabled = true;
static bool firstLine = true;
static enum DEBUG_LEVEL debugLevel = DALL;
static std::vector<debugTypeEntry_t> debugTypeEntries;

void initializeDebugging()
{
	/* Create standard debug types and kinds used by the VM */
	setNewDefaultDebugType(DVM, "VM", DEBUGTYPEENTRY_OWNER_VM);
	isDebuggingInitialized = true;
}

void enableDebugging()
{
	if(!isDebuggingInitialized)
		initializeDebugging();
	debugging = true;
}

void disableDebugging()
{
	debugging = false;
}

bool setDebuggingLevel(enum DEBUG_LEVEL level)
{
	if(isDebuggingEnabled() && level > DEBUG_LEVEL_NULL && level < DEBUG_LEVEL__COUNT)
		debugLevel = level;
	else
		return false;
	return true;
}

enum DEBUG_LEVEL getDebuggingLevel()
{
	return debugLevel;
}

bool isDebuggingEnabled()
{
	return debugging;
}

std::string debugLevelToStr(enum DEBUG_LEVEL level)
{
	switch (level) {
	case DNONE: return "NONE";
	case DL1:   return "LVL 1";
	case DL2:   return "LVL 2";
	case DL3:   return "LVL 3";
	case DALL:  return "ALL";
	default:    return "UNKNOWN LEVEL";
	}
}

bool setNewDebugType(debugTypeEntry_t newDebugTypeEntry)
{
	/* See if this debug entry type already exists */
	for (debugTypeEntry_t entry : debugTypeEntries) {
		if(entry.level == newDebugTypeEntry.level && strTolower(entry.levelName) == strTolower(newDebugTypeEntry.levelName) &&
		   entry.type  == newDebugTypeEntry.type  && strTolower(entry.typeName)  == strTolower(newDebugTypeEntry.typeName)  &&
		   entry.kind  == newDebugTypeEntry.kind  && strTolower(entry.kindName)  == strTolower(newDebugTypeEntry.kindName))
			return false;
	}

	/* It doesn't. Push it into the debug entry type list */
	debugTypeEntries.push_back(newDebugTypeEntry);	
	return true;
}

bool setNewDefaultDebugType(enum DEBUG_KIND kind, std::string kindName, TargetRegistry * targetOwner)
{
	debugTypeEntries.push_back(debugTypeEntry_t{DNONE, DNORMAL,  kind, debugLevelToStr(DNONE), "INFO",    kindName, targetOwner }); /* The debug type that is used when there is no debugging                                          */
	debugTypeEntries.push_back(debugTypeEntry_t{DALL,  DNORMAL,  kind, debugLevelToStr(DALL),  "INFO",    kindName, targetOwner }); /* The debug type that is used as a way to print normal text without potential meaning             */
	debugTypeEntries.push_back(debugTypeEntry_t{DALL,  DNORMALH, kind, debugLevelToStr(DALL),  "INFO",    kindName, targetOwner }); /* The debug type that is used as a way to print normal text without potential meaning (no header) */
	debugTypeEntries.push_back(debugTypeEntry_t{DALL,  DGOOD,    kind, debugLevelToStr(DALL),  "INFO",    kindName, targetOwner }); /* The debug type that is used to print valid information                                          */
	debugTypeEntries.push_back(debugTypeEntry_t{DALL,  DINFO,    kind, debugLevelToStr(DALL),  "INFO",    kindName, targetOwner }); /* The debug type that is used to print general purpose information                                */
	debugTypeEntries.push_back(debugTypeEntry_t{DALL,  DINFO2,   kind, debugLevelToStr(DALL),  "INFO",    kindName, targetOwner }); /* The debug type that is used to print general purpose information                                */
	debugTypeEntries.push_back(debugTypeEntry_t{DALL,  DWARN,    kind, debugLevelToStr(DALL),  "WARNING", kindName, targetOwner }); /* The debug type that is used to print warnings                                                   */
	debugTypeEntries.push_back(debugTypeEntry_t{DALL,  DERROR,   kind, debugLevelToStr(DALL),  "ERROR",   kindName, targetOwner }); /* The debug type that is used to print errors                                                     */
	return true;
}

namespace debug {

bool changeDebugLevel(std::string kindName, enum DEBUG_TYPE type, enum DEBUG_LEVEL newLevel)
{
	for (unsigned int i = 0; i < debugTypeEntries.size(); i++) {
		if (strTolower(debugTypeEntries[i].kindName) == strTolower(kindName) && debugTypeEntries[i].type == type) {
			debugTypeEntries[i].level = newLevel;
			debugTypeEntries[i].levelName = debugLevelToStr(newLevel);
			return true;
		}
	}
	return false;
}

bool changeDebugLevel(std::string kindName, enum DEBUG_LEVEL newLevel)
{
	bool found = false;
	for (unsigned int i = 0; i < debugTypeEntries.size(); i++) {
		if (strTolower(debugTypeEntries[i].kindName) == strTolower(kindName)) {
			debugTypeEntries[i].level = newLevel;
			debugTypeEntries[i].levelName = debugLevelToStr(newLevel);
			found = true;
		}
	}
	return found;
}

}

void debugEnableDisableColor(bool enable)
{
	colorEnabled = enable;
}

#ifdef __linux__

static bool raw_print(std::string str, debugTypeEntry_t * debugTypeEntry, bool isMsgHeader)
{
	/* TODO */
}

#elif _WIN32

#include <fvm/Host/Windows/WindowsAPI.h>

bool raw_print(std::string str, debugTypeEntry_t * debugTypeEntry, bool isMsgHeader)
{
	WORD colorAttribute = winapi_console_getDefaultAttr();
	BOOL success = FALSE;

	if(debugTypeEntry)
		switch (debugTypeEntry->type) {
		case DNORMAL: case DNORMALH:
			if(colorEnabled) colorAttribute = winapi_console_getDefaultAttr();
			success = winapi_console_print(std::cout, str, colorAttribute);
			break;
		case DGOOD:
			if (colorEnabled) {
				if (isMsgHeader) colorAttribute = GREEN_FADE_BACKGROUND | WHITE_TEXT;
				else colorAttribute = WHITE_TEXT;
			}
			success = winapi_console_print(std::cout, str, colorAttribute);
			break;
		case DINFO: 
			if (colorEnabled) {
				if(isMsgHeader) colorAttribute = BLUE_BACKGROUND | WHITE_TEXT;
				else colorAttribute = WHITE_TEXT;
			}
			success = winapi_console_print(std::clog, str, colorAttribute);
			break;
		case DINFO2:
			if (colorEnabled) {
				if (isMsgHeader) colorAttribute = BLUE_BACKGROUND | WHITE_TEXT;
				else colorAttribute = BLUE_BACKGROUND | WHITE_TEXT;
			}
			success = winapi_console_print(std::clog, str, colorAttribute);
			break;
		case DWARN:
			if (colorEnabled) {
				if (isMsgHeader) colorAttribute = YELLOW_BACKGROUND;
				else colorAttribute = WHITE_FADE_BACKGROUND;
			}
			success = winapi_console_print(std::cerr, str, colorAttribute);
			break;
		case DERROR:
			if (colorEnabled) {
				if (isMsgHeader) colorAttribute = RED_BACKGROUND | WHITE_TEXT;
				else colorAttribute = RED_FADE_BACKGROUND | WHITE_TEXT;
			}
			success = winapi_console_print(std::cerr, str, colorAttribute);
			break;
		}
	else {
		colorAttribute = winapi_console_getDefaultAttr();
		success = winapi_console_print(std::cout, str, colorAttribute);
	}
		
	/* Restore console attributes */
	winapi_console_setAttr(winapi_console_getDefaultAttr());
	return success == TRUE ? true : false;
}

#endif

namespace debug {

bool DEBUG(enum DEBUG_KIND kind, std::string kindName, bool kindByName, enum DEBUG_TYPE type, bool override_flag, std::string fmt, va_list args)
{
	LOCK(glob_debug_mutex);
	static char debugBuff[2048];
	static char debugBuffHeader[128];
	debugTypeEntry_t * debugTypeEntry = nullptr;

	if(!override_flag)
		if (!isDebuggingEnabled())
			return false;

	if (!isDebuggingInitialized)
		initializeDebugging();

	for (unsigned int i = 0; i < debugTypeEntries.size(); i++) {
		if (kindByName) {
			if (debugTypeEntries[i].type == type && strTolower(debugTypeEntries[i].kindName) == strTolower(kindName)) {
				debugTypeEntry = &debugTypeEntries[i];
				break;
			}
		}
		else {
			if (debugTypeEntries[i].type == type && debugTypeEntries[i].kind == kind) {
				debugTypeEntry = &debugTypeEntries[i];
				break;
			}
		}

	}

	if (debugTypeEntry == nullptr)
		return false;

	/* Check if the debug level condition can be satisfied */
	if (!override_flag) {
		if(debugTypeEntry->level > debugLevel)
			return false; /* Oops, the current debug level is too low for this entry. Can't show the debug message */
	}

	if (!override_flag && type != DNORMALH) {
		std::string targetName;
		std::string passName;
		if (debugTypeEntry->targetOwner == DEBUGTYPEENTRY_OWNER_VM) {
			targetName = "VM";
			passName = "";
		}
		else {
			targetName = debugTypeEntry->targetOwner->targetName;
			passName = "@" + debugTypeEntry->kindName;
		}

		if(debugTypeEntry->level != DNONE) {
			sprintf(debugBuffHeader, "%s (%s%s, %s)",
					debugTypeEntry->typeName.c_str(),
					targetName.c_str(),
					passName.c_str(),
					debugTypeEntry->levelName.c_str());
		} else {
			sprintf(debugBuffHeader, "%s (%s%s)",
				debugTypeEntry->typeName.c_str(),
				targetName.c_str(),
				passName.c_str());
		}
		
		if(firstLine) {
			raw_print("> ", nullptr, false);
			firstLine = false;
		} else {
			raw_print("\n> ", nullptr, false);
		}

		raw_print(std::string(debugBuffHeader), debugTypeEntry, true);
		fmt = ": " + fmt; /* Prepend colon and space */
	}

	vsprintf(debugBuff, fmt.c_str(), args);
	return raw_print(std::string(debugBuff), debugTypeEntry, false);
}

bool DEBUG(enum DEBUG_KIND kind, enum DEBUG_TYPE type, bool override_flag, std::string fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	bool success = DEBUG(kind, "", false, type, override_flag, fmt, args);
	va_end(args);
	return success;
}

bool DEBUG(enum DEBUG_KIND kind, enum DEBUG_TYPE type, std::string fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	bool success = DEBUG(kind, "", false, type, false, fmt, args);
	va_end(args);
	return success;
}

bool DEBUG(std::string kindName, enum DEBUG_TYPE type, bool override_flag, std::string fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	bool success = DEBUG(DEBUG_KIND_NULL, kindName, true, type, override_flag, fmt, args);
	va_end(args);
	return success;
}

bool DEBUG(std::string kindName, enum DEBUG_TYPE type, std::string fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	bool success = DEBUG(DEBUG_KIND_NULL, kindName, true, type, false, fmt, args);
	va_end(args);
	return success;
}

bool DEBUG(enum DEBUG_TYPE type, std::string fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	bool success = DEBUG(DVM, "", false, type, false, fmt, args);
	va_end(args);
	return success;
}

bool DEBUG(std::string fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	bool success = DEBUG(DVM, "", false, DNORMAL, false, fmt, args);
	va_end(args);
	return success;
}

}