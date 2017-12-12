#ifndef EASYLANG_IOLIB_H
#define EASYLANG_IOLIB_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "PrimativeValue.h"

typedef std::shared_ptr<std::vector<PrimativeValue*> > FunctionArgs;

void print(FunctionArgs const & args, PrimativeValue & returnValue);
void readline(FunctionArgs const & args, PrimativeValue & returnValue);

void toInt(FunctionArgs const & args, PrimativeValue & returnValue);
void toDouble(FunctionArgs const & args, PrimativeValue & returnValue);
void toString(FunctionArgs const & args, PrimativeValue & returnValue);
void toBool(FunctionArgs const & args, PrimativeValue & returnValue);
void isEmpty(FunctionArgs const & args, PrimativeValue & returnValue);

void isInt(FunctionArgs const & args, PrimativeValue & returnValue);
void isDouble(FunctionArgs const & args, PrimativeValue & returnValue);
void isString(FunctionArgs const & args, PrimativeValue & returnValue);
void isBool(FunctionArgs const & args, PrimativeValue & returnValue);


class IOLibInit {
public:
	IOLibInit();
};

#endif //EASYLANG_PRIMATIVEVALUE_H
