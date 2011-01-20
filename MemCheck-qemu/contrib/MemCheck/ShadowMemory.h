#ifndef _ShadowMemory_h
#define _ShadowMemory_h

#include "Addr32.h"
#include "Machine.h"
#include <map>

class MemoryAllocContents {
public:
	MemoryAllocContents(unsigned int size, int type) : m_Type(type) {}
	int m_Type;
};

class MemoryAlloc {
public:
	Addr32 m_Address;
	unsigned int m_Size;

	MemoryAlloc(Addr32 address, unsigned int size) :
		m_Address(address),
		m_Size(size) {}
	MemoryAlloc() : m_Address(0), m_Size(0) {}

	int operator()(const MemoryAlloc &x, const MemoryAlloc &y) const
	{
		if (x.m_Size)
			return (x.m_Address + x.m_Size) <= y.m_Address;
		else
			return (x.m_Address + x.m_Size) < y.m_Address;
	}
};

class ShadowMemory {
public:
	std::map<MemoryAlloc, MemoryAllocContents *, MemoryAlloc> m_MemoryMap;

	int NewMemoryAlloc(int type, Addr32 allocPtr, unsigned int size);
	int FreeMemory(int type, Addr32 freePtr);
	int FreeSizeMemory(int type, Addr32 freePtr, unsigned int size);
	int FreeAnySizeMemory(int type, Addr32 freePtr, unsigned int size);
};

#endif
