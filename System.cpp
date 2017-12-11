#include "System.h"
#include "iolib.h"

std::unordered_map<std::wstring, MethodCallback> System::SystemMethods;

void System::WarmUp()
{
	new IOLibInit();
}