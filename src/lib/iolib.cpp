#include "lib/iolib.h"
#include "Macros.h"
#include "System.h"
#include "Exceptions.h"

vm_object* print(vm_system* vm)
{
    auto* item = vm->getObject();
    if (item != nullptr)
        switch (item->Type) {
            case vm_object::vm_object_type::BOOL:
                console_out << item->Bool << '\n';
                break;

            case vm_object::vm_object_type::INT:
                console_out << item->Int << '\n';
                break;

            case vm_object::vm_object_type::DOUBLE:
                console_out << item->Double << '\n';
                break;

            case vm_object::vm_object_type::STR:
                console_out << static_cast<char_type*>(item->Pointer) << '\n';
                break;
        }

    return nullptr;
}

vm_object* readLine(vm_system* vm)
{
    string_type text;
    std::getline(console_in, text);

    vm_object* returnValue = new vm_object(text.c_str());
    return returnValue;
}

IOLibInit::IOLibInit()
{
    System::SystemMethods[_T("io::print")] = print;
    System::SystemMethods[_T("io::readline")] = readLine;
}
