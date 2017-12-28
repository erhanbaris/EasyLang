#ifndef EASYLANG_ARRAYLIB_H
#define EASYLANG_ARRAYLIB_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "PrimativeValue.h"

Void addTo(std::vector<Any> & args, Any & item);

class ArrayLibInit {
public:
    ArrayLibInit();
};

#endif //EASYLANG_ARRAYLIB_H
