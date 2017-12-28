#include "System.h"
#include "lib/corelib.h"
#include "lib/iolib.h"
#include "lib/arraylib.h"

std::unordered_map<string_type, FunctionInfo*> System::UserMethods;

std::unordered_map<string_type, std::unordered_map<string_type, Caller*>> System::SystemPackages;
std::unordered_map<string_type, std::unordered_map<string_type, FunctionInfo*>> System::UserPackages;

void System::WarmUp()
{
	new IOLibInit();
	new CoreLibInit();
	new ArrayLibInit();
}
