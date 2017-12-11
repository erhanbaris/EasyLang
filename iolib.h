#ifndef EASYLANG_IOLIB_H
#define EASYLANG_IOLIB_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "PrimativeValue.h"


void print(std::shared_ptr<std::vector<PrimativeValue*> > const & args, PrimativeValue & returnValue);
void readline(std::shared_ptr<std::vector<PrimativeValue*> > const & args, PrimativeValue & returnValue);

class IOLibInit {
public:
	IOLibInit();
};

#endif //EASYLANG_PRIMATIVEVALUE_H
