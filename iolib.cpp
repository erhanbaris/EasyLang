#include "iolib.h"
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
				std::wcout << L"#ERROR Argument not found" << '\n';
				return;
			}

			std::wcout << (*it)->Describe() << '\n';
		}
	}
}

void readline(FunctionArgs const & args, PrimativeValue & returnValue)
{
	std::wstring text;
	std::getline(std::wcin, text);
	returnValue.SetString(text);
}

IOLibInit::IOLibInit()
{
    System::SystemPackages[L"io"] = std::unordered_map<std::wstring, MethodCallback>();

    System::SystemPackages[L"io"][L"print"] = &print;
    System::SystemPackages[L"io"][L"readline"] = &readline;
}
