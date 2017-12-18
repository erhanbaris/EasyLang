#include "Scope.h"

Scope::Scope()
{
    baseScope = nullptr;
    //variablesEnd = variables.end();
}

Scope::Scope(Scope* pBaseScope)
{
    baseScope = pBaseScope;
    //variablesEnd = variables.end();
}

PrimativeValue* Scope::GetVariable(std::wstring const & key)
{
    if (variables.find(key) != variables.end())
        return variables[key];

    if (baseScope != nullptr)
        return baseScope->GetVariable(key);

    return nullptr;
}

void Scope::SetVariable(std::wstring const & key, PrimativeValue* value)
{
    variables[key] = value;
    //variablesEnd = variables.end();
}

/*
 func fibonacci(num) { if num <= 1 then return 1 left = fibonacci(num - 1) right = fibonacci(num - 2) return left + right }
 * */