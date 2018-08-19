#ifndef EASYLANG_CORELIB_H
#define EASYLANG_CORELIB_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "PrimativeValue.h"
#include <Vm.h>

Value toInt(vm_system*type);
Value toDouble(vm_system*type);
Value toString(vm_system*type);
Value toBool(vm_system*type);
Value isEmpty(vm_system*type);

Value isInt(vm_system*type);
Value isDouble(vm_system*type);
Value isString(vm_system*type);
Value isBool(vm_system*type);
Value isArray(vm_system*type);
Value isDictionary(vm_system*type);
Value length(vm_system*data);
Value exitApp(vm_system*type);

class CoreLibInit {
public:
    CoreLibInit();
};

#endif //EASYLANG_CORELIB_H
