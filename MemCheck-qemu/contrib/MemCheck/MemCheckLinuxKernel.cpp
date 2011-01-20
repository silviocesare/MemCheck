#include "MemCheckLinuxKernel.h"
#include <sys/time.h>

MemCheckLinuxKernel::MemCheckLinuxKernel(Machine *machine) :
	MemCheck(machine),
	m_Initialized(false)
{
}

MemCheckLinuxKernel::~MemCheckLinuxKernel()
{
	if (m_Initialized) {
		delete m_AllocFunction;
		delete m_FreeFunction;
		delete m_FreeSizeFunction;
	}
}

void
MemCheckLinuxKernel::Init(FunctionInterceptor *interceptor,
	const std::string systemMap)
{
	m_FunctionInterceptor = interceptor;
	interceptor->InitArg(this);

	m_SymbolTable.Load(systemMap);

	m_AllocFunction		= new MemCheck_Alloc(&m_SymbolTable); 
	m_FreeFunction		= new MemCheck_Free(&m_SymbolTable); 
	m_FreeSizeFunction	= new MemCheck_FreeSize(&m_SymbolTable); 
	m_FreeAnySizeFunction	= new MemCheck_FreeAnySize(&m_SymbolTable); 

	m_FunctionInterceptor->NewFunction(m_AllocFunction);
	m_FunctionInterceptor->NewFunction(m_FreeFunction);
	m_FunctionInterceptor->NewFunction(m_FreeSizeFunction);
	m_FunctionInterceptor->NewFunction(m_FreeAnySizeFunction);

	m_MemoryAccessStop = 0xdfff0000UL;
	m_SymbolTable.Lookup(std::string("__bss_stop"), m_MemoryAccessStart);

//	MemoryAccessDisable();
	MemoryAccessEnable();
//	m_FunctionInterceptor->NewFunction(new LinuxKernelEnableMemoryAccess("mem_init", &m_SymbolTable, 0, true));
//	m_FunctionInterceptor->NewFunction(new LinuxKernelEnableMemoryAccess("page_alloc_init", &m_SymbolTable, 0, true));

	m_FunctionInterceptor->NewFunction(new LinuxKernelHeapFunction("__free_slab", &m_SymbolTable)); // slub debugging

	m_FunctionInterceptor->NewFunction(new LinuxKernelHeapFunction("__alloc_bootmem_core", &m_SymbolTable));
	m_FunctionInterceptor->NewFunction(new LinuxKernelHeapFunction("free_bootmem_core", &m_SymbolTable));
	m_FunctionInterceptor->NewFunction(new LinuxKernelHeapFunction("init_bootmem_core", &m_SymbolTable));
	m_FunctionInterceptor->NewFunction(new LinuxKernelHeapFunction("reserve_bootmem", &m_SymbolTable));

	m_FunctionInterceptor->NewFunction(new LinuxKernelHeapFunction("free_init_pages", &m_SymbolTable)); // fixme. move Free to after freerd in kernel
	m_FunctionInterceptor->NewFunction(new LinuxKernelHeapFunction("__kmalloc", &m_SymbolTable));
	m_FunctionInterceptor->NewFunction(new LinuxKernelHeapFunction("mykfree", &m_SymbolTable));
	m_FunctionInterceptor->NewFunction(new LinuxKernelHeapFunction("mykmem_cache_free", &m_SymbolTable));
	m_FunctionInterceptor->NewFunction(new LinuxKernelHeapFunction("my__kmalloc_track_caller", &m_SymbolTable));

	m_FunctionInterceptor->NewFunction(new LinuxKernelHeapFunction("__alloc_pages", &m_SymbolTable));
	m_FunctionInterceptor->NewFunction(new LinuxKernelHeapFunction("mykmem_cache_alloc", &m_SymbolTable));
	m_FunctionInterceptor->NewFunction(new LinuxKernelHeapFunction("free_bootmem", &m_SymbolTable));
	m_Initialized = true;
}

LinuxKernelFunction::LinuxKernelFunction(const char *symbolName,
	SymbolTable *symbolTable, unsigned int numberOfArguments, bool debug) :
	Function(symbolName, numberOfArguments, debug)
{
	symbolTable->Lookup(m_SymbolName, m_Address);
}

