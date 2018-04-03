#include "Backend.h"
#include "Scope.h"

#if !defined(EASYLANG_LLVMBACKEND_H) && defined(EASYLANG_JIT_ACTIVE)
#define EASYLANG_LLVMBACKEND_H

class LLVMBackendImpl;
class LLVMBackend :
		public Backend,
		public StmtVisitor<void>,
		public ExprVisitor<void>
{
public:

	LLVMBackend();
	~LLVMBackend();

	void Prepare(std::shared_ptr<std::vector<Ast*>> pAsts) override;
	PrimativeValue* getPrimative(Ast* ast);
	PrimativeValue* getAstItem(Ast* ast);
    BACKEND_ITEM_TYPE detectType(Ast* ast);
	BACKEND_ITEM_TYPE operationResultType(BACKEND_ITEM_TYPE from, BACKEND_ITEM_TYPE to);
	PrimativeValue* Execute() override;
	void Execute(std::vector<char_type> const & opcodes) override;
	void Compile(std::vector<char_type> & opcode) override;

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

	void visit(PrimativeValue* ast);

private:
	std::vector<char_type> opcodes;
	std::vector<Ast*> asts;
	std::vector<Ast*> temporaryAsts;
	std::unordered_map<string_type, PrimativeValue*> variables;
	LLVMBackendImpl* impl;
};

#endif //EASYLANG_LLVMBACKEND_H
