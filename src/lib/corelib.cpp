#include <lib/corelib.h>
#include "Macros.h"
#include "System.h"
#include "Exceptions.h"

int toInt(Any & type)
{
	if (type.is<int>())
		return type.cast<int>();
	else if (type.is<bool>())
		return type.cast<bool>() ? 1 : 0;
	else if (type.is<double>())
		return (int)type.cast<double>();
	else if (type.is<string_type>())
		return std::stoi(type.cast<string_type>());
	return 0;
}

double toDouble(Any & type)
{
	if (type.is<int>())
		return type.cast<int>();
	else if (type.is<bool>())
		return type.cast<bool>() ? 1 : 0;
	else if (type.is<double>())
		return (int)type.cast<double>();
	else if (type.is<string_type>())
		return std::stod(type.cast<string_type>());
	return 0.0;
}

string_type toString(Any & type)
{
	if (type.is<int>())
		return AS_STRING(type.cast<int>());
	else if (type.is<bool>())
		return type.cast<bool>() ? "true" : "false";
	else if (type.is<double>())
		return AS_STRING(type.cast<double>());
	else if (type.is<string_type>())
		return type.cast<string_type>();
	return _T("");
}
bool toBool(Any & type)
{
	if (type.is<bool>())
		return type.cast<bool>();
	else if (type.is<int>())
		return type.cast<int>() > 0;
	else if (type.is<double>())
		return type.cast<double>() > 0;
	else if (type.is<string_type>())
		return type.cast<string_type>() > 0;
	else if (type.is<std::vector<Any>>())
		return type.cast<std::vector<Any>>().size() > 0;
	else if (type.is<std::unordered_map<string_type, Any>>())
		return type.cast<std::unordered_map<string_type, Any>>().size() > 0;

	return false;
}

bool isEmpty(Any & type)
{
	if (type.is<string_type>())
		return type.cast<string_type>().size() == 0;
	else if (type.is<std::vector<Any>>())
		return type.cast<std::vector<Any>>().size() == 0;
	else if (type.is<std::unordered_map<string_type, Any>>())
		return type.cast<std::unordered_map<string_type, Any>>().size() == 0;
	else if (type.is<int>() || type.is<double>() || type.is<bool>())
		return false;

	return true;
}

bool isInt(Any & type)
{
	return type.is<int>();
}

bool isDouble(Any & type)
{
	return type.is<double>();
}

bool isString(Any & type)
{
	return type.is<string_type>();
}

bool isBool(Any & type)
{
	return type.is<bool>();
}

bool isArray(Any & type)
{
	return type.is<std::vector<Any>>();
}

bool isDictionary(Any& type)
{
	return type.is<std::unordered_map<string_type, Any>>();
}

int length(Any& data)
{
	if (data.is<bool>())
		return 1;
	else if (data.is<int>())
		return 1;
	else if (data.is<double>())
		return 1;
	else if (data.is<string_type>())
		return data.cast<string_type>().size();
	else if (data.is<std::vector<Any>>())
		return data.cast<std::vector<Any>>().size();
	else if (data.is<std::unordered_map<string_type, Any>>())
		return data.cast<std::unordered_map<string_type, Any>>().size();

	return 0;
}

CoreLibInit::CoreLibInit()
{
    System::SystemPackages[_T("core")] = std::unordered_map<string_type, Caller*>();

    System::SystemPackages[_T("core")][_T("toInt")] = def_function(toInt);
    System::SystemPackages[_T("core")][_T("toDouble")] = def_function(toDouble);
    System::SystemPackages[_T("core")][_T("toString")] = def_function(toString);
    System::SystemPackages[_T("core")][_T("toBool")] = def_function(toBool);
    System::SystemPackages[_T("core")][_T("isEmpty")] = def_function(isEmpty);

    System::SystemPackages[_T("core")][_T("isInt")] = def_function(isInt);
    System::SystemPackages[_T("core")][_T("isDouble")] = def_function(isDouble);
    System::SystemPackages[_T("core")][_T("isString")] = def_function(isString);
    System::SystemPackages[_T("core")][_T("isBool")] = def_function(isBool);
    System::SystemPackages[_T("core")][_T("isArray")] = def_function(isArray);
    System::SystemPackages[_T("core")][_T("isDictionary")] = def_function(isDictionary);
	System::SystemPackages[_T("core")][_T("length")] = def_function(length);
}