int
MemCheckLinuxKernel::DoAddress(void *cpuState)
{
	Addr32 pc;

	pc = m_Machine->GetPC(cpuState);
	if (pc >= 0xc0000000UL) {
		return m_FunctionInterceptor->DoAddress(pc, cpuState);
	}
	return 0;
}

void
MemCheckLinuxKernel::PrintKernelComm(void *cpuState)
{
	Addr32 currentPtr, current;
	char comm[16];
	int ret;

	ret = m_SymbolTable.Lookup(
		std::string("per_cpu__current_task"), currentPtr);
	if (ret)
		return;
	m_Machine->ReadMemory(cpuState,
		currentPtr, (unsigned char *)&current, sizeof(current));
#define COMM_OFFSET 761
	m_Machine->ReadMemory(cpuState,
		current + COMM_OFFSET, (unsigned char *)comm, sizeof(comm));
	printf("comm: %s\n", comm);
}

void
MemCheckLinuxKernel::DumpTraceAtFunction(void *cpuState)
{
	printf("time: %u\n", (unsigned int)time(NULL));
	PrintKernelComm(cpuState);
	m_Machine->StackTraceAtFunction(cpuState, &m_SymbolTable);
}

void
MemCheckLinuxKernel::DumpTrace(void *cpuState)
{
	PrintKernelComm(cpuState);
	m_Machine->StackTrace(cpuState, &m_SymbolTable);
}

void
LinuxKernelHeapFunction::Enter(void *arg, void *cpuState)
{
	MemCheckLinuxKernel *memCheck = (MemCheckLinuxKernel *)arg;

	memCheck->MemoryAccessDisable();
}

void
LinuxKernelHeapFunction::Leave(void *arg, void *cpuState)
{
	MemCheckLinuxKernel *memCheck = (MemCheckLinuxKernel *)arg;

	memCheck->MemoryAccessEnable();
}

void
MemCheck_Alloc::Enter(void *arg, void *cpuState)
{
	MemCheckLinuxKernel *memCheck = (MemCheckLinuxKernel *)arg;
	Addr32 allocPtr;
	unsigned int size;
	int ret;

	allocPtr = memCheck->m_Machine->GetArgument1(cpuState);
	size = memCheck->m_Machine->GetArgument2(cpuState);
	ret = memCheck->m_ShadowMemory.NewMemoryAlloc(0, allocPtr, size);
	if (ret)
		memCheck->DumpTraceAtFunction(cpuState);
}

void
MemCheck_Free::Enter(void *arg, void *cpuState)
{
	MemCheckLinuxKernel *memCheck = (MemCheckLinuxKernel *)arg;
	Addr32 freePtr;
	int ret;

	freePtr = memCheck->m_Machine->GetArgument1(cpuState);
	ret = memCheck->m_ShadowMemory.FreeMemory(0, freePtr);
	if (ret)
		memCheck->DumpTraceAtFunction(cpuState);
}

void
MemCheck_FreeSize::Enter(void *arg, void *cpuState)
{
	MemCheckLinuxKernel *memCheck = (MemCheckLinuxKernel *)arg;
	Addr32 freePtr;
	unsigned int size;
	int ret;

	freePtr = memCheck->m_Machine->GetArgument1(cpuState);
	size = memCheck->m_Machine->GetArgument2(cpuState);
	ret = memCheck->m_ShadowMemory.FreeSizeMemory(0, freePtr, size);
	if (ret)
		memCheck->DumpTraceAtFunction(cpuState);
}

void
MemCheck_FreeAnySize::Enter(void *arg, void *cpuState)
{
	MemCheckLinuxKernel *memCheck = (MemCheckLinuxKernel *)arg;
	Addr32 freePtr;
	unsigned int size;
	int ret;

	freePtr = memCheck->m_Machine->GetArgument1(cpuState);
	size = memCheck->m_Machine->GetArgument2(cpuState);
	ret = memCheck->m_ShadowMemory.FreeAnySizeMemory(0, freePtr, size);
	if (ret)
		memCheck->DumpTraceAtFunction(cpuState);
}
