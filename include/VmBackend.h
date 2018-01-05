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
	void Generate(std::vector<size_t>& codes);
	PrimativeValue* getData(Ast* ast);
	PrimativeValue* Execute() override;

	void visit(AssignmentAst* ast);
	void visit(BlockAst* ast);
	void visit(IfStatementAst* ast);
	void visit(FunctionDefinetionAst* ast);
	void visit(ForStatementAst* ast);
	void visit(VariableAst* ast);
	void visit(PrimativeAst* ast);
	void visit(ControlAst* ast);
	void visit(BinaryAst* ast);
	void visit(StructAst* ast);
	void visit(ReturnAst* ast);
	void visit(ParenthesesGroupAst* ast);
	void visit(FunctionCallAst* ast);
	void visit(UnaryAst* ast);
	void visit(ExprStatementAst* ast);

private:
	std::vector<Ast*> asts;
	std::vector<Ast*> temporaryAsts;
	std::unordered_map<string_type, PrimativeValue*> variables;
	VmBackendImpl* impl;
};

#endif //EASYLANG_VMBACKEND_H
