#ifndef EASYLANG_ARRAYLIB_H
#define EASYLANG_ARRAYLIB_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "PrimativeValue.h"
#include "Vm.h"

vm_object* addTo(vm_system* vm);

class ArrayLibInit {
public:
    ArrayLibInit();
};

#endif //EASYLANG_ARRAYLIB_H
