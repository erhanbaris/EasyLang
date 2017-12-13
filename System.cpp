#include "System.h"
#include "iolib.h"

std::unordered_map<std::wstring, MethodCallback> System::SystemMethods;
std::unordered_map<std::wstring, std::function<void (std::shared_ptr<std::vector<PrimativeValue*> > const &, PrimativeValue &)>> System::UserMethods;

void System::WarmUp()
{
	new IOLibInit();
}
