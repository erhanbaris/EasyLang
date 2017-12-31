#ifndef EASYLANG_ASTS_H
#define EASYLANG_ASTS_H


#include <vector>
#include <memory>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <cmath>
#include <unordered_map>


#include "Lexer.h"
#include "PrimativeValue.h"

enum class EASY_AST_TYPE {
	NONE,
	IF_STATEMENT,
	ASSIGNMENT,
	VARIABLE,
	PRIMATIVE,
	BINARY_OPERATION,
	STRUCT_OPERATION,
	CONTROL_OPERATION,
	FUNCTION_CALL,
	BLOCK,
	PARENTHESES_BLOCK,
    FOR,
	FUNCTION_DECLERATION,
    RETURN
};

class AssignmentAst;
class VariableAst;
class PrimativeAst;
class ControlAst;
class BlockAst;
class BinaryAst;
class StructAst;
class IfStatementAst;
class FunctionDefinetionAst;
class ReturnAst;
class ParenthesesGroupAst;
class ForStatementAst;
class FunctionCallAst;

class BaseVisitor
{
    public:
    virtual ~BaseVisitor() {}
};

template <class T, typename R = void>
class Visitor {
    public:
    typedef R ReturnType;
    virtual ReturnType visit(T*) = 0;
};

template <typename T>
class IExprVisitor
{
public:
    virtual T visit(VariableAst* ast) = 0;
    virtual T visit(PrimativeAst* ast) = 0;
    virtual T visit(ControlAst* ast) = 0;
    virtual T visit(BinaryAst* ast) = 0;
    virtual T visit(StructAst* ast) = 0;
    virtual T visit(ReturnAst* ast) = 0;
    virtual T visit(ParenthesesGroupAst* ast) = 0;
    virtual T visit(FunctionCallAst* ast) = 0;
};

template <typename R = void>
class ExprVisitor : public BaseVisitor,
					public Visitor<VariableAst, R>,
					public Visitor<PrimativeAst, R>,
					public Visitor<ControlAst, R>,
					public Visitor<BinaryAst, R>,
					public Visitor<StructAst, R>,
					public Visitor<ReturnAst, R>,
					public Visitor<ParenthesesGroupAst, R>,
					public Visitor<FunctionCallAst, R>
{
public:
	virtual R visit(VariableAst* ast) = 0;
	virtual R visit(PrimativeAst* ast) = 0;
	virtual R visit(ControlAst* ast) = 0;
	virtual R visit(BinaryAst* ast) = 0;
	virtual R visit(StructAst* ast) = 0;
	virtual R visit(ReturnAst* ast) = 0;
	virtual R visit(ParenthesesGroupAst* ast) = 0;
	virtual R visit(FunctionCallAst* ast) = 0;
};

class TestExprVisitor : public ExprVisitor<void>
{
public:
	virtual void visit(VariableAst* ast) = 0;
	virtual void visit(PrimativeAst* ast) = 0;
	virtual void visit(ControlAst* ast) = 0;
	virtual void visit(BinaryAst* ast) = 0;
	virtual void visit(StructAst* ast) = 0;
	virtual void visit(ReturnAst* ast) = 0;
	virtual void visit(ParenthesesGroupAst* ast) = 0;
	virtual void visit(FunctionCallAst* ast) = 0;
};

template <typename T>
class IStmtVisitor
{
public:
    virtual T visit(AssignmentAst* ast) = 0;
    virtual T visit(BlockAst* ast) = 0;
    virtual T visit(IfStatementAst* ast) = 0;
    virtual T visit(FunctionDefinetionAst* ast) = 0;
    virtual T visit(ForStatementAst* ast) = 0;
};

class Ast
{
public:
	EASY_AST_TYPE GetType() { return Type; }

protected:
	EASY_AST_TYPE Type;
};

template<class A>
class StmtAst : public Ast//, public Visitor<TestExprVisitor, void>
{
public:
    template <typename T, template <typename S> class PrintVisitor>
    T Print(PrintVisitor<T> * visitor)
    { return ((IStmtVisitor<T>*)visitor)->visit(reinterpret_cast<A*>(this)); }
    
    template <typename T, template <typename S> class AcceptVisitor>
    T Accept(AcceptVisitor<T> * visitor)
    { return ((IStmtVisitor<T>*)visitor)->visit(reinterpret_cast<A*>(this)); }
};

template<class A>
class ExprAst : public Ast
{
public:
    template <typename T, template <typename S> class PrintVisitor>
    T Print(PrintVisitor<T> * visitor)
    { return ((IExprVisitor<T>*)visitor)->visit(reinterpret_cast<A*>(this)); }
    
    template <typename T, template <typename S> class AcceptVisitor>
    T Accept(AcceptVisitor<T> * visitor)
    { return ((IExprVisitor<T>*)visitor)->visit(reinterpret_cast<A*>(this)); }
};

class AssignmentAst : public StmtAst<AssignmentAst> {
public:
	string_type Name;
	Ast* Data{ nullptr };
	AssignmentAst() { Type = EASY_AST_TYPE::ASSIGNMENT; }
};

class VariableAst : public ExprAst<VariableAst> {
public:
	string_type Value;
	VariableAst() { Type = EASY_AST_TYPE::VARIABLE; }
    VariableAst(string_type value) { Type = EASY_AST_TYPE::VARIABLE; Value = value; }
};

