#include <lib/corelib.h>
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
            returnValue.SetString(item->Bool ? _T("true"): _T("false"));
            break;
            
        case PrimativeValue::Type::PRI_DOUBLE:
            returnValue.SetString(AS_STRING(item->Double));
            break;
            
        case PrimativeValue::Type::PRI_INTEGER:
            returnValue.SetString(AS_STRING(item->Integer));
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

void length(FunctionArgs const & args, PrimativeValue & returnValue)
{
	REQUIRED_ARGUMENT_COUNT(1);
	auto* item = args->at(0);
	switch (args->at(0)->Type)
	{
	case PrimativeValue::Type::PRI_STRING:
		returnValue.SetInteger(item->String->size());
		break;

	case PrimativeValue::Type::PRI_ARRAY:
		returnValue.SetInteger(item->Array->size());
		break;

	case PrimativeValue::Type::PRI_DICTIONARY:
		returnValue.SetInteger(item->Dictionary->size());
		break;

	default:
		returnValue.SetNull();
		break;
	}
}

CoreLibInit::CoreLibInit()
{
    System::SystemPackages[_T("core")] = std::unordered_map<string_type, MethodCallback>();

    System::SystemPackages[_T("core")][_T("toInt")] = &toInt;
    System::SystemPackages[_T("core")][_T("toDouble")] = &toDouble;
    System::SystemPackages[_T("core")][_T("toString")] = &toString;
    System::SystemPackages[_T("core")][_T("toBool")] = &toBool;
    System::SystemPackages[_T("core")][_T("isEmpty")] = &isEmpty;

    System::SystemPackages[_T("core")][_T("isInt")] = &isInt;
    System::SystemPackages[_T("core")][_T("isDouble")] = &isDouble;
    System::SystemPackages[_T("core")][_T("isString")] = &isString;
    System::SystemPackages[_T("core")][_T("isBool")] = &isBool;
    System::SystemPackages[_T("core")][_T("isArray")] = &isArray;
    System::SystemPackages[_T("core")][_T("isDictionary")] = &isDictionary;
	System::SystemPackages[_T("core")][_T("length")] = &length;
}
