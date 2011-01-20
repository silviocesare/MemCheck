#ifndef _MemCheck_h
#define _MemCheck_h

#include "FunctionInterceptor.h"
#include "ShadowMemory.h"
#include "Addr32.h"
#include "Machine.h"
#include "SymbolTable.h"

class MemCheck {
public:
	MemCheck(Machine *machine) :
		m_Machine(machine), m_MemoryAccessDepth(0),
		m_MemoryAccessDisable(false) {}
	virtual ~MemCheck() {}

	virtual int DoAddress(void *cpuState) = 0;
	void MemoryAccess(void *cpuState, Addr32 address, unsigned int length);
	void MemoryAccessEnable();
	void MemoryAccessDisable();
	virtual void DumpTrace(void *cpuState) = 0;
	virtual void DumpTraceAtFunction(void *cpuState) = 0;

	Addr32 m_MemoryAccessStart;
	Addr32 m_MemoryAccessStop;
	ShadowMemory m_ShadowMemory;
	FunctionInterceptor *m_FunctionInterceptor;
	Machine *m_Machine;
	SymbolTable m_SymbolTable;
	unsigned int m_MemoryAccessDepth;
	bool m_MemoryAccessDisable;
};

#endif
