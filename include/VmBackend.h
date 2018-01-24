#include "Backend.h"
#include "Scope.h"

#ifndef EASYLANG_VMBACKEND_H
#define EASYLANG_VMBACKEND_H


enum class Type {
	EMPTY,
	INT,
	DOUBLE,
	STRING,
	BOOL,
	ARRAY,
	DICTIONARY
};

class VmBackendImpl;
class VmBackend :
		public Backend,
		public StmtVisitor<void>,
		public ExprVisitor<void>/*10 * 20,
		public BaseVisitor,
		public Visitor<VariableAst, Type>,
		public Visitor<PrimativeAst, Type>,
		public Visitor<ControlAst, Type>,
		public Visitor<BinaryAst, Type>,
		public Visitor<StructAst, Type>,
		public Visitor<ParenthesesGroupAst, Type>,
		public Visitor<UnaryAst, Type>,
		public Visitor<FunctionCallAst, Type>,
		public Visitor<AssignmentAst, Type>,
		public Visitor<BlockAst, Type>,
		public Visitor<IfStatementAst, Type>,
		public Visitor<FunctionDefinetionAst, Type>,
		public Visitor<ForStatementAst, Type>,
		public Visitor<ExprStatementAst, Type>,
		public Visitor<ReturnAst, Type>*/
{
public:

	VmBackend();
	~VmBackend();

	void Prepare(std::shared_ptr<std::vector<Ast*>> pAsts) override;
	PrimativeValue* getPrimative(Ast* ast);
	PrimativeValue* getAstItem(Ast* ast);
	Type detectType(Ast* ast);
    void addConvertOpcode(Type from, Type to);
	Type operationResultType(Type from, Type to);
	PrimativeValue* Execute() override;
	void Execute(std::vector<char> const & opcodes) override;
	void Compile(std::vector<char> & opcode) override;

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
	std::vector<char> opcodes;
	std::vector<Ast*> asts;
	std::vector<Ast*> temporaryAsts;
	std::unordered_map<string_type, PrimativeValue*> variables;
	VmBackendImpl* impl;
};

#endif //EASYLANG_VMBACKEND_H
