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

void toInt(FunctionArgs const & args, PrimativeValue & returnValue)
{
	REQUIRED_ARGUMENT_COUNT(1);
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
            returnValue.SetInteger(std::stoi(*item->String));
            break;
    }
}

void toDouble(FunctionArgs const & args, PrimativeValue & returnValue)
{
	REQUIRED_ARGUMENT_COUNT(1);
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
            returnValue.SetDouble(std::stod(*item->String));
            break;
    }
}

void toString(FunctionArgs const & args, PrimativeValue & returnValue)
{
	REQUIRED_ARGUMENT_COUNT(1);
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
            returnValue.SetString(*item->String);
            break;
    }
}

void toBool(FunctionArgs const & args, PrimativeValue & returnValue)
{
	REQUIRED_ARGUMENT_COUNT(1);
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
            returnValue.SetBool(item->String->size() > 0);
            break;
    }
}

void isEmpty(FunctionArgs const & args, PrimativeValue & returnValue)
{
	REQUIRED_ARGUMENT_COUNT(1);
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
            returnValue.SetBool(item->String->size() > 0);
            break;
            
        case PrimativeValue::Type::PRI_NULL:
            returnValue.SetNull();
            break;
    }
}


void isInt(FunctionArgs const & args, PrimativeValue & returnValue)
{
	REQUIRED_ARGUMENT_COUNT(1);
	returnValue.SetBool(args->at(0)->IsInteger());
}

void isDouble(FunctionArgs const & args, PrimativeValue & returnValue)
{
	REQUIRED_ARGUMENT_COUNT(1);
	returnValue.SetBool(args->at(0)->IsDouble());
}

void isString(FunctionArgs const & args, PrimativeValue & returnValue)
{
	REQUIRED_ARGUMENT_COUNT(1);
	returnValue.SetBool(args->at(0)->IsString());
}

void isBool(FunctionArgs const & args, PrimativeValue & returnValue)
{

	REQUIRED_ARGUMENT_COUNT(1);
	returnValue.SetBool(args->at(0)->IsBool());
}

IOLibInit::IOLibInit()
{
    System::SystemMethods[L"print"] = &print;
    System::SystemMethods[L"readline"] = &readline;
    
    System::SystemMethods[L"toInt"] = &toInt;
    System::SystemMethods[L"toDouble"] = &toDouble;
    System::SystemMethods[L"toString"] = &toString;
    System::SystemMethods[L"toBool"] = &toBool;
    System::SystemMethods[L"isEmpty"] = &isEmpty;

	System::SystemMethods[L"isInt"] = &isInt;
	System::SystemMethods[L"isDouble"] = &isDouble;
	System::SystemMethods[L"isString"] = &isString;
	System::SystemMethods[L"isBool"] = &isBool;
}
