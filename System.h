#ifndef EASYLANG_SYSTEM_H
#define EASYLANG_SYSTEM_H

#include <vector>
#include <memory>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <cmath>
#include <unordered_map>

struct PrimativeValue;
typedef void(*MethodCallback)(std::shared_ptr<std::vector<PrimativeValue*> > const & args, PrimativeValue & returnValue);

class System {
public:
	static std::unordered_map<std::wstring, MethodCallback> SystemMethods;
	static void WarmUp();
};

#endif //EASYLANG_SYSTEM_H
