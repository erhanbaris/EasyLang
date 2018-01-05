#include "Backend.h"
#include "Scope.h"
#include "CodeGenerator.h"

#ifndef EASYLANG_VMBACKEND_H
#define EASYLANG_VMBACKEND_H

class VmBackend : public Backend {
public:

	VmBackend();
	~VmBackend();

	void Prepare(std::shared_ptr<std::vector<Ast*>> pAsts) override;
	PrimativeValue* getPrimative(Ast* ast);
	PrimativeValue* getData(Ast* ast, Scope & scope);
	PrimativeValue* Execute() override;

private:
	std::vector<Ast*> asts;
	std::vector<Ast*> temporaryAsts;
	std::unordered_map<string_type, PrimativeValue*> variables;
	CodeGenerator* generator{ nullptr };
};

#endif //EASYLANG_VMBACKEND_H
