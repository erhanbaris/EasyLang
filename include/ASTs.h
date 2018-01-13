#ifndef EASYLANG_ASTS_H
#define EASYLANG_ASTS_H


#include <vector>
#include <memory>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <cmath>
#include <unordered_map>
#include <array>


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
					public Visitor<ParenthesesGroupAst, R>,
                    public Visitor<UnaryAst, R>,
                    public Visitor<FunctionCallAst, R>,
					public Visitor<AssignmentAst, R>
{
public:
	virtual R visit(VariableAst* ast) = 0;
	virtual R visit(PrimativeAst* ast) = 0;
	virtual R visit(ControlAst* ast) = 0;
	virtual R visit(BinaryAst* ast) = 0;
	virtual R visit(StructAst* ast) = 0;
    virtual R visit(ParenthesesGroupAst* ast) = 0;
    virtual R visit(UnaryAst* ast) = 0;
    virtual R visit(FunctionCallAst* ast) = 0;
	virtual R visit(AssignmentAst* ast) = 0;
};

template <typename R = void>
class StmtVisitor : public BaseVisitor,
                    public Visitor<BlockAst, R>,
                    public Visitor<IfStatementAst, R>,
                    public Visitor<FunctionDefinetionAst, R>,
                    public Visitor<ForStatementAst, R>,
					public Visitor<ExprStatementAst, R>,
					public Visitor<ReturnAst, R>
{
public:
    virtual R visit(BlockAst* ast) = 0;
    virtual R visit(IfStatementAst* ast) = 0;
    virtual R visit(FunctionDefinetionAst* ast) = 0;
	virtual R visit(ForStatementAst* ast) = 0;
	virtual R visit(ExprStatementAst* ast) = 0;
	virtual R visit(ReturnAst* ast) = 0;
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

class AssignmentAst : public ExprAst {
public:
	string_type Name;
    ExprAst* Data{ nullptr };
	EASY_KEYWORD_TYPE VariableType;
    AssignmentAst() { Type = EASY_AST_TYPE::ASSIGNMENT; }
	AssignmentAst(string_type name, ExprAst* data) { Type = EASY_AST_TYPE::ASSIGNMENT; Name = name; Data = data; }
	string_type print(ExprVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(ExprVisitor<void>* visitor) override { visitor->visit(this); }

	~AssignmentAst()
	{
		delete Data;
	}
};

class UnaryAst : public ExprAst
{
public:
	EASY_OPERATOR_TYPE Opt;
    ExprAst* Data{ nullptr };
    UnaryAst() { Type = EASY_AST_TYPE::UNARY; }
	UnaryAst(EASY_OPERATOR_TYPE opt, ExprAst* data) { Type = EASY_AST_TYPE::UNARY; Opt = opt; Data = data; }
    string_type print(ExprVisitor<string_type>* visitor) override { return visitor->visit(this); }
    void accept(ExprVisitor<void>* visitor) override { visitor->visit(this); }

	~UnaryAst()
	{
		delete Data;
	}
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

	~PrimativeAst()
	{
		delete Value;
	}
};

class ControlAst : public ExprAst
{
public:
	ExprAst* Left{nullptr};
	ExprAst* Right{nullptr};
	EASY_OPERATOR_TYPE Op;
    ControlAst() : Left(nullptr), Right(nullptr) { Type = EASY_AST_TYPE::CONTROL_OPERATION; Op = EASY_OPERATOR_TYPE::OPERATOR_NONE; }
	ControlAst(ExprAst* left, EASY_OPERATOR_TYPE opt, ExprAst* right) { Type = EASY_AST_TYPE::CONTROL_OPERATION; Op = opt; Left = left; Right = right; }
    string_type print(ExprVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(ExprVisitor<void>* visitor) override { visitor->visit(this); }

	~ControlAst()
	{
		delete Left;
		delete Right;
	}
};

class BlockAst : public StmtAst
{
public:
	std::vector<Ast*> Blocks;
    BlockAst() { Type = EASY_AST_TYPE::BLOCK; }
	string_type print(StmtVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(StmtVisitor<void>* visitor) override { visitor->visit(this); }

	~BlockAst()
	{
		size_t totalBlocks = Blocks.size();
		for (size_t i = 0; i < totalBlocks; i++)
			delete Blocks[i];
	}
};

class BinaryAst : public ExprAst
{
public:
	ExprAst* Left{ nullptr };
	ExprAst* Right{ nullptr };
	EASY_OPERATOR_TYPE Op;
    BinaryAst() : Left(nullptr), Right(nullptr) { Type = EASY_AST_TYPE::BINARY_OPERATION; Op = EASY_OPERATOR_TYPE::OPERATOR_NONE; }
    BinaryAst(ExprAst* left, EASY_OPERATOR_TYPE opt, ExprAst* right) { Type = EASY_AST_TYPE::BINARY_OPERATION; Op = opt; Left = left; Right = right; }
    string_type print(ExprVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(ExprVisitor<void>* visitor) override { visitor->visit(this); }
	
	~BinaryAst()
	{
		delete Left;
		delete Right;
	}
};

class StructAst : public ExprAst
{
public:
	ExprAst* Target{ nullptr };
	ExprAst* Source1{ nullptr };
	ExprAst* Source2{ nullptr };
	EASY_OPERATOR_TYPE Op;
    StructAst() : Target(nullptr), Source1(nullptr), Source2(nullptr) { Type = EASY_AST_TYPE::STRUCT_OPERATION; Op = EASY_OPERATOR_TYPE::OPERATOR_NONE; }
    string_type print(ExprVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(ExprVisitor<void>* visitor) override { visitor->visit(this); }
	
	~StructAst()
	{
		delete Target;
		delete Source1;
		delete Source2;
	}
};

class IfStatementAst : public StmtAst
{
public:
    ExprAst* ControlOpt{nullptr};
    StmtAst* True{nullptr};
    StmtAst* False{nullptr};
    IfStatementAst() { Type = EASY_AST_TYPE::IF_STATEMENT; }
    IfStatementAst(ExprAst* controlOpt, StmtAst* trueAst, StmtAst* falseAst) { Type = EASY_AST_TYPE::IF_STATEMENT; ControlOpt = controlOpt; True = trueAst; False = falseAst; }
	string_type print(StmtVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(StmtVisitor<void>* visitor) override { visitor->visit(this); }
	
	~IfStatementAst()
	{
		delete ControlOpt;
		delete True;
		delete False;
	}
};

class FunctionDefinetionArg {
public:
	string_type Name;
	EASY_KEYWORD_TYPE Type;

	FunctionDefinetionArg() {}
	FunctionDefinetionArg(string_type name, EASY_KEYWORD_TYPE type) : Name(name), Type(type) {}
};

class FunctionDefinetionAst : public StmtAst
{
public:
	string_type Name;
	std::vector<FunctionDefinetionArg*> Args;
	StmtAst* Body {nullptr};
    EASY_KEYWORD_TYPE  ReturnType;
    FunctionDefinetionAst() { Type = EASY_AST_TYPE::FUNCTION_DECLERATION; }
	FunctionDefinetionAst(string_type const & name, std::vector<FunctionDefinetionArg*> & args, EASY_KEYWORD_TYPE  returnType, StmtAst* body) { Type = EASY_AST_TYPE::FUNCTION_DECLERATION; Name = name; Args = args; ReturnType = returnType; Body = body; }
	string_type print(StmtVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(StmtVisitor<void>* visitor) override { visitor->visit(this); }

	~FunctionDefinetionAst()
	{
		size_t totalArgs = Args.size();
		for (size_t i = 0; i < totalArgs; i++)
			delete Args[i];

		delete Body;
	}
};

class ReturnAst : public StmtAst
{
public:
	ExprAst* Data {nullptr};
    ReturnAst() { Type = EASY_AST_TYPE::RETURN; }
	ReturnAst(ExprAst* data) { Type = EASY_AST_TYPE::RETURN; Data = data; }
    string_type print(StmtVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(StmtVisitor<void>* visitor) override { visitor->visit(this); }

	~ReturnAst()
	{
		delete Data;
	}
};

class ParenthesesGroupAst : public ExprAst {
public:
	ExprAst* Data{ nullptr };
    ParenthesesGroupAst() { Type = EASY_AST_TYPE::PARENTHESES_BLOCK; }
    string_type print(ExprVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(ExprVisitor<void>* visitor) override { visitor->visit(this); }
	
	~ParenthesesGroupAst()
	{
		delete Data;
	}
};

class ForStatementAst : public StmtAst
{
public:
    string_type Variable;
    ExprAst* Start{nullptr};
    ExprAst* End{nullptr};
    StmtAst* Repeat{nullptr};
    ForStatementAst() { Type = EASY_AST_TYPE::FOR; }
    ForStatementAst(string_type const& variable, ExprAst* start, ExprAst* end, StmtAst* repeat) { Type = EASY_AST_TYPE::FOR; Variable = variable; Start = start; End = end; Repeat = repeat; }
	string_type print(StmtVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(StmtVisitor<void>* visitor) override { visitor->visit(this); }

	~ForStatementAst()
	{
		delete Start;
		delete End;
		delete Repeat;
	}
};

class ExprStatementAst : public StmtAst
{
public:
    ExprAst* Expr{nullptr};
    ExprStatementAst() { Type = EASY_AST_TYPE::EXPR_STATEMENT; }
    ExprStatementAst(ExprAst* expr) { Type = EASY_AST_TYPE::EXPR_STATEMENT; Expr = expr; }
    string_type print(StmtVisitor<string_type>* visitor) override { return visitor->visit(this); }
    void accept(StmtVisitor<void>* visitor) override { visitor->visit(this); }

	~ExprStatementAst()
	{
		delete Expr;
	}
};


class FunctionCallAst : public ExprAst
{
public:
    string_type Function;
	string_type Package;
    std::vector<ExprAst*> Args;
    FunctionCallAst() { Type = EASY_AST_TYPE::FUNCTION_CALL; }
    FunctionCallAst(string_type function, std::vector<ExprAst*> args) { Type = EASY_AST_TYPE::FUNCTION_CALL; Function = function; Args = args;  }
    FunctionCallAst(string_type package, string_type function, std::vector<ExprAst*> args) { Type = EASY_AST_TYPE::FUNCTION_CALL; Package = package; Function = function; Args = args;  }
    string_type print(ExprVisitor<string_type>* visitor) override { return visitor->visit(this); }
	void accept(ExprVisitor<void>* visitor) override { visitor->visit(this); }
	~FunctionCallAst()
	{
		size_t argsCount = Args.size();
		for (size_t i = 0; i < argsCount; i++)
			delete Args[i];
	}
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
	void Dump(std::shared_ptr<std::vector<Ast*>> asts);
	~AstParser()
	{
		delete impl;
	}
private:
    AstParserImpl* impl{nullptr};

	
};

#endif //EASYLANG_ASTS_H
