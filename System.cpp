#include "System.h"
#include "iolib.h"


std::unordered_map<std::wstring, MethodCallback> System::SystemMethods;
std::unordered_map<std::wstring, FunctionInfo*> System::UserMethods;

void System::WarmUp()
{
	new IOLibInit();
}
