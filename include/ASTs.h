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

class Ast
{
public:
	EASY_AST_TYPE GetType() { return Type; }

protected:
	EASY_AST_TYPE Type;
};

class AssignmentAst : public Ast {
public:
	string_type Name;
	Ast* Data{ nullptr };
	AssignmentAst() { Type = EASY_AST_TYPE::ASSIGNMENT; }
};

class VariableAst : public Ast {
public:
	string_type Value;
	VariableAst() { Type = EASY_AST_TYPE::VARIABLE; }
	VariableAst(string_type value) { Type = EASY_AST_TYPE::VARIABLE; Value = value; }
};

class PrimativeAst : public Ast {
public:
	PrimativeValue* Value;

	PrimativeAst() { Type = EASY_AST_TYPE::PRIMATIVE;  Value = new PrimativeValue(); }
	PrimativeAst(int value) : Ast() { Type = EASY_AST_TYPE::PRIMATIVE; Value = new PrimativeValue(value); }
	PrimativeAst(double value) : Ast() { Type = EASY_AST_TYPE::PRIMATIVE; Value = new PrimativeValue(value); }
	PrimativeAst(string_type value) : Ast() { Type = EASY_AST_TYPE::PRIMATIVE; Value = new PrimativeValue(value); }
	PrimativeAst(bool value) : Ast() { Type = EASY_AST_TYPE::PRIMATIVE; Value = new PrimativeValue(value); }
	PrimativeAst(std::vector<PrimativeValue*>* value) : Ast() { Type = EASY_AST_TYPE::PRIMATIVE; Value = new PrimativeValue(value); }
	PrimativeAst(std::unordered_map<string_type, PrimativeValue*>* value) : Ast() { Type = EASY_AST_TYPE::PRIMATIVE; Value = new PrimativeValue(value); }
};

class ControlAst : public Ast
{
public:
	Ast* Left{nullptr};
	Ast* Right{nullptr};
	EASY_OPERATOR_TYPE Op;
    ControlAst() : Left(nullptr), Right(nullptr) { Type = EASY_AST_TYPE::CONTROL_OPERATION; Op = EASY_OPERATOR_TYPE::OPERATOR_NONE; }
};

class BlockAst : public Ast
{
public:
	std::shared_ptr <std::vector<Ast*>> Blocks;
	BlockAst() { Type = EASY_AST_TYPE::BLOCK; Blocks = std::make_shared<std::vector<Ast*>>(); }
};

class BinaryAst : public Ast
{
public:
	Ast* Left{ nullptr };
	Ast* Right{ nullptr };
	EASY_OPERATOR_TYPE Op;
	BinaryAst() : Left(nullptr), Right(nullptr) { Type = EASY_AST_TYPE::BINARY_OPERATION; Op = EASY_OPERATOR_TYPE::OPERATOR_NONE; }
};

class StructAst : public Ast
{
public:
	Ast* Target{ nullptr };
	Ast* Source1{ nullptr };
	Ast* Source2{ nullptr };
	EASY_OPERATOR_TYPE Op;
	StructAst() : Target(nullptr), Source1(nullptr), Source2(nullptr) { Type = EASY_AST_TYPE::STRUCT_OPERATION; Op = EASY_OPERATOR_TYPE::OPERATOR_NONE; }
};

class IfStatementAst : public Ast
{
public:
    Ast* ControlOpt{nullptr};
    Ast* True{nullptr};
    Ast* False{nullptr};
    IfStatementAst() { Type = EASY_AST_TYPE::IF_STATEMENT; }
};

class FunctionDefinetionAst : public Ast
{
public:
	string_type Name;
	std::vector<string_type> Args;
	Ast* Body {nullptr};
	FunctionDefinetionAst() { Type = EASY_AST_TYPE::FUNCTION_DECLERATION; }
};

class ReturnAst : public Ast {
public:
    Ast* Data {nullptr};
    ReturnAst() { Type = EASY_AST_TYPE::RETURN; }
};

class ParenthesesGroupAst : public Ast {
public:
	Ast* Data{ nullptr };
	ParenthesesGroupAst() { Type = EASY_AST_TYPE::PARENTHESES_BLOCK; }
};

class ForStatementAst : public Ast
{
public:
    string_type Variable;
    Ast* Start{nullptr};
    Ast* End{nullptr};
    Ast* Repeat{nullptr};
    ForStatementAst() { Type = EASY_AST_TYPE::FOR; }
};

class FunctionCallAst : public Ast
{
public:
    string_type Function;
	string_type Package;
    std::vector<Ast*> Args;
    FunctionCallAst() { Type = EASY_AST_TYPE::FUNCTION_CALL; }
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
