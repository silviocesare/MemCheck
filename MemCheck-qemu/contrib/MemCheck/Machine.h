#ifndef _Machine_h
#define _Machine_h

#include "Addr32.h"
#include "SymbolTable.h"

class Machine {
public:
	virtual ~Machine() {}

	virtual Word32 GetReturnValue(void *cpuState) = 0;
	virtual Addr32 GetReturnAddress(void *cpuState) = 0;
	virtual Word32 GetArgumentN(void *cpuState, unsigned int arg) = 0;
	virtual Word32 GetArgument1(void *cpuState) = 0;
	virtual Word32 GetArgument2(void *cpuState) = 0;
	virtual Addr32 GetPC(void *cpuState) = 0;
	virtual int ReadMemory(void *cpuState, Addr32 address, unsigned char *buf, unsigned int length) = 0;
	virtual void StackTrace(void *cpuState, SymbolTable *symbolTable) = 0;
	virtual void StackTraceAtFunction(void *cpuState, SymbolTable *symbolTable) = 0;
};

#endif
