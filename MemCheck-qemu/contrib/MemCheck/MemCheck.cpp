#include "MemCheck.h"

void
MemCheck::MemoryAccessEnable()
{
	if (m_MemoryAccessDepth) {
		m_MemoryAccessDepth--;
	} else {
		m_MemoryAccessDisable = false;
	}
}

void
MemCheck::MemoryAccessDisable()
{
	if (m_MemoryAccessDisable) {
		m_MemoryAccessDepth++;
	} else {
		m_MemoryAccessDisable = true;
	}
}

void
MemCheck::MemoryAccess(void *cpuState, Addr32 address, unsigned int length)
{
	std::map<MemoryAlloc, MemoryAllocContents *>::iterator pos;

	if (m_MemoryAccessDisable)
		return;

	if (address < m_MemoryAccessStart || address >= m_MemoryAccessStop)
		return;

	pos = m_ShadowMemory.m_MemoryMap.find(MemoryAlloc(address, length));
	if (pos == m_ShadowMemory.m_MemoryMap.end()) {
printf("OUT OF BOUND access %x:%i\n", address, length);
		DumpTrace(cpuState);
		fflush(stdout);
	}
}
