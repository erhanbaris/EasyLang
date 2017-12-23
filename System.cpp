#include "System.h"
#include "corelib.h"
#include "iolib.h"
#include "arraylib.h"

std::unordered_map<std::wstring, FunctionInfo*> System::UserMethods;

std::unordered_map<std::wstring, std::unordered_map<std::wstring, MethodCallback >> System::SystemPackages;
std::unordered_map<std::wstring, std::unordered_map<std::wstring, FunctionInfo*>> System::UserPackages;

void System::WarmUp()
{
	new IOLibInit();
	new CoreLibInit();
	new ArrayLibInit();
}
