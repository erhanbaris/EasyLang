#ifndef EASYLANG_SCOPE_H
#define EASYLANG_SCOPE_H

#include <string>
#include <unordered_map>
#include "PrimativeValue.h"
#include "Definitions.h"

class Scope {
public:
    Scope();
    ~Scope();
    Scope(Scope* pBaseScope);
    PrimativeValue* GetVariable(string_type const & key);
    void SetVariable(string_type const & key, PrimativeValue* value);

    static Scope* GlobalScope;

protected:
    bool setVariable(string_type const & key, PrimativeValue* value);

private:
    Scope* baseScope { nullptr };
    std::unordered_map<string_type, PrimativeValue*> variables;
	std::unordered_map<string_type, PrimativeValue*>::iterator variablesEnd;
};


#endif //EASYLANG_SCOPE_H
