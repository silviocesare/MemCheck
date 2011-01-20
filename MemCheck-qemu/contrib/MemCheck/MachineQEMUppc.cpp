#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <signal.h>
#include <assert.h>

#include "cpu.h"

#include "MachineQEMU.h"

Word32
MachineQEMU::GetReturnValue(void *arg)
{
	CPUPPCState *cpuState = (CPUPPCState *)arg;
	return cpuState->gpr[3];
}

Addr32
MachineQEMU::GetReturnAddress(void *arg)
{
	CPUPPCState *cpuState = (CPUPPCState *)arg;
	return cpuState->lr;
}

Word32
MachineQEMU::GetArgumentN(void *arg, unsigned int argN)
{
	CPUPPCState *cpuState = (CPUPPCState *)arg;

	if (argN >= 1 && argN <= 8) {
		return cpuState->gpr[argN - 1 + 3];
	} else {
		return -1; // XXX
	}
}

Word32
MachineQEMU::GetArgument1(void *arg)
{
	CPUPPCState *cpuState = (CPUPPCState *)arg;
	return cpuState->gpr[3];
}

Word32
MachineQEMU::GetArgument2(void *arg)
{
	CPUPPCState *cpuState = (CPUPPCState *)arg;
	return cpuState->gpr[4];
}

Addr32
MachineQEMU::GetPC(void *arg)
{
	CPUPPCState *cpuState = (CPUPPCState *)arg;
	return cpuState->nip;
}

int
MachineQEMU::ReadMemory(void *arg, Addr32 address, unsigned char *buf, unsigned int length)
{
	CPUPPCState *cpuState = (CPUPPCState *)arg;

	cpu_memory_rw_debug(cpuState, address, (uint8_t *)buf, length, 0);
	return 0;
}

void
MachineQEMU::PrintPC(unsigned int depth, Addr32 pc, SymbolTable *symbolTable)
{
	Symbol symbol;

	if (symbolTable->Lookup(pc, symbol) == 0) {
		printf("%2u: %s+%u (%8x)\n", depth,
			symbol.m_Name.c_str(),
			pc - symbol.m_Address,
			pc);
	} else {
		printf("%2u: %8x\n", depth, pc);
	}
}

void
MachineQEMU::StackTraceAtFunction(void *arg, SymbolTable *symbolTable)
{
	CPUPPCState *cpuState = (CPUPPCState *)arg;
	Addr32 returnAddress;


	printf("TRACE\n");
	PrintPC(0, cpuState->nip, symbolTable);
	PrintPC(1, cpuState->lr, symbolTable);
//	RecurseStackTrace(cpuState, symbolTable, cpuState->regs[R_EBP], 2);
}

void
MachineQEMU::StackTrace(void *arg, SymbolTable *symbolTable)
{
	CPUPPCState *cpuState = (CPUPPCState *)arg;

	printf("TRACE\n");
	PrintPC(0, cpuState->nip, symbolTable);
//	RecurseStackTrace(cpuState, symbolTable, cpuState->regs[R_EBP], 1);
}

void
MachineQEMU::RecurseStackTrace(void *arg, SymbolTable *symbolTable, Addr32 framePointer, unsigned int depth)
{
	CPUPPCState *cpuState = (CPUPPCState *)arg;
/*
	for (; depth < 20 && framePointer != 0; depth++) {
		Addr32 returnAddress;

		cpu_memory_rw_debug(cpuState, framePointer + 4,
			(uint8_t *)&returnAddress, sizeof(returnAddress), 0);
		if (returnAddress == 0)
			break;
		PrintPC(depth, returnAddress, symbolTable);
		cpu_memory_rw_debug(cpuState, framePointer + 0,
			(uint8_t *)&framePointer, sizeof(framePointer), 0);
	}
	fflush(stdout);
*/
}
