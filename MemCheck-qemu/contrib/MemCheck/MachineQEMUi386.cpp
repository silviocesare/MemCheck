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
	CPUX86State *cpuState = (CPUX86State *)arg;
	return cpuState->regs[R_EAX];
}

Addr32
MachineQEMU::GetReturnAddress(void *arg)
{
	CPUX86State *cpuState = (CPUX86State *)arg;
	Addr32 returnAddress;

	cpu_memory_rw_debug(cpuState, cpuState->regs[R_ESP], (uint8_t *)&returnAddress, sizeof(returnAddress), 0);
	return returnAddress;
}

Word32
MachineQEMU::GetArgumentN(void *arg, unsigned int argN)
{
	CPUX86State *cpuState = (CPUX86State *)arg;

	switch (argN) {
	case 1:
		return cpuState->regs[R_EAX];

	case 2:
		return cpuState->regs[R_EDX];

	case 3:
		return cpuState->regs[R_ECX];

	default:
		Word32 r;

		// assume we at the beginning of the function before
		// the frame is setup
		cpu_memory_rw_debug(cpuState,
			cpuState->regs[R_ESP] + 4 + 4*(argN - 3 - 1),
			(uint8_t *)&r, sizeof(r), 0);
		return r;
	}
}

Word32
MachineQEMU::GetArgument1(void *arg)
{
	CPUX86State *cpuState = (CPUX86State *)arg;
	return cpuState->regs[R_EAX];
}

Word32
MachineQEMU::GetArgument2(void *arg)
{
	CPUX86State *cpuState = (CPUX86State *)arg;
	return cpuState->regs[R_EDX];
}

Addr32
MachineQEMU::GetPC(void *arg)
{
	CPUX86State *cpuState = (CPUX86State *)arg;

	return cpuState->eip;
}

int
MachineQEMU::ReadMemory(void *arg, Addr32 address, unsigned char *buf, unsigned int length)
{
	CPUX86State *cpuState = (CPUX86State *)arg;

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
	CPUX86State *cpuState = (CPUX86State *)arg;
	Addr32 returnAddress;


	printf("TRACE\n");
	PrintPC(0, cpuState->eip, symbolTable);
	cpu_memory_rw_debug(cpuState, cpuState->regs[R_ESP],
		(uint8_t *)&returnAddress, sizeof(returnAddress), 0);
	PrintPC(1, returnAddress, symbolTable);
	RecurseStackTrace(cpuState, symbolTable, cpuState->regs[R_EBP], 2);
}

void
MachineQEMU::StackTrace(void *arg, SymbolTable *symbolTable)
{
	CPUX86State *cpuState = (CPUX86State *)arg;

	printf("TRACE\n");
	PrintPC(0, cpuState->eip, symbolTable);
	RecurseStackTrace(cpuState, symbolTable, cpuState->regs[R_EBP], 1);
}

void
MachineQEMU::RecurseStackTrace(void *arg, SymbolTable *symbolTable, Addr32 framePointer, unsigned int depth)
{
	CPUX86State *cpuState = (CPUX86State *)arg;

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
}
