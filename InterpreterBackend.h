#include "Backend.h"

#ifndef EASYLANG_INTERPRETERBACKEND_H
#define EASYLANG_INTERPRETERBACKEND_H

class InterpreterBackend : public Backend {
public:
    
    InterpreterBackend();
    InterpreterBackend(std::ostream & stream);
    ~InterpreterBackend();
    
    void Prepare(std::shared_ptr<std::vector<Ast*>> pAsts) override;
    PrimativeValue* getPrimative(Ast* ast);
    PrimativeValue* getData(Ast* ast);
    void Execute() override;

private:
    std::shared_ptr<std::vector<Ast*>> asts;
    std::unordered_map<std::wstring, PrimativeValue*> variables;
    std::ostream& stream;
};

#endif //EASYLANG_INTERPRETERBACKEND_H
