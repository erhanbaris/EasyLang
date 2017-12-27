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

bool Scope::setVariable(string_type const & key, PrimativeValue* value)
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

PrimativeValue* Scope::GetVariable(string_type const & key)
{
	auto item = variables.find(key);
    if (item != variablesEnd)
        return item->second;

    if (baseScope != nullptr)
        return baseScope->GetVariable(key);

    return nullptr;
}

void Scope::SetVariable(string_type const & key, PrimativeValue* value)
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