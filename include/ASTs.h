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
    RETURN,
    UNARY,
    EXPR_STATEMENT
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
class UnaryAst;
class ExprStatementAst;

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

template <typename R = void>
class ExprVisitor : public BaseVisitor,
					public Visitor<VariableAst, R>,
					public Visitor<PrimativeAst, R>,
					public Visitor<ControlAst, R>,
					public Visitor<BinaryAst, R>,
					public Visitor<StructAst, R>,
					public Visitor<ReturnAst, R>,
					public Visitor<ParenthesesGroupAst, R>,
					public Visitor<FunctionCallAst, R>,
                    public Visitor<UnaryAst, R>
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
    virtual R visit(UnaryAst* ast) = 0;
};

template <typename R = void>
class StmtVisitor : public BaseVisitor,
                    public Visitor<AssignmentAst, R>,
                    public Visitor<BlockAst, R>,
                    public Visitor<IfStatementAst, R>,
                    public Visitor<FunctionDefinetionAst, R>,
                    public Visitor<ForStatementAst, R>,
					public Visitor<ExprStatementAst, R>
{
public:
    virtual R visit(AssignmentAst* ast) = 0;
    virtual R visit(BlockAst* ast) = 0;
    virtual R visit(IfStatementAst* ast) = 0;
    virtual R visit(FunctionDefinetionAst* ast) = 0;
	virtual R visit(ForStatementAst* ast) = 0;
	virtual R visit(ExprStatementAst* ast) = 0;
};

class Ast
{
public:
	EASY_AST_TYPE GetType() { return Type; }

protected:
	EASY_AST_TYPE Type;
};

class StmtAst : public Ast
{
public:
	virtual string_type print(StmtVisitor<string_type>*) = 0;
	virtual void accept(StmtVisitor<void>*) = 0;
};

class ExprAst : public Ast
{
public:
    virtual string_type print(ExprVisitor<string_type>*) = 0;
	virtual void accept(ExprVisitor<void>*) = 0;
};

class AssignmentAst : public StmtAst {
public:
	string_type Name;
	Ast* Data{ nullptr };
	AssignmentAst() { Type = EASY_AST_TYPE::ASSIGNMENT; }
	string_type print(StmtVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(StmtVisitor<void>* visitor) override { visitor->visit(this); }
};

class UnaryAst : public ExprAst
{
public:
    OperatorToken Token;
    ExprAst* Data{ nullptr };
    UnaryAst() { Type = EASY_AST_TYPE::UNARY; }
    string_type print(ExprVisitor<string_type>* visitor) override { return visitor->visit(this); }
    void accept(ExprVisitor<void>* visitor) override { visitor->visit(this); }
};

class VariableAst : public ExprAst {
public:
	string_type Value;
	VariableAst() { Type = EASY_AST_TYPE::VARIABLE; }
    VariableAst(string_type value) { Type = EASY_AST_TYPE::VARIABLE; Value = value; }
    string_type print(ExprVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(ExprVisitor<void>* visitor) override { visitor->visit(this); }
};

class PrimativeAst : public ExprAst {
public:
	PrimativeValue* Value;

