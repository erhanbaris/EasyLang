//
// Created by Erhan on 18/12/2017.
//

#ifndef EASYLANG_SCOPE_H
#define EASYLANG_SCOPE_H

#include <string>
#include <unordered_map>
#include "PrimativeValue.h"


class Scope {
public:
    Scope();
    Scope(Scope* pBaseScope);
    PrimativeValue* GetVariable(std::wstring const & key);
    void SetVariable(std::wstring const & key, PrimativeValue* value);

private:
    Scope* baseScope { nullptr };
    std::unordered_map<std::wstring, PrimativeValue*> variables;
	std::unordered_map<std::wstring, PrimativeValue*>::iterator variablesEnd;
};


#endif //EASYLANG_SCOPE_H
