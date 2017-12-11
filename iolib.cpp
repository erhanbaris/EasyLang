#include "iolib.h"
#include "Macros.h"
#include "System.h"
#include "Exceptions.h"

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

void toInt(std::shared_ptr<std::vector<PrimativeValue*> > const & args, PrimativeValue & returnValue)
{
    if (args->size() == 0)
        throw ParameterError("toInt method require a parameter");
    
    if (args->size() > 1)
        throw ParameterError("toInt method handle only 1 parameter");
    
    auto* item = args->at(0);
    switch (args->at(0)->Type)
    {
        case PrimativeValue::Type::PRI_BOOL:
            returnValue.SetInteger(item->Bool ? 1 : 0);
            break;
            
        case PrimativeValue::Type::PRI_DOUBLE:
            returnValue.SetInteger(item->Double);
            break;
            
        case PrimativeValue::Type::PRI_INTEGER:
            returnValue.SetInteger(item->Integer);
            break;
            
        case PrimativeValue::Type::PRI_STRING:
            returnValue.SetInteger(std::stoi(item->String));
            break;
    }
}

void toDouble(std::shared_ptr<std::vector<PrimativeValue*> > const & args, PrimativeValue & returnValue)
{
    if (args->size() == 0)
        throw ParameterError("toInt method require a parameter");
    
    if (args->size() > 1)
        throw ParameterError("toInt method handle only 1 parameter");
    
    auto* item = args->at(0);
    switch (args->at(0)->Type)
    {
        case PrimativeValue::Type::PRI_BOOL:
            returnValue.SetDouble(item->Bool ? 1.0 : 0.0);
            break;
            
        case PrimativeValue::Type::PRI_DOUBLE:
            returnValue.SetDouble(item->Double);
            break;
            
        case PrimativeValue::Type::PRI_INTEGER:
            returnValue.SetDouble(item->Integer);
            break;
            
        case PrimativeValue::Type::PRI_STRING:
            returnValue.SetDouble(std::stod(item->String));
            break;
    }
}

void toString(std::shared_ptr<std::vector<PrimativeValue*> > const & args, PrimativeValue & returnValue)
{
    if (args->size() == 0)
        throw ParameterError("toInt method require a parameter");
    
    if (args->size() > 1)
        throw ParameterError("toInt method handle only 1 parameter");
    
    auto* item = args->at(0);
    switch (args->at(0)->Type)
    {
        case PrimativeValue::Type::PRI_BOOL:
            returnValue.SetString(item->Bool ? L"true": L"false");
            break;
            
        case PrimativeValue::Type::PRI_DOUBLE:
            returnValue.SetString(std::to_wstring(item->Double));
            break;
            
        case PrimativeValue::Type::PRI_INTEGER:
            returnValue.SetString(std::to_wstring(item->Integer));
            break;
            
        case PrimativeValue::Type::PRI_STRING:
            returnValue.SetString(item->String);
            break;
    }
}

void toBool(std::shared_ptr<std::vector<PrimativeValue*> > const & args, PrimativeValue & returnValue)
{
    if (args->size() == 0)
        throw ParameterError("toInt method require a parameter");
    
    if (args->size() > 1)
        throw ParameterError("toInt method handle only 1 parameter");
    
    auto* item = args->at(0);
    switch (args->at(0)->Type)
    {
        case PrimativeValue::Type::PRI_BOOL:
            returnValue.SetBool(item->Bool);
            break;
            
        case PrimativeValue::Type::PRI_DOUBLE:
            returnValue.SetBool(item->Double >= 1.0);
            break;
            
        case PrimativeValue::Type::PRI_INTEGER:
            returnValue.SetBool(item->Integer >= 1.0);
            break;
            
        case PrimativeValue::Type::PRI_STRING:
            returnValue.SetBool(item->String.size() > 0);
            break;
    }
}

void isEmpty(std::shared_ptr<std::vector<PrimativeValue*> > const & args, PrimativeValue & returnValue)
{
    if (args->size() == 0)
        throw ParameterError("toInt method require a parameter");
    
    if (args->size() > 1)
        throw ParameterError("toInt method handle only 1 parameter");
    
    auto* item = args->at(0);
    switch (args->at(0)->Type)
    {
        case PrimativeValue::Type::PRI_BOOL:
            returnValue.SetBool(false);
            break;
            
        case PrimativeValue::Type::PRI_DOUBLE:
            returnValue.SetBool(false);
            break;
            
        case PrimativeValue::Type::PRI_INTEGER:
            returnValue.SetBool(false);
            break;
            
        case PrimativeValue::Type::PRI_STRING:
            returnValue.SetBool(item->String.size() > 0);
            break;
            
        case PrimativeValue::Type::PRI_NULL:
            returnValue.SetNull();
            break;
    }
}


IOLibInit::IOLibInit()
{
	System::SystemMethods[L"yaz"] = &print;
    System::SystemMethods[L"print"] = &print;
    System::SystemMethods[L"readline"] = &readline;
    
    System::SystemMethods[L"toInt"] = &toInt;
    System::SystemMethods[L"toDouble"] = &toDouble;
    System::SystemMethods[L"toString"] = &toString;
    System::SystemMethods[L"toBool"] = &toBool;
    System::SystemMethods[L"isEmpty"] = &isEmpty;
}
