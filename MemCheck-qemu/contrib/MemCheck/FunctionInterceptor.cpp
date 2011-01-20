#include "FunctionInterceptor.h"

AddressOfInterest*
AddressOfInterest::NewFunction(Function *f)
{
	AddressOfInterest *aoi;

	aoi = new AddressOfInterest;
	aoi->m_Type = AddressOfInterest::FUNCTION;
	aoi->m_Address = f->m_Address;
	aoi->m_Function = f;
	return aoi;
}

AddressOfInterest*
AddressOfInterest::NewReturnAddress(Addr32 returnAddress, Function *f)
{
	AddressOfInterest *aoi;

	aoi = new AddressOfInterest;
	aoi->m_Type = AddressOfInterest::RETURNADDRESS;
	aoi->m_Address = returnAddress;
	aoi->m_Function = f;
	return aoi;
}


FunctionInterceptor::~FunctionInterceptor()
{
}

void
FunctionInterceptor::NewFunction(Function *f)
{
	m_FunctionList.push_back(f);
	m_AddressOfInterestMap[f->m_Address] =
		AddressOfInterest::NewFunction(f);
		
}

void
FunctionInterceptor::InitArg(void *arg)
{
	m_Arg = arg;
}

int
FunctionInterceptor::DoAddress(Addr32 pc, void *cpuState)
{
	std::map<Addr32, AddressOfInterest *>::iterator pos;

	pos = m_AddressOfInterestMap.find(pc);
	if (pos == m_AddressOfInterestMap.end()) {
		return 0;
	}

	AddressOfInterest *aoi;

	aoi = pos->second;
	if (aoi->m_Type == AddressOfInterest::RETURNADDRESS) {
		if (aoi->m_Function->m_Debug) {
			printf("Leave %s pc=%x retval=%x\n",
				aoi->m_Function->m_SymbolName.c_str(),
				m_Machine->GetPC(cpuState),
				m_Machine->GetReturnValue(cpuState));
			fflush(stdout);
		}
		aoi->m_Function->Leave(m_Arg, cpuState);
		aoi->m_Function->m_Active = false;
		if (--aoi->m_RefCnt == 0) {
			delete aoi;
			m_AddressOfInterestMap.erase(pos);
		}
		return 0;
	}
	// aoi->m_Type == AddressOfInterest::FUNCTION
	Addr32 returnAddress;

	returnAddress = m_Machine->GetReturnAddress(cpuState);
	if (aoi->m_Function->m_Debug) {
		unsigned int i;

		printf("Enter %s pc=%x retaddr=%x ",
				aoi->m_Function->m_SymbolName.c_str(),
				m_Machine->GetPC(cpuState),
				returnAddress);
		for (i = 1; i <= aoi->m_Function->m_NumberOfArguments; i++) {
			printf("arg_%u=%x ", i,
				m_Machine->GetArgumentN(cpuState, i));
		}
		printf("\n");
		fflush(stdout);
	}
int ret = 0;
	aoi->m_Function->Enter(m_Arg, cpuState);
if (aoi->m_Function->m_Active) {
//printf("RECURSIVE %s\n", aoi->m_Function->m_SymbolName.c_str());
//fflush(stdout);
ret = 1;
}
	aoi->m_Function->m_Active = true;
	pos = m_AddressOfInterestMap.find(returnAddress);
	if (pos != m_AddressOfInterestMap.end()) {
//printf("GOT IT\n");
//fflush(stdout);
		pos->second->m_RefCnt++;
	} else {
		m_AddressOfInterestMap[returnAddress] =
			AddressOfInterest::NewReturnAddress(returnAddress,
				aoi->m_Function);
	}
	return ret;
}
