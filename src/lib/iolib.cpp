#include "lib/iolib.h"
#include "Macros.h"
#include "System.h"
#include "Exceptions.h"

void print(FunctionArgs const & args, PrimativeValue & returnValue)
{
	if (args->size() > 0)
	{
		auto argsEnd = args->end();
		for (auto it = args->begin(); it != argsEnd; ++it)
		{
			if ((*it) == nullptr)
			{
				console_out << _T("#ERROR Argument not found\n");
				return;
			}

			console_out << (*it)->Describe() << '\n';
		}
	}
}

void readline(FunctionArgs const & args, PrimativeValue & returnValue)
{
	string_type text;
	std::getline(console_in, text);
	returnValue.SetString(text);
}

IOLibInit::IOLibInit()
{
    System::SystemPackages[_T("io")] = std::unordered_map<string_type, MethodCallback>();

    System::SystemPackages[_T("io")][_T("print")] = &print;
    System::SystemPackages[_T("io")][_T("readline")] = &readline;
}
