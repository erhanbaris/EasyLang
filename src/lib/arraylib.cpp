#include "lib/arraylib.h"
#include "Macros.h"
#include "System.h"
#include "Exceptions.h"

Void addTo(std::vector<Any> & args, Any & item)
{
	args.push_back(item);
	return Void();
}

ArrayLibInit::ArrayLibInit()
{
    System::SystemPackages[_T("array")] = std::unordered_map<string_type, Caller*>();
    System::SystemPackages[_T("array")][_T("addTo")] = def_function(addTo);
}
