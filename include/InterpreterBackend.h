#include "Backend.h"
#include "Scope.h"
#include "CodeGenerator.h"

#ifndef EASYLANG_INTERPRETERBACKEND_H
#define EASYLANG_INTERPRETERBACKEND_H

class InterpreterBackend : public Backend {
public:
    
    InterpreterBackend();
    ~InterpreterBackend();
    
    void Prepare(std::shared_ptr<std::vector<Ast*>> pAsts) override;
    PrimativeValue* getPrimative(Ast* ast);
    PrimativeValue* getData(Ast* ast, Scope & scope);
	PrimativeValue* Execute() override;

private:
    std::vector<Ast*> asts;
	std::vector<Ast*> temporaryAsts;
    std::unordered_map<string_type, PrimativeValue*> variables;
	CodeGenerator* generator {nullptr};
};

#endif //EASYLANG_INTERPRETERBACKEND_H
