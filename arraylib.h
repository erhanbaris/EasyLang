#ifndef EASYLANG_ARRAYLIB_H
#define EASYLANG_ARRAYLIB_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "PrimativeValue.h"

void addTo(FunctionArgs const & args, PrimativeValue & returnValue);

class ArrayLibInit {
public:
    ArrayLibInit();
};

#endif //EASYLANG_ARRAYLIB_H
