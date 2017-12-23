#include "lib/arraylib.h"
#include "Macros.h"
#include "System.h"
#include "Exceptions.h"

void addTo(FunctionArgs const & args, PrimativeValue & returnValue)
{
    if (args->size() < 2)
        throw ParameterError("Method require a parameter");

    if (!args->at(0)->IsArray())
        throw ParameterError("First parameter must be array");

    if (args->size() == 3)
    {
        auto* array = args->at(0)->Array;
        if (args->at(1)->Integer > array->size())
            array->insert(array->begin() + array->size(), args->at(2));
        else
            array->insert(array->begin() + args->at(1)->Integer, args->at(2));
    }
    else
        args->at(0)->Array->push_back(args->at(1));
}

ArrayLibInit::ArrayLibInit()
{
    System::SystemPackages[L"array"] = std::unordered_map<std::wstring, MethodCallback>();
    System::SystemPackages[L"array"][L"addTo"] = &addTo;
}
