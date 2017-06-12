#ifndef DEBUG_H_
#define DEBUG_H_

#include <string>

enum DEBUG_LEVEL {
	DEBUG_LEVEL_NULL = -1,
	DNONE,
	DALL,
	DEBUG_LEVEL__COUNT
};

enum DEBUG_TYPE {
	DEBUG_TYPE_NULL = -1,
	DNORMAL,
	DNORMALH,
	DGOOD,
	DINFO,
	DINFO2,
	DWARN,
	DERROR,
	DEBUG_TYPE__COUNT
};

enum DEBUG_KIND {
	DEBUG_KIND_NULL = -1,
	DVM,
	DCUSTOM,
	DEBUG_KIND__COUNT
};

typedef struct {
	enum DEBUG_LEVEL level;
	enum DEBUG_TYPE type;
	enum DEBUG_KIND kind;
	std::string levelName;
	std::string typeName;
	std::string kindName;
} debugTypeEntry_t;

extern void enableDebugging();
extern void disableDebugging();
extern bool setDebuggingLevel(enum DEBUG_LEVEL level);
extern enum DEBUG_LEVEL getDebuggingLevel();
extern bool isDebuggingEnabled();
extern bool setNewDebugType(debugTypeEntry_t newDebugTypeEntry);
extern void debugEnableDisableColor(bool enable);

extern bool DEBUG(enum DEBUG_KIND kind, enum DEBUG_TYPE type, bool override_flag, std::string fmt, ...);
extern bool DEBUG(enum DEBUG_KIND kind, enum DEBUG_TYPE type, std::string fmt, ...);
extern bool DEBUG(enum DEBUG_TYPE type, std::string fmt, ...);
extern bool DEBUG(std::string fmt, ...);

#define PRINT(fmt, ...) DEBUG(DVM, DNORMAL, true, fmt, __VA_ARGS__)
#define PRINTC(type, fmt, ...) DEBUG(DVM, type, true, fmt, __VA_ARGS__)

#endif