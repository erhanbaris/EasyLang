#include "corelib.h"
#include "Macros.h"
#include "System.h"
#include "Exceptions.h"

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

        case PrimativeValue::Type::PRI_ARRAY:
            returnValue.SetBool(item->Array->size() > 0);
            break;

        case PrimativeValue::Type::PRI_DICTIONARY:
            returnValue.SetBool(item->Dictionary->size() > 0);
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

void isArray(FunctionArgs const & args, PrimativeValue & returnValue)
{

    REQUIRED_ARGUMENT_COUNT(1);
    returnValue.SetBool(args->at(0)->IsArray());
}

void isDictionary(FunctionArgs const & args, PrimativeValue & returnValue)
{
    REQUIRED_ARGUMENT_COUNT(1);
    returnValue.SetBool(args->at(0)->IsDictionary());
}

CoreLibInit::CoreLibInit()
{
    System::SystemPackages[L"core"] = std::unordered_map<std::wstring, MethodCallback>();

    System::SystemPackages[L"core"][L"toInt"] = &toInt;
    System::SystemPackages[L"core"][L"toDouble"] = &toDouble;
    System::SystemPackages[L"core"][L"toString"] = &toString;
    System::SystemPackages[L"core"][L"toBool"] = &toBool;
    System::SystemPackages[L"core"][L"isEmpty"] = &isEmpty;

    System::SystemPackages[L"core"][L"isInt"] = &isInt;
    System::SystemPackages[L"core"][L"isDouble"] = &isDouble;
    System::SystemPackages[L"core"][L"isString"] = &isString;
    System::SystemPackages[L"core"][L"isBool"] = &isBool;
    System::SystemPackages[L"core"][L"isArray"] = &isArray;
    System::SystemPackages[L"core"][L"isDictionary"] = &isDictionary;
}
