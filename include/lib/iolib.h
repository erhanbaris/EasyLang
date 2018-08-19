#ifndef EASYLANG_IOLIB_H
#define EASYLANG_IOLIB_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "PrimativeValue.h"
#include "Vm.h"

Value print(vm_system* vm);
Value readLine(vm_system* vm);

class IOLibInit {
public:
	IOLibInit();
};

#endif //EASYLANG_IOLIB_H
