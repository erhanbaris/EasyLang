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
#include "Definitions.h"
#include "FunctionDispatch.h"
#include "Vm.h"

struct PrimativeValue;
typedef void(*MethodCallback)(std::shared_ptr<std::vector<PrimativeValue*> > const & args, PrimativeValue & returnValue);

class FunctionInfo {
public:
    FunctionDefinetionAst* FunctionAst { nullptr };
    std::function<void(std::unordered_map<string_type, PrimativeValue*> const &, PrimativeValue *, Scope & scope)> Callback;
};

class System {
public:
    static std::unordered_map<string_type, FunctionInfo*> UserMethods;

    static std::unordered_map<string_type, std::unordered_map<string_type, Caller*>> SystemPackages;
    static std::unordered_map<string_type, VmMethod> SystemMethods;

    static std::unordered_map<string_type, std::unordered_map<string_type, FunctionInfo*>> UserPackages;
    static void WarmUp();
};

#endif //EASYLANG_SYSTEM_H