class PrimativeAst : public ExprAst<PrimativeAst> {
public:
	PrimativeValue* Value;

	PrimativeAst() { Type = EASY_AST_TYPE::PRIMATIVE;  Value = new PrimativeValue(); }
	PrimativeAst(int value) : ExprAst() { Type = EASY_AST_TYPE::PRIMATIVE; Value = new PrimativeValue(value); }
	PrimativeAst(double value) : ExprAst() { Type = EASY_AST_TYPE::PRIMATIVE; Value = new PrimativeValue(value); }
	PrimativeAst(string_type value) : ExprAst() { Type = EASY_AST_TYPE::PRIMATIVE; Value = new PrimativeValue(value); }
	PrimativeAst(bool value) : ExprAst() { Type = EASY_AST_TYPE::PRIMATIVE; Value = new PrimativeValue(value); }
	PrimativeAst(std::vector<PrimativeValue*>* value) : ExprAst() { Type = EASY_AST_TYPE::PRIMATIVE; Value = new PrimativeValue(value); }
    PrimativeAst(std::unordered_map<string_type, PrimativeValue*>* value) : ExprAst() { Type = EASY_AST_TYPE::PRIMATIVE; Value = new PrimativeValue(value); }
};

class ControlAst : public ExprAst<ControlAst>
{
public:
	Ast* Left{nullptr};
	Ast* Right{nullptr};
	EASY_OPERATOR_TYPE Op;
    ControlAst() : Left(nullptr), Right(nullptr) { Type = EASY_AST_TYPE::CONTROL_OPERATION; Op = EASY_OPERATOR_TYPE::OPERATOR_NONE; }
};

class BlockAst : public StmtAst<BlockAst>
{
public:
	std::shared_ptr <std::vector<Ast*>> Blocks;
    BlockAst() { Type = EASY_AST_TYPE::BLOCK; Blocks = std::make_shared<std::vector<Ast*>>(); }
};

class BinaryAst : public ExprAst<BinaryAst>
{
public:
	Ast* Left{ nullptr };
	Ast* Right{ nullptr };
	EASY_OPERATOR_TYPE Op;
    BinaryAst() : Left(nullptr), Right(nullptr) { Type = EASY_AST_TYPE::BINARY_OPERATION; Op = EASY_OPERATOR_TYPE::OPERATOR_NONE; }
};

class StructAst : public ExprAst<StructAst>
{
public:
	Ast* Target{ nullptr };
	Ast* Source1{ nullptr };
	Ast* Source2{ nullptr };
	EASY_OPERATOR_TYPE Op;
    StructAst() : Target(nullptr), Source1(nullptr), Source2(nullptr) { Type = EASY_AST_TYPE::STRUCT_OPERATION; Op = EASY_OPERATOR_TYPE::OPERATOR_NONE; }
};

class IfStatementAst : public StmtAst<IfStatementAst>
{
public:
    Ast* ControlOpt{nullptr};
    Ast* True{nullptr};
    Ast* False{nullptr};
    IfStatementAst() { Type = EASY_AST_TYPE::IF_STATEMENT; }
};

class FunctionDefinetionAst : public StmtAst<FunctionDefinetionAst>
{
public:
	string_type Name;
	std::vector<string_type> Args;
	Ast* Body {nullptr};
    FunctionDefinetionAst() { Type = EASY_AST_TYPE::FUNCTION_DECLERATION; }
};

class ReturnAst : public ExprAst<ReturnAst>
{
public:
    Ast* Data {nullptr};
    ReturnAst() { Type = EASY_AST_TYPE::RETURN; }
};

class ParenthesesGroupAst : public ExprAst<ParenthesesGroupAst> {
public:
	Ast* Data{ nullptr };
    ParenthesesGroupAst() { Type = EASY_AST_TYPE::PARENTHESES_BLOCK; }
};

class ForStatementAst : public StmtAst<ForStatementAst>
{
public:
    string_type Variable;
    Ast* Start{nullptr};
    Ast* End{nullptr};
    Ast* Repeat{nullptr};
    ForStatementAst() { Type = EASY_AST_TYPE::FOR; }
};

class FunctionCallAst : public ExprAst<FunctionCallAst>
{
public:
    string_type Function;
	string_type Package;
    std::vector<Ast*> Args;
    FunctionCallAst() { Type = EASY_AST_TYPE::FUNCTION_CALL; }
};

class PrintStmtVisitor : public IStmtVisitor<string_type>
{
public:
    string_type visit(AssignmentAst* ast){ return _T(""); }
    string_type visit(BlockAst* ast) { return _T(""); }
    string_type visit(IfStatementAst* ast) { return _T(""); }
    string_type visit(FunctionDefinetionAst* ast) { return _T(""); }
    string_type visit(ForStatementAst* ast) { return _T(""); }
};


class AstParserImpl;
class AstParser
{
public:
	AstParser();
    void Parse(std::shared_ptr<std::vector<Token*>> tokens, std::shared_ptr<std::vector<Ast*>> asts);
	void TempParse(std::shared_ptr<std::vector<Token*>> tokens, std::shared_ptr<std::vector<Ast*>> asts);
	void Dump(std::shared_ptr<std::vector<Ast*>> asts);

private:
    AstParserImpl* impl{nullptr};
};
#endif //EASYLANG_ASTS_H
