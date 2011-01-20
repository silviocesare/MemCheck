#ifndef _MemCheckLinuxKernelQEMU_h
#define _MemCheckLinuxKernelQEMU_h

#include "Addr32.h"

#ifdef __cplusplus
extern "C" {
#endif

void MemCheckLinuxKernelQEMU_Init(const char *systemMap);
void MemCheckLinuxKernelQEMU_DoAddress(void *cpuState);
void MemCheckLinuxKernelQEMU_MemoryAccess_b(void *cpuState, Addr32 address);
void MemCheckLinuxKernelQEMU_MemoryAccess_w(void *cpuState, Addr32 address);
void MemCheckLinuxKernelQEMU_MemoryAccess_l(void *cpuState, Addr32 address);
void MemCheckLinuxKernelQEMU_MemoryAccess_q(void *cpuState, Addr32 address);

#ifdef __cplusplus
};
#endif

#endif
