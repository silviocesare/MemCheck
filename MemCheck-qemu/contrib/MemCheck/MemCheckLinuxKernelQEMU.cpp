#include "MemCheckLinuxKernelQEMU.h"
#include "MemCheckLinuxKernel.h"
#include "MachineQEMU.h"
#include <string>

static MachineQEMU g_MachineQEMU;
static FunctionInterceptor g_FunctionInterceptor(&g_MachineQEMU);
static MemCheckLinuxKernel g_MemCheck(&g_MachineQEMU);

extern "C" {

void
MemCheckLinuxKernelQEMU_Init(const char *systemMap)
{
	g_MemCheck.Init(&g_FunctionInterceptor, std::string(systemMap));
}

void
MemCheckLinuxKernelQEMU_DoAddress(void *cpuState)
{
	g_MemCheck.DoAddress(cpuState);
/*
	if (g_MemCheck.DoAddress(cpuState))
		g_MemCheck.m_Machine->StackTraceAtFunction(cpuState,
			&g_MemCheck.m_SymbolTable);
*/
}

void
MemCheckLinuxKernelQEMU_MemoryAccess_b(void *cpuState, Addr32 address)
{
	g_MemCheck.MemoryAccess(cpuState, address, 1);
}

void
MemCheckLinuxKernelQEMU_MemoryAccess_w(void *cpuState, Addr32 address)
{
	g_MemCheck.MemoryAccess(cpuState, address, 2);
}

void
MemCheckLinuxKernelQEMU_MemoryAccess_l(void *cpuState, Addr32 address)
{
	g_MemCheck.MemoryAccess(cpuState, address, 4);
}

void
MemCheckLinuxKernelQEMU_MemoryAccess_q(void *cpuState, Addr32 address)
{
	g_MemCheck.MemoryAccess(cpuState, address, 8);
}

}; // extern "C"
