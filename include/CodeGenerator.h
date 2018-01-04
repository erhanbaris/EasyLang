#ifndef EASYLANG_CODEGENERATOR_H
#define EASYLANG_CODEGENERATOR_H

#include <type_traits>

#include <vector>
#include <memory>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <cmath>
#include <unordered_map>
#include <unordered_set>

#include "ASTs.h"

class CodeGeneratorImpl;
class CodeGenerator : 
	public StmtVisitor<void>,
	public ExprVisitor<void>
{
public:
	CodeGenerator();
	~CodeGenerator();

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
	CodeGeneratorImpl* impl;
};

#endif // EASYLANG_CODEGENERATOR_H