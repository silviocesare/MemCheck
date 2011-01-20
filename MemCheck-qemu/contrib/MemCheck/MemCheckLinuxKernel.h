#ifndef _MemCheckLinuxKernel_h
#define _MemCheckLinuxKernel_h

#include "MemCheck.h"
#include "SymbolTable.h"

class MemCheckLinuxKernel : public MemCheck {
public:
	MemCheckLinuxKernel(Machine *machine);
	~MemCheckLinuxKernel();

	void Init(FunctionInterceptor *interceptor, const std::string systemMap);
	int DoAddress(void *cpuState);
	void DumpTraceAtFunction(void *cpuState);
	void DumpTrace(void *cpuState);

	void PrintKernelComm(void *cpuState);

	bool m_Initialized;
	Function *m_AllocFunction;
	Function *m_FreeFunction;
	Function *m_FreeSizeFunction;
	Function *m_FreeAnySizeFunction;
};

class LinuxKernelFunction : public Function {
public:
	LinuxKernelFunction(const char *symbolName, SymbolTable *symbolTable,
		unsigned int numberOfArguments = 0, bool debug = false);
};

class LinuxKernelEnableMemoryAccess : public LinuxKernelFunction {
public:
	LinuxKernelEnableMemoryAccess(const char *symbolName,
		SymbolTable *symbolTable, unsigned int numberOfArguments = 0,
		bool debug = false) :
		LinuxKernelFunction(symbolName, symbolTable, numberOfArguments,
			debug) {}

	void Leave(void *arg, void *cpuState)
	{
		MemCheckLinuxKernel *memCheck = (MemCheckLinuxKernel *)arg;
		memCheck->MemoryAccessEnable();
	}
};

class LinuxKernelHeapFunction : public LinuxKernelFunction {
public:
	LinuxKernelHeapFunction(const char *symbolName,
		SymbolTable *symbolTable, unsigned int numberOfArguments = 0,
		bool debug = false) :
		LinuxKernelFunction(symbolName, symbolTable, numberOfArguments,
			debug) {}

	void Enter(void *memCheck, void *cpuState);
	void Leave(void *memCheck, void *cpuState);
};

class MemCheck_Alloc : public LinuxKernelFunction {
public:
	MemCheck_Alloc(SymbolTable *symbolTable, bool debug = false) :
		LinuxKernelFunction("MemCheck_Alloc", symbolTable, 2, debug) {}

	void Enter(void *memCheck, void *cpuState);
};

class MemCheck_Free : public LinuxKernelFunction {
public:
	MemCheck_Free(SymbolTable *symbolTable, bool debug = false) :
		LinuxKernelFunction("MemCheck_Free", symbolTable, 1, debug) {}

	void Enter(void *memCheck, void *cpuState);
};

class MemCheck_FreeSize: public LinuxKernelFunction {
public:
	MemCheck_FreeSize(SymbolTable *symbolTable, bool debug = false) :
		LinuxKernelFunction("MemCheck_FreeSize", symbolTable, 2, debug) {}

	void Enter(void *memCheck, void *cpuState);
};

class MemCheck_FreeAnySize: public LinuxKernelFunction {
public:
	MemCheck_FreeAnySize(SymbolTable *symbolTable, bool debug = false) :
		LinuxKernelFunction("MemCheck_FreeAnySize", symbolTable, 2,
			debug) {}

	void Enter(void *memCheck, void *cpuState);
};

#endif
