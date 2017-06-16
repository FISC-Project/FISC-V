#ifndef FISCCPUMODULE_H_
#define FISCCPUMODULE_H_

#include <fvm/Pass.h>

namespace FISC {

class MemoryModule;
class CPUConfigurator;
class Instruction;

class CPUModule : public RunPass {
private:
	MemoryModule * memory;   /* The main memory handle */
	CPUConfigurator * cconf; /* The configuration of the CPU. Contains the list of instructions */
	bool isBranching;
	bool isInsideException;
	bool isInsideInterrupt;
	bool generatedException;
	bool generatedExternalException;
	bool generatedInterrupt;
	bool generatedExternalInterrupt;
	unsigned oldCPUMode;

public:
	uint64_t readRegister(unsigned registerIndex);
	enum FISC_RETTYPE writeRegister(unsigned registerIndex, 
	                                uint64_t data,
									bool setFlags, 
									uint64_t operand1, uint64_t operand2, 
									char operation);

	enum FISC_RETTYPE branch(uint32_t new_addr, bool isPCRel);

	uint64_t mmu_read(uint32_t address,
	                  enum FISC_DATATYPE dataType, bool forceAlign,
					  bool debug);
	enum FISC_RETTYPE mmu_write(uint64_t data, uint32_t address, 
	                            enum FISC_DATATYPE dataType, bool forceAlign,
								bool debug);

	enum FISC_RETTYPE triggerSoftInterrupt(unsigned intCode);
	enum FISC_RETTYPE triggerHardInterrupt(unsigned intCode);
	enum FISC_RETTYPE triggerSoftException(unsigned excCode);
	enum FISC_RETTYPE triggerHardException(unsigned excCode);
	enum FISC_RETTYPE intExcReturn(uint32_t retAddr);

private:
	enum FISC_RETTYPE enterISR(uint32_t interruptVectorPtr, unsigned isrID);
	enum FISC_RETTYPE enterEXC(uint32_t exceptionVectorPtr, unsigned excID);
	enum FISC_RETTYPE switchContext(enum FISC_CPU_MODE newMode);
	enum FISC_RETTYPE restoreContext();
	bool areInterruptsEnabled();
	bool areExceptionsEnabled();
	enum FISC_RETTYPE disableInterrupts();
	enum FISC_RETTYPE enableInterrupts();
	enum FISC_RETTYPE disableExceptions();
	enum FISC_RETTYPE enableExceptions();
	enum FISC_RETTYPE interruptCPU(unsigned code, bool isException, bool isInternal);
	bool detectOverflow(uint64_t operand1, uint64_t operand2, char operation);
	bool detectCarry(uint64_t operand1, uint64_t operand2, char operation);
	Instruction * decode(uint32_t instruction);
	std::string disassembleConstant(unsigned val);
	std::string disassembleRegister(unsigned registerIndex);
	std::string CPUModule::disassembleBCC(unsigned cc);
	std::string disassemble(Instruction * instruction);
	std::string getCurrentCPUModeStr();
	enum FISC_RETTYPE enterUndefMode();
	enum FISC_RETTYPE mmu_translate(uint32_t & retVal, uint32_t virtualAddr);

	void dumpWarning(std::string problematicArg, std::string fullArg);
	void dumpInternals();

public:
	CPUModule();
	enum PassRetcode init();
	enum PassRetcode finit();
	enum PassRetcode run();
	enum PassRetcode watchdog();
};

}

#endif