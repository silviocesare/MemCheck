#ifndef _FunctionInterceptor_h
#define _FunctionInterceptor_h

#include <list>
#include <map>
#include <string>

#include "Addr32.h"
#include "Machine.h"

class Function {
public:
	Function(const char *symbolName, unsigned int numberOfArguments = 0,
		bool debug = false) :
		m_SymbolName(symbolName),
		m_Active(false),
		m_NumberOfArguments(numberOfArguments),
		m_Debug(debug)
	{}
	virtual ~Function() {}

	virtual void Enter(void *arg, void *cpuState) {}
	virtual void Leave(void *arg, void *cpuState) {}
	Addr32 m_Address;
	std::string m_SymbolName;
	bool m_Active;
	unsigned int m_NumberOfArguments;
	bool m_Debug;
};

class AddressOfInterest {
public:
	static AddressOfInterest *NewFunction(Function *f);
	static AddressOfInterest *NewReturnAddress(	Addr32 returnAddress,
							Function *f);

	AddressOfInterest() : m_RefCnt(1) {}

	enum {
		RETURNADDRESS,
		FUNCTION
	} m_Type;
	Addr32 m_Address;
	unsigned int m_RefCnt;
	Function *m_Function;
};

class FunctionInterceptor {
public:
	std::map<Addr32, AddressOfInterest *> m_AddressOfInterestMap;
	std::list<Function *> m_FunctionList;
	void *m_Arg;
	Machine *m_Machine;

	FunctionInterceptor(Machine *machine) : m_Machine(machine) {}
	~FunctionInterceptor();

	void InitArg(void *arg);
	int DoAddress(Addr32 pc, void *cpuState);
	void NewFunction(Function *f);
};

#endif
