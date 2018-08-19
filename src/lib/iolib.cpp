#include "lib/iolib.h"
#include "Macros.h"
#include "System.h"
#include "Exceptions.h"

Value print(vm_system* vm)
{
    auto value = vm->getObject();
    vm_object* o = AS_OBJ(value);
    if (IS_STRING(value))
        console_out << (char_type*)o->Pointer << '\n';
    else if (IS_NUM(value))
    {
        double val = valueToNumber(value);
        if (std::isinf(val) || std::isnan(val))
            console_out << "NAN" << '\n';
        else
        {
            char_type* str = new char_type[10];
            sprintf(str, "%g", val);
            console_out << str << '\n';
            delete str;
        }
    }
    else if (IS_BOOL(value))
        console_out << (IS_FALSE(value) ? "false" : "true") << '\n';
    else if (IS_ARRAY(value))
        console_out << "[array:" << ((vm_array*)o->Pointer)->Indicator << "]" << '\n';
    return NULL_VAL;
}

Value readLine(vm_system* vm)
{
    string_type text;
    std::getline(console_in, text);
    size_t strLen = text.size();
    char_type* chars = new char_type[strLen + 1];
    std::memcpy(chars, text.c_str(), strLen);
    chars[strLen] = 0;
    return GET_VALUE_FROM_OBJ(new vm_object(chars));
}

IOLibInit::IOLibInit()
{
    System::SystemMethods[_T("io::print")] = print;
    System::SystemMethods[_T("io::readline")] = readLine;
}
