#include "Backend.h"
#include "Scope.h"

#if !defined(EASYLANG_LLVMBACKEND_H) && defined(EASYLANG_JIT_ACTIVE)
#define EASYLANG_LLVMBACKEND_H

namespace llvm{
    class Value;
}

class JITExprVisitor;

class CodeGenerator
{
public:
    virtual llvm::Value* generate(JITExprVisitor* visitor) = 0;
};

class JITAssignmentAst : public AssignmentAst, public CodeGenerator { };
class JITVariableAst : public VariableAst, public CodeGenerator { };
class JITPrimativeAst : public PrimativeAst, public CodeGenerator { };
class JITControlAst : public ControlAst, public CodeGenerator { };
class JITBinaryAst : public BinaryAst, public CodeGenerator { };
class JITStructAst : public StructAst, public CodeGenerator { };
class JITParenthesesGroupAst : public ParenthesesGroupAst, public CodeGenerator { };
class JITUnaryAst : public UnaryAst, public CodeGenerator { };
class JITFunctionCallAst : public FunctionCallAst, public CodeGenerator { };

class JITExprVisitor : public BaseVisitor,
public Visitor<JITAssignmentAst, llvm::Value*>,
public Visitor<JITVariableAst, llvm::Value*>,
public Visitor<JITPrimativeAst, llvm::Value*>,
public Visitor<JITControlAst, llvm::Value*>,
public Visitor<JITBinaryAst, llvm::Value*>,
public Visitor<JITStructAst, llvm::Value*>,
public Visitor<JITParenthesesGroupAst, llvm::Value*>,
public Visitor<JITUnaryAst, llvm::Value*>,
public Visitor<JITFunctionCallAst, llvm::Value*>
{
public:
    virtual llvm::Value* visit(JITAssignmentAst* ast) = 0;
    virtual llvm::Value* visit(JITVariableAst* ast) = 0;
    virtual llvm::Value* visit(JITPrimativeAst* ast) = 0;
    virtual llvm::Value* visit(JITControlAst* ast) = 0;
    virtual llvm::Value* visit(JITBinaryAst* ast) = 0;
    virtual llvm::Value* visit(JITStructAst* ast) = 0;
    virtual llvm::Value* visit(JITParenthesesGroupAst* ast) = 0;
    virtual llvm::Value* visit(JITUnaryAst* ast) = 0;
    virtual llvm::Value* visit(JITFunctionCallAst* ast) = 0;
};

class LLVMBackendImpl;
class LLVMBackend :
		public Backend,
		public StmtVisitor<void>,
		public JITExprVisitor
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
    
    llvm::Value* visit(JITAssignmentAst* ast) override;
    llvm::Value* visit(JITVariableAst* ast) override;
    llvm::Value* visit(JITPrimativeAst* ast) override;
    llvm::Value* visit(JITControlAst* ast) override;
    llvm::Value* visit(JITBinaryAst* ast) override;
    llvm::Value* visit(JITStructAst* ast) override;
    llvm::Value* visit(JITParenthesesGroupAst* ast) override;
    llvm::Value* visit(JITUnaryAst* ast) override;
    llvm::Value* visit(JITFunctionCallAst* ast) override;
    
	void visit(BlockAst* ast) override;
	void visit(IfStatementAst* ast) override;
	void visit(FunctionDefinetionAst* ast) override;
	void visit(ForStatementAst* ast) override;
	void visit(ReturnAst* ast) override;
	void visit(ExprStatementAst* ast) override;

private:
	std::vector<char_type> opcodes;
	std::vector<Ast*> asts;
	std::vector<Ast*> temporaryAsts;
	std::unordered_map<string_type, PrimativeValue*> variables;
	LLVMBackendImpl* impl;
};

#endif //EASYLANG_LLVMBACKEND_H
