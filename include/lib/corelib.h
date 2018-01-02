#ifndef EASYLANG_CORELIB_H
#define EASYLANG_CORELIB_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "PrimativeValue.h"

int toInt(Any & type);
double toDouble(Any & type);
string_type toString(Any & type);
bool toBool(Any & type);
bool isEmpty(Any & type);

bool isInt(Any & type);
bool isDouble(Any & type);
bool isString(Any & type);
bool isBool(Any & type);
bool isArray(Any & type);
bool isDictionary(Any & type);
int length(Any& data);

class CoreLibInit {
public:
    CoreLibInit();
};

#endif //EASYLANG_CORELIB_H
