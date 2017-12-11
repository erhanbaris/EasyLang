#include "iolib.h"
#include "Macros.h"
#include "System.h"

void print(std::shared_ptr<std::vector<PrimativeValue*> > const & args, PrimativeValue & returnValue)
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

void readline(std::shared_ptr<std::vector<PrimativeValue*> > const & args, PrimativeValue & returnValue)
{
	std::wstring text;
	std::getline(std::wcin, text);
	returnValue.SetString(text);
}


IOLibInit::IOLibInit()
{
	System::SystemMethods[L"yaz"] = &print;
	System::SystemMethods[L"print"] = &print;
	System::SystemMethods[L"readline"] = &readline;
}