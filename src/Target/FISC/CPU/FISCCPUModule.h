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

public:
	uint64_t readRegister(unsigned registerIndex);
	enum FISC_RETTYPE writeRegister(unsigned registerIndex, 
	                                uint64_t data,
									bool setFlags, 
									uint64_t operand1, uint64_t operand2, 
									char operation);

	enum FISC_RETTYPE branch(uint32_t new_addr, bool isPCRel);

private:
	bool detectOverflow(uint64_t operand1, uint64_t operand2, char operation);
	bool detectCarry(uint64_t operand1, uint64_t operand2, char operation);
	Instruction * decode(uint32_t instruction);
	std::string disassembleConstant(unsigned val);
	std::string disassembleRegister(unsigned registerIndex);
	std::string CPUModule::disassembleBCC(unsigned cc);
	std::string disassemble(Instruction * instruction);
	std::string getCurrentCPUModeStr();

public:
	CPUModule();
	enum PassRetcode init();
	enum PassRetcode finit();
	enum PassRetcode run();
	enum PassRetcode watchdog();
};

}

#endif