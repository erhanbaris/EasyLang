#ifndef EASYLANG_CORELIB_H
#define EASYLANG_CORELIB_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "PrimativeValue.h"

void toInt(FunctionArgs const & args, PrimativeValue & returnValue);
void toDouble(FunctionArgs const & args, PrimativeValue & returnValue);
void toString(FunctionArgs const & args, PrimativeValue & returnValue);
void toBool(FunctionArgs const & args, PrimativeValue & returnValue);
void isEmpty(FunctionArgs const & args, PrimativeValue & returnValue);

void isInt(FunctionArgs const & args, PrimativeValue & returnValue);
void isDouble(FunctionArgs const & args, PrimativeValue & returnValue);
void isString(FunctionArgs const & args, PrimativeValue & returnValue);
void isBool(FunctionArgs const & args, PrimativeValue & returnValue);
void isArray(FunctionArgs const & args, PrimativeValue & returnValue);
void isDictionary(FunctionArgs const & args, PrimativeValue & returnValue);
void length(FunctionArgs const & args, PrimativeValue & returnValue);

class CoreLibInit {
public:
    CoreLibInit();
};

#endif //EASYLANG_CORELIB_H
