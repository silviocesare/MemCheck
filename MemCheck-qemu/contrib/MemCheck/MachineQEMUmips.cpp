#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <signal.h>
#include <assert.h>

#include "cpu.h"

#include "MachineQEMU.h"

#define MIPS_GPR_V0	2
#define MIPS_GPR_V1	3
#define MIPS_GPR_A0	4
#define MIPS_GPR_A1	5
#define MIPS_GPR_A2	6
#define MIPS_GPR_A3	7
#define MIPS_GPR_RA	31
#define MIPS_GPR_SP	29

Word32
MachineQEMU::GetReturnValue(void *arg)
{
	CPUMIPSState *cpuState = (CPUMIPSState *)arg;
	return cpuState->gpr[MIPS_GPR_V0][cpuState->current_tc];
}

Addr32
MachineQEMU::GetReturnAddress(void *arg)
{
	CPUMIPSState *cpuState = (CPUMIPSState *)arg;

	return cpuState->gpr[MIPS_GPR_RA][cpuState->current_tc];
}

Word32
MachineQEMU::GetArgumentN(void *arg, unsigned int argN)
{
	CPUMIPSState *cpuState = (CPUMIPSState *)arg;

	switch (argN) {
	case 1:
		return cpuState->gpr[MIPS_GPR_A0][cpuState->current_tc];

	case 2:
		return cpuState->gpr[MIPS_GPR_A1][cpuState->current_tc];

	case 3:
		return cpuState->gpr[MIPS_GPR_A2][cpuState->current_tc];

	case 4:
		return cpuState->gpr[MIPS_GPR_A3][cpuState->current_tc];

	default:
		return -1; // XXX
	}
}

Word32
MachineQEMU::GetArgument1(void *arg)
{
	CPUMIPSState *cpuState = (CPUMIPSState *)arg;
	return cpuState->gpr[MIPS_GPR_A0][cpuState->current_tc];
}

Word32
MachineQEMU::GetArgument2(void *arg)
{
	CPUMIPSState *cpuState = (CPUMIPSState *)arg;
	return cpuState->gpr[MIPS_GPR_A1][cpuState->current_tc];
}

Addr32
MachineQEMU::GetPC(void *arg)
{
	CPUMIPSState *cpuState = (CPUMIPSState *)arg;
	return cpuState->PC[cpuState->current_tc];
}

int
MachineQEMU::ReadMemory(void *arg, Addr32 address, unsigned char *buf, unsigned int length)
{
	CPUMIPSState *cpuState = (CPUMIPSState *)arg;

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
	CPUMIPSState *cpuState = (CPUMIPSState *)arg;
	Addr32 returnAddress;


	printf("TRACE\n");
	PrintPC(0, cpuState->PC[cpuState->current_tc], symbolTable);
	PrintPC(1, cpuState->gpr[MIPS_GPR_RA][cpuState->current_tc],
		symbolTable);
}

void
MachineQEMU::StackTrace(void *arg, SymbolTable *symbolTable)
{
	CPUMIPSState *cpuState = (CPUMIPSState *)arg;

	printf("TRACE\n");
	PrintPC(0, cpuState->PC[cpuState->current_tc], symbolTable);
}

void
MachineQEMU::RecurseStackTrace(	void *arg, SymbolTable *symbolTable,
				Addr32 framePointer, unsigned int depth)
{
}
