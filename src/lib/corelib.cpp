#include <lib/corelib.h>
#include "Macros.h"
#include "System.h"
#include "Exceptions.h"

Value toInt(vm_system* type)
{
    Value value = type->getObject();
    if (IS_STRING(value))
         return numberToValue(std::stoi(static_cast<char_type*>(AS_OBJ(value)->Pointer)));
    else if (IS_NUM(value))
        return numberToValue((int)valueToNumber(value));
    else if (IS_BOOL(value))
        return TRUE_VAL == value ? numberToValue(1) : numberToValue(0);
    else if (IS_ARRAY(value))
        return NULL_VAL;

    return NULL_VAL;
}

Value toDouble(vm_system* type)
{
    Value value = type->getObject();
    if (IS_STRING(value))
         return numberToValue(std::stoi(static_cast<char_type*>(AS_OBJ(value)->Pointer)));
    else if (IS_NUM(value))
        return numberToValue((double)valueToNumber(value));
    else if (IS_BOOL(value))
        return TRUE_VAL == value ? numberToValue(1) : numberToValue(0);
    else if (IS_ARRAY(value))
        return NULL_VAL;

    return NULL_VAL;
}

Value toString(vm_system* type)
{
    Value value = type->getObject();
    char_type * returnValue = nullptr;
    vm_object* o = AS_OBJ(value);

    if (IS_STRING(value))
    {
        size_t strLen = strlen((char_type*)o->Pointer);

        returnValue = new char_type[strLen + 1];
        std::memcpy(returnValue, returnValue, strLen);
        returnValue[strLen] = 0;
    }
    else if (IS_NUM(value))
    {
        double val = valueToNumber(value);
        if (std::isinf(val) || std::isnan(val))
            returnValue = "";
        else
        {
            returnValue = new char_type[10];
            sprintf(returnValue, "%g", val);
        }
    }
    else if (IS_BOOL(value))
    {
        returnValue = new char_type[6];
        std::memcpy(returnValue, IS_FALSE(value) ? "false" : "true", 5);
        returnValue[5] = 0;
    }

    return GET_VALUE_FROM_OBJ(new vm_object(returnValue));
}

Value toBool(vm_system* type)
{
    Value value = type->getObject();
    if (IS_STRING(value))
    {
        char_type* str = static_cast<char_type*>(AS_OBJ(value)->Pointer);
        if (strcmp(str, "true") == 0)
            return TRUE_VAL;

        return FALSE_VAL;
    }
    else if (IS_NUM(value))
        return valueToNumber(value) > 0 ? TRUE_VAL : FALSE_VAL;
    else if (IS_BOOL(value))
        return value;
    else if (IS_ARRAY(value))
        return NULL_VAL;

    return FALSE_VAL;
}

Value isEmpty(vm_system* type)
{
    Value value = type->getObject();
    if (IS_STRING(value))
        return strlen(static_cast<char_type*>(AS_OBJ(value)->Pointer)) > 0 ? FALSE_VAL : TRUE_VAL;
    else if (IS_ARRAY(value))
        return static_cast<vm_array*>(AS_OBJ(value)->Pointer)->Indicator > 0 ? FALSE_VAL : TRUE_VAL;

    return FALSE_VAL;
}

Value isInt(vm_system* type)
{
    Value obj = type->getObject();
    if (IS_NUM(obj))
    {
        double dVal = valueToNumber(obj);
        if (dVal == trunc(dVal))
            return TRUE_VAL;
    }

    return FALSE_VAL;
}

Value isDouble(vm_system* type)
{
    Value obj = type->getObject();
    if (IS_NUM(obj))
    {
        double dVal = valueToNumber(obj);
        if (dVal != trunc(dVal))
            return TRUE_VAL;
    }

    return FALSE_VAL;
}

Value isString(vm_system* type)
{
    return IS_STRING(type->getObject()) ? TRUE_VAL : FALSE_VAL;
}

Value isBool(vm_system* type)
{
   return IS_BOOL(type->getObject()) ? TRUE_VAL : FALSE_VAL;
}

Value isArray(vm_system* type)
{
    return IS_ARRAY(type->getObject()) ? TRUE_VAL : FALSE_VAL;
}

Value isDictionary(vm_system* type)
{
    return FALSE_VAL;
}

Value exitApp(vm_system* type)
{
    std::exit(0);
    return NULL_VAL;
}

Value length(vm_system* data)
{
    Value value = data->getObject();

    if (IS_STRING(value))
         return numberToValue(strlen(static_cast<char_type*>(AS_OBJ(value)->Pointer)));
    else if (IS_NUM(value))
        return numberToValue(1);
    else if (IS_BOOL(value))
        return numberToValue(1);
    else if (IS_ARRAY(value))
        return ((vm_array*)AS_OBJ(value))->Indicator;

    return numberToValue(0);
}

Value typeOf(vm_system* data)
{
    Value value = data->getObject();
    if (IS_STRING(value))
         return GET_VALUE_FROM_OBJ(new vm_object("string"));
    else if (IS_NUM(value))
    {
        double dVal = valueToNumber(value);
        if (dVal == trunc(dVal))
            return GET_VALUE_FROM_OBJ(new vm_object("int"));

        return GET_VALUE_FROM_OBJ(new vm_object("double"));
    }
    else if (IS_BOOL(value))
        return GET_VALUE_FROM_OBJ(new vm_object("bool"));
    else if (IS_ARRAY(value))
        return GET_VALUE_FROM_OBJ(new vm_object("array"));

    return GET_VALUE_FROM_OBJ(new vm_object("empty"));
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
    System::SystemMethods[_T("core::typeOf")] = typeOf;
}
