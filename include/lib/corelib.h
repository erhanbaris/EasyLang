#ifndef EASYLANG_CORELIB_H
#define EASYLANG_CORELIB_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "PrimativeValue.h"
#include <Vm.h>

vm_object* toInt(vm_system*type);
vm_object* toDouble(vm_system*type);
vm_object* toString(vm_system*type);
vm_object* toBool(vm_system*type);
vm_object* isEmpty(vm_system*type);

vm_object* isInt(vm_system*type);
vm_object* isDouble(vm_system*type);
vm_object* isString(vm_system*type);
vm_object* isBool(vm_system*type);
vm_object* isArray(vm_system*type);
vm_object* isDictionary(vm_system*type);
vm_object* length(vm_system*data);
vm_object* exitApp(vm_system*type);

class CoreLibInit {
public:
    CoreLibInit();
};

#endif //EASYLANG_CORELIB_H
