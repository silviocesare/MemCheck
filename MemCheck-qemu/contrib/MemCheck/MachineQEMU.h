#ifndef _MachineQEMU_h
#define _MachineQEMU_h

#include "Machine.h"

class MachineQEMU : public Machine {
public:
	Word32 GetReturnValue(void *cpuState);
	Addr32 GetReturnAddress(void *cpuState);
	Word32 GetArgumentN(void *cpuState, unsigned int arg);
	Word32 GetArgument1(void *cpuState);
	Word32 GetArgument2(void *cpuState);
	Addr32 GetPC(void *cpuState);
	int ReadMemory(void *cpuState, Addr32 address, unsigned char *buf, unsigned int length);
	void StackTrace(void *cpuState, SymbolTable *symbolTable);
	void StackTraceAtFunction(void *cpuState, SymbolTable *symbolTable);

protected:
	void RecurseStackTrace(void *arg, SymbolTable *symbolTable,
		Addr32 framePointer, unsigned int depth);
	void PrintPC(unsigned int depth, Addr32 pc, SymbolTable *symbolTable);
};

#endif
