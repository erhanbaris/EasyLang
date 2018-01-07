#include "Backend.h"
#include "Scope.h"
#include "CodeGenerator.h"

#ifndef EASYLANG_VMBACKEND_H
#define EASYLANG_VMBACKEND_H

class VmBackendImpl;
class VmBackend :
		public Backend,
		public StmtVisitor<void>,
		public ExprVisitor<void>
{
public:

	VmBackend();
	~VmBackend();

	void Prepare(std::shared_ptr<std::vector<Ast*>> pAsts) override;
	PrimativeValue* getPrimative(Ast* ast);
	void Generate();
	PrimativeValue* getData(Ast* ast);
	PrimativeValue* Execute() override;

	void visit(AssignmentAst* ast) override;
	void visit(BlockAst* ast) override;
	void visit(IfStatementAst* ast) override;
	void visit(FunctionDefinetionAst* ast) override;
	void visit(ForStatementAst* ast) override;
	void visit(VariableAst* ast) override;
	void visit(PrimativeAst* ast) override;
	void visit(ControlAst* ast) override;
	void visit(BinaryAst* ast) override;
	void visit(StructAst* ast) override;
	void visit(ReturnAst* ast) override;
	void visit(ParenthesesGroupAst* ast) override;
	void visit(FunctionCallAst* ast) override;
	void visit(UnaryAst* ast) override;
	void visit(ExprStatementAst* ast) override;

private:
	std::vector<Ast*> asts;
	std::vector<Ast*> temporaryAsts;
	std::unordered_map<string_type, PrimativeValue*> variables;
	VmBackendImpl* impl;
};

#endif //EASYLANG_VMBACKEND_H
