#include <lib/corelib.h>
#include "Macros.h"
#include "System.h"
#include "Exceptions.h"

vm_object* toInt(vm_system* type)
{
	vm_object const * obj = type->getObject();
	switch (obj->Type)
	{
		case vm_object::vm_object_type::INT:
			return const_cast<vm_object*>(obj);

		case vm_object::vm_object_type::BOOL:
			return new vm_object(obj->Int ? 1 : 0);

		case vm_object::vm_object_type::DOUBLE:
			return new vm_object((int)obj->Double);

		case vm_object::vm_object_type::STR:
			return new vm_object(std::stoi(static_cast<char_type*>(obj->Pointer)));

		default:
			return new vm_object();
	}
}

vm_object* toDouble(vm_system* type)
{
	vm_object const * obj = type->getObject();
	switch (obj->Type)
	{
		case vm_object::vm_object_type::INT:
			return new vm_object((double)obj->Int);

		case vm_object::vm_object_type::BOOL:
			return new vm_object(obj->Int ? 1.0 : 0.0);

		case vm_object::vm_object_type::DOUBLE:
			return const_cast<vm_object*>(obj);

		case vm_object::vm_object_type::STR:
			return new vm_object(std::stod(static_cast<char_type*>(obj->Pointer)));

		default:
			return new vm_object();
	}
}

vm_object* toString(vm_system* type)
{
	vm_object const * obj = type->getObject();
	switch (obj->Type) {
		case vm_object::vm_object_type::INT:
			return new vm_object(AS_STRING(obj->Int));

		case vm_object::vm_object_type::BOOL:
			return new vm_object(obj->Bool ? _T("true") : _T("false"));

		case vm_object::vm_object_type::DOUBLE:
			return new vm_object(AS_STRING(obj->Double));

		case vm_object::vm_object_type::STR:
			return const_cast<vm_object *>(obj);

		default:
			return new vm_object();
	}
}

vm_object* toBool(vm_system* type)
{
	vm_object const * obj = type->getObject();
	switch (obj->Type)
	{
		case vm_object::vm_object_type::INT:
			return new vm_object(obj->Int > 0);

		case vm_object::vm_object_type::BOOL:
			return const_cast<vm_object*>(obj);

		case vm_object::vm_object_type::DOUBLE:
			return new vm_object(obj->Double > 0);

		case vm_object::vm_object_type::STR:
			return new vm_object(strlen(static_cast<char_type*>(obj->Pointer)) > 0);

		case vm_object::vm_object_type::ARRAY:
			return new vm_object(static_cast<vm_array*>(obj->Pointer)->Indicator > 0);

		default:
			return new vm_object();
	}
}

vm_object* isEmpty(vm_system* type)
{
	vm_object const * obj = type->getObject();
	switch (obj->Type)
	{
		case vm_object::vm_object_type::STR:
			return new vm_object(strlen(static_cast<char_type*>(obj->Pointer)) == 0);

		case vm_object::vm_object_type::ARRAY:
			return new vm_object(static_cast<vm_array*>(obj->Pointer)->Indicator == 0);

		case vm_object::vm_object_type::EMPTY:
			return new vm_object(true);

		default:
			return new vm_object(false);
	}
}

vm_object* isInt(vm_system* type)
{
	vm_object const * obj = type->getObject();
	return new vm_object(obj->Type == vm_object::vm_object_type::INT);
}

vm_object* isDouble(vm_system* type)
{
	vm_object const * obj = type->getObject();
	return new vm_object(obj->Type == vm_object::vm_object_type::DOUBLE);
}

vm_object* isString(vm_system* type)
{
	vm_object const * obj = type->getObject();
	return new vm_object(obj->Type == vm_object::vm_object_type::STR);
}

vm_object* isBool(vm_system* type)
{
	vm_object const * obj = type->getObject();
	return new vm_object(obj->Type == vm_object::vm_object_type::BOOL);
}

vm_object* isArray(vm_system* type)
{
	vm_object const * obj = type->getObject();
	return new vm_object(obj->Type == vm_object::vm_object_type::ARRAY);
}

vm_object* isDictionary(vm_system* type)
{
	vm_object const * obj = type->getObject();
	return new vm_object(obj->Type == vm_object::vm_object_type::DICT);
}

vm_object* exitApp(vm_system* type)
{
    std::exit(0);
    return 0;
}

vm_object* length(vm_system* data)
{
	vm_object const * obj = data->getObject();
	switch (obj->Type)
	{
		case vm_object::vm_object_type::INT:
			return new vm_object(1);

		case vm_object::vm_object_type::BOOL:
			return new vm_object(1);

		case vm_object::vm_object_type::DOUBLE:
			return new vm_object(1);

		case vm_object::vm_object_type::STR:
			return new vm_object((int)strlen(static_cast<char_type*>(obj->Pointer)));

		case vm_object::vm_object_type::ARRAY:
			return new vm_object((int)static_cast<vm_array*>(obj->Pointer)->Indicator);

		default:
			return new vm_object(0);
	}
}

CoreLibInit::CoreLibInit()
{
	System::SystemMethods[_T("core::toInt")] = toInt;
	System::SystemMethods[_T("core::toDouble")] = toDouble;
	System::SystemMethods[_T("core::toString")] = toString;
	System::SystemMethods[_T("core::toBool")] = toBool;
	System::SystemMethods[_T("core::isEmpty")] = isEmpty;
	System::SystemMethods[_T("core::isInt")] = isInt;
	System::SystemMethods[_T("core::isDouble")] = isDouble;
	System::SystemMethods[_T("core::isString")] = isString;
	System::SystemMethods[_T("core::isBool")] = isBool;
	System::SystemMethods[_T("core::isArray")] = isArray;
	System::SystemMethods[_T("core::isDictionary")] = isDictionary;
	System::SystemMethods[_T("core::length")] = length;
	System::SystemMethods[_T("core::exit")] = exitApp;
}
