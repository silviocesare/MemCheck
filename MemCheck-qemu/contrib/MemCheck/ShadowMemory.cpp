#include "ShadowMemory.h"

int
ShadowMemory::NewMemoryAlloc(int type, Addr32 allocPtr, unsigned int size)
{
	std::pair<std::map<MemoryAlloc, MemoryAllocContents *,
		MemoryAlloc>::iterator, bool> p;

	p = m_MemoryMap.insert(std::pair<MemoryAlloc,
		MemoryAllocContents *>(MemoryAlloc(allocPtr, size),
		new MemoryAllocContents(size, type)));
	if (p.second == false) {
printf("ERROR (Allocation already exists) newAllocPtr=%x:%x oldAllocPtr=%x:%x\n", allocPtr, size, p.first->first.m_Address, p.first->first.m_Size);
		return 1;
	}
	return 0;
}

int
ShadowMemory::FreeMemory(int type, Addr32 freePtr)
{
	std::map<MemoryAlloc, MemoryAllocContents *>::iterator pos;

	if (freePtr == 0) {
		return 0;
	}
	pos = m_MemoryMap.find(MemoryAlloc(freePtr, 0));
	if (pos == m_MemoryMap.end()) {
		printf("ERROR (no allocated memory) freePtr=%x\n", freePtr);
		return 1;
	}
	if (pos->first.m_Address == freePtr) {
		delete pos->second;
		m_MemoryMap.erase(pos);
		return 0;
	} else {
printf("ERROR (free middle of allocation) oldAllocPtr=%x:%x freePtr=%x\n",
	pos->first.m_Address, pos->first.m_Size, freePtr);
		return 1;
	}
}

int
ShadowMemory::FreeSizeMemory(int type, Addr32 freePtr, unsigned int size)
{
	std::map<MemoryAlloc, MemoryAllocContents *>::iterator pos;

	if (freePtr == 0) {
		return 0;
	}
	pos = m_MemoryMap.find(MemoryAlloc(freePtr, 0));
	if (pos == m_MemoryMap.end()) {
		printf("ERROR (no allocated memory) freePtr=%x\n", freePtr);
		return 1;
	}
	if (pos->first.m_Address == freePtr) {
		if (pos->first.m_Size != size) {
			printf("ERROR (freeSize not equal to allocSize) freePtr=%x alloc_size=%x free_size=%x\n", freePtr, pos->first.m_Size, size);
			if (size < pos->first.m_Size) {
				unsigned int newSize = pos->first.m_Size - size;
				Addr32 newAddress = pos->first.m_Address + size;

				printf("Truncating allocation - maybe OK\n");
				delete pos->second;
				m_MemoryMap.erase(pos);
				m_MemoryMap.insert(std::pair<MemoryAlloc,
					MemoryAllocContents *>(MemoryAlloc(
						newAddress, newSize),
					new MemoryAllocContents(newSize, type)));
			}
			return 1;
		}
		delete pos->second;
		m_MemoryMap.erase(pos);
		return 0;
	} else {
printf("ERROR (free middle of allocation) oldAllocPtr=%x:%x freePtr=%x freeSize=%x\n",
	pos->first.m_Address, pos->first.m_Size, freePtr, size);
		return 1;
	}
}

int
ShadowMemory::FreeAnySizeMemory(int type, Addr32 freePtr, unsigned int size)
{
	std::map<MemoryAlloc, MemoryAllocContents *>::iterator pos;

	if (freePtr == 0) {
		return 0;
	}
	pos = m_MemoryMap.find(MemoryAlloc(freePtr, 0));
	if (pos == m_MemoryMap.end()) {
		printf("ERROR (no allocated memory) freePtr=%x\n", freePtr);
		return 1;
	}
	if (pos->first.m_Address == freePtr) {
		if (pos->first.m_Size != size) {
			if (size < pos->first.m_Size) {
				unsigned int newSize = pos->first.m_Size - size;
				Addr32 newAddress = pos->first.m_Address + size;

				delete pos->second;
				m_MemoryMap.erase(pos);
				m_MemoryMap.insert(std::pair<MemoryAlloc,
					MemoryAllocContents *>(MemoryAlloc(
						newAddress, newSize),
					new MemoryAllocContents(newSize, type)));
				return 0;
			}
			// falling through (allow free with large size)
		}
		delete pos->second;
		m_MemoryMap.erase(pos);
	} else {
		unsigned int oldSize = pos->first.m_Size;
		Addr32 oldAddress = pos->first.m_Address;

		delete pos->second;
		m_MemoryMap.erase(pos);

		// the chunk at the beginning
		m_MemoryMap.insert(std::pair<MemoryAlloc,
			MemoryAllocContents *>(MemoryAlloc(
				oldAddress, freePtr - oldAddress),
				new MemoryAllocContents(freePtr - oldAddress,
				type)));
		// the tail
		if ((freePtr + size) != (oldAddress + oldSize)) {
			m_MemoryMap.insert(std::pair<MemoryAlloc,
				MemoryAllocContents *>(MemoryAlloc(
		freePtr + size, oldAddress + oldSize - (freePtr + size)),
	new MemoryAllocContents(oldAddress + oldSize - (freePtr + size),
					type)));
		}
	}
	return 0;
}