	PrimativeAst() { Type = EASY_AST_TYPE::PRIMATIVE;  Value = new PrimativeValue(); }
	PrimativeAst(int value) : ExprAst() { Type = EASY_AST_TYPE::PRIMATIVE; Value = new PrimativeValue(value); }
	PrimativeAst(double value) : ExprAst() { Type = EASY_AST_TYPE::PRIMATIVE; Value = new PrimativeValue(value); }
	PrimativeAst(string_type value) : ExprAst() { Type = EASY_AST_TYPE::PRIMATIVE; Value = new PrimativeValue(value); }
	PrimativeAst(bool value) : ExprAst() { Type = EASY_AST_TYPE::PRIMATIVE; Value = new PrimativeValue(value); }
	PrimativeAst(std::vector<PrimativeValue*>* value) : ExprAst() { Type = EASY_AST_TYPE::PRIMATIVE; Value = new PrimativeValue(value); }
    PrimativeAst(std::unordered_map<string_type, PrimativeValue*>* value) : ExprAst() { Type = EASY_AST_TYPE::PRIMATIVE; Value = new PrimativeValue(value); }
    string_type print(ExprVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(ExprVisitor<void>* visitor) override { visitor->visit(this); }
};

class ControlAst : public ExprAst
{
public:
	Ast* Left{nullptr};
	Ast* Right{nullptr};
	EASY_OPERATOR_TYPE Op;
    ControlAst() : Left(nullptr), Right(nullptr) { Type = EASY_AST_TYPE::CONTROL_OPERATION; Op = EASY_OPERATOR_TYPE::OPERATOR_NONE; }
    string_type print(ExprVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(ExprVisitor<void>* visitor) override { visitor->visit(this); }
};

class BlockAst : public StmtAst
{
public:
	std::shared_ptr <std::vector<Ast*>> Blocks;
    BlockAst() { Type = EASY_AST_TYPE::BLOCK; Blocks = std::make_shared<std::vector<Ast*>>(); }
	string_type print(StmtVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(StmtVisitor<void>* visitor) override { visitor->visit(this); }
};

class BinaryAst : public ExprAst
{
public:
	Ast* Left{ nullptr };
	Ast* Right{ nullptr };
	EASY_OPERATOR_TYPE Op;
    BinaryAst() : Left(nullptr), Right(nullptr) { Type = EASY_AST_TYPE::BINARY_OPERATION; Op = EASY_OPERATOR_TYPE::OPERATOR_NONE; }
    string_type print(ExprVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(ExprVisitor<void>* visitor) override { visitor->visit(this); }
};

class StructAst : public ExprAst
{
public:
	Ast* Target{ nullptr };
	Ast* Source1{ nullptr };
	Ast* Source2{ nullptr };
	EASY_OPERATOR_TYPE Op;
    StructAst() : Target(nullptr), Source1(nullptr), Source2(nullptr) { Type = EASY_AST_TYPE::STRUCT_OPERATION; Op = EASY_OPERATOR_TYPE::OPERATOR_NONE; }
    string_type print(ExprVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(ExprVisitor<void>* visitor) override { visitor->visit(this); }
};

class IfStatementAst : public StmtAst
{
public:
    Ast* ControlOpt{nullptr};
    Ast* True{nullptr};
    Ast* False{nullptr};
    IfStatementAst() { Type = EASY_AST_TYPE::IF_STATEMENT; }
	string_type print(StmtVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(StmtVisitor<void>* visitor) override { visitor->visit(this); }
};

class FunctionDefinetionAst : public StmtAst
{
public:
	string_type Name;
	std::vector<string_type> Args;
	Ast* Body {nullptr};
    FunctionDefinetionAst() { Type = EASY_AST_TYPE::FUNCTION_DECLERATION; }
	string_type print(StmtVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(StmtVisitor<void>* visitor) override { visitor->visit(this); }
};

class ReturnAst : public ExprAst
{
public:
    Ast* Data {nullptr};
    ReturnAst() { Type = EASY_AST_TYPE::RETURN; }
    string_type print(ExprVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(ExprVisitor<void>* visitor) override { visitor->visit(this); }
};

class ParenthesesGroupAst : public ExprAst {
public:
	Ast* Data{ nullptr };
    ParenthesesGroupAst() { Type = EASY_AST_TYPE::PARENTHESES_BLOCK; }
    string_type print(ExprVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(ExprVisitor<void>* visitor) override { visitor->visit(this); }
};

class ForStatementAst : public StmtAst
{
public:
    string_type Variable;
    Ast* Start{nullptr};
    Ast* End{nullptr};
    Ast* Repeat{nullptr};
    ForStatementAst() { Type = EASY_AST_TYPE::FOR; }
	string_type print(StmtVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(StmtVisitor<void>* visitor) override { visitor->visit(this); }
};

class ExprStatementAst : public StmtAst
{
public:
    ExprAst* Expr{nullptr};
    ExprStatementAst() { Type = EASY_AST_TYPE::EXPR_STATEMENT; }
    string_type print(StmtVisitor<string_type>* visitor) override { return visitor->visit(this); }
    void accept(StmtVisitor<void>* visitor) override { visitor->visit(this); }
};


class FunctionCallAst : public ExprAst
{
public:
    string_type Function;
	string_type Package;
    std::vector<Ast*> Args;
    FunctionCallAst() { Type = EASY_AST_TYPE::FUNCTION_CALL; }
    string_type print(ExprVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(ExprVisitor<void>* visitor) override { visitor->visit(this); }
};

class PrintVisitor : public StmtVisitor<string_type>,
						 public ExprVisitor<string_type>
{
public:
	string_type visit(AssignmentAst* ast){ return _T(""); }
	string_type visit(BlockAst* ast) { return _T(""); }
	string_type visit(IfStatementAst* ast) { return _T(""); }
	string_type visit(FunctionDefinetionAst* ast) { return _T(""); }
	string_type visit(ForStatementAst* ast) { return _T(""); }

	string_type visit(VariableAst* ast) { return _T(""); }
	string_type visit(PrimativeAst* ast) { return _T(""); }
	string_type visit(ControlAst* ast) { return _T(""); }
	string_type visit(BinaryAst* ast) { return _T(""); }
	string_type visit(StructAst* ast) { return _T(""); }
	string_type visit(ReturnAst* ast) { return _T(""); }
	string_type visit(ParenthesesGroupAst* ast) { return _T(""); }
	string_type visit(FunctionCallAst* ast) { return _T(""); }
	string_type visit(UnaryAst* ast) { return _T(""); }
	string_type visit(ExprStatementAst* ast) { return _T(""); }
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
