#include "SymbolTable.h"
#include <fstream>
#include <cstdio>

SymbolTable::SymbolTable(const std::string systemMap)
{
	Load(systemMap);
}

SymbolTable::SymbolTable()
{
}

int
SymbolTable::Load(const std::string systemMap)
{
	std::ifstream in;
	std::map<Addr32, std::string> AddressToNameMap;

	in.open(systemMap.c_str());
	if (!in.is_open()) {
		printf("System.map must be in the current directory\n");
		exit(1);
	}
	while (!in.eof()) {
		char s[500];
		char type;
		Addr32 addr;
		char name[100];

		in.getline(s, sizeof(s));
		sscanf(s, "%8x %c %s\n", &addr, &type, name);
		if (type != 'A') {
			AddressToNameMap[addr] = std::string(name);
			m_NameToAddressMap[std::string(name)] = addr;
		}
	}
	std::map<Addr32, std::string>::const_iterator pos, next;

	pos = AddressToNameMap.begin();
	next = pos;
	next++;
	while (next != AddressToNameMap.end()) {
		m_SymbolMap.insert(Symbol(pos->second, pos->first,
			next->first - pos->first));
		pos = next;
		next++;
	}
	return 0;
}

int
SymbolTable::Lookup(Addr32 address, Symbol &symbol)
{
	std::set<Symbol>::iterator pos;

	pos = m_SymbolMap.find(Symbol(address));
	if (pos != m_SymbolMap.end()) {
		symbol = *pos;
		return 0;
	}
	return 1;
}

int
SymbolTable::Lookup(const std::string name, Addr32 &address)
{
	std::map<std::string, Addr32>::iterator pos;

	pos = m_NameToAddressMap.find(name);
	if (pos != m_NameToAddressMap.end()) {
		address = pos->second;
		return 0;
	}
	return 1;
}
