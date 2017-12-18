#ifndef EASYLANG_SYSTEM_H
#define EASYLANG_SYSTEM_H

#include <vector>
#include <memory>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <cmath>
#include <unordered_map>
#include <functional>
#include "Scope.h"
#include "ASTs.h"

struct PrimativeValue;
typedef void(*MethodCallback)(std::shared_ptr<std::vector<PrimativeValue*> > const & args, PrimativeValue & returnValue);

class FunctionInfo {
public:
	FunctionDefinetionAst* FunctionAst { nullptr };
	std::function<void(std::shared_ptr<std::vector<PrimativeValue*> > const &, PrimativeValue &, Scope & scope)> Callback;
};

class System {
public:
	static std::unordered_map<std::wstring, MethodCallback> SystemMethods;
    static std::unordered_map<std::wstring, FunctionInfo*> UserMethods;
	static void WarmUp();
};

#endif //EASYLANG_SYSTEM_H