#ifndef EASYLANG_IOLIB_H
#define EASYLANG_IOLIB_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "PrimativeValue.h"

Void print_(string_type message);
string_type readline_();

class IOLibInit {
public:
	IOLibInit();
};

#endif //EASYLANG_IOLIB_H
