#ifndef _SymbolTable_h
#define _SymbolTable_h

#include "Addr32.h"

#include <map>
#include <set>
#include <string>

class Symbol {
public:
        Addr32 m_Address;
        unsigned int m_Size;
	std::string m_Name;

        Symbol(const std::string name, Addr32 address, unsigned int size) :
                m_Address(address),
                m_Size(size),
		m_Name(name) {}
	Symbol(Addr32 address) : m_Address(address), m_Size(0) {}
        Symbol() : m_Address(0), m_Size(0) {}

        int operator()(const Symbol &x, const Symbol &y) const
        {
                if (x.m_Size)
                        return (x.m_Address + x.m_Size) <= y.m_Address;
                else
                        return (x.m_Address + x.m_Size) < y.m_Address;
        }
};

class SymbolTable {
public:
	SymbolTable(const std::string systemMap);
	SymbolTable();

	int Load(const std::string systemMap);
	int Lookup(Addr32 address, Symbol &symbol);
	int Lookup(const std::string name, Addr32 &address);

protected:
	std::map<std::string, Addr32> m_NameToAddressMap;
	std::set<Symbol, Symbol> m_SymbolMap;
};

#endif
