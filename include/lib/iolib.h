#ifndef EASYLANG_IOLIB_H
#define EASYLANG_IOLIB_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "PrimativeValue.h"
#include "Vm.h"

vm_object* print(vm_system* vm);
vm_object* readLine(vm_system* vm);

class IOLibInit {
public:
	IOLibInit();
};

#endif //EASYLANG_IOLIB_H
