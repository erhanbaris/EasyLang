#include "Scope.h"

Scope* Scope::GlobalScope;

Scope::Scope() : variablesEnd(variables.end())
{
    baseScope = nullptr;
}

Scope::Scope(Scope* pBaseScope) : variablesEnd(variables.end())
{
    baseScope = pBaseScope;
}

bool Scope::setVariable(std::wstring const & key, PrimativeValue* value)
{
    auto item = variables.find(key);
    if (item != variablesEnd)
    {
        item->second = value;
        return true;
    }

    if (this->baseScope != nullptr)
        return baseScope->setVariable(key, value);

    return false;
}

PrimativeValue* Scope::GetVariable(std::wstring const & key)
{
	auto item = variables.find(key);
    if (item != variablesEnd)
        return item->second;

    if (baseScope != nullptr)
        return baseScope->GetVariable(key);

    return nullptr;
}

void Scope::SetVariable(std::wstring const & key, PrimativeValue* value)
{
    auto item = variables.find(key);
    if (item != variablesEnd)
        item->second = value;
    else if (!setVariable(key, value))
    {
        variables[key] = value;
        variablesEnd = variables.end();
    }
}

/*
 func fibonacci(num) { if num <= 1 then return 1 left = fibonacci(num - 1) right = fibonacci(num - 2) return left + right }
 * */