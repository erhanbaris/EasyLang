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

typedef void (*MethodCallback)(std::wstring const & message);

enum class AstType {
	NONE,
	IF_STATEMENT,
	ASSIGNMENT,
	VARIABLE,
	PRIMATIVE,
	BINARY_OPERATION,
	CONTROL_OPERATION,
	FUNCTION_CALL,
	BLOCK
};

class Ast
{
public:
	AstType GetType() { return Type; }

protected:
	AstType Type;
};

class AssignmentAst : public Ast {
public:
	std::wstring Name;
	Ast* Data{ nullptr };
	AssignmentAst() { Type = AstType::ASSIGNMENT; }
};

class VariableAst : public Ast {
public:
	std::wstring Value;
	VariableAst() { Type = AstType::VARIABLE; }
	VariableAst(std::wstring value) { Type = AstType::VARIABLE; Value = value; }
};

struct PrimativeValue {
	enum class Type {
		PRI_NONE,
		PRI_INTEGER,
		PRI_DOUBLE,
		PRI_STRING,
		PRI_BOOL
	};

	Type Type;

	union {
		int Integer;
		double Double;
		std::wstring String;
		bool Bool;
	};

	PrimativeValue(int value) { Integer = value; Type = Type::PRI_INTEGER; }
	PrimativeValue(double value) { Double = value; Type = Type::PRI_DOUBLE;}
	PrimativeValue(std::wstring value) { new (&String) std::wstring(value); Type = Type::PRI_STRING;}
	PrimativeValue(bool value) { Bool = value; Type = Type::PRI_BOOL;}
};

class PrimativeAst : public Ast {
public:
	PrimativeValue* Value;

	PrimativeAst() { Type = AstType::PRIMATIVE; }
	PrimativeAst(int value) : Ast() { Type = AstType::PRIMATIVE; Value = new PrimativeValue(value); }
	PrimativeAst(double value) : Ast() { Type = AstType::PRIMATIVE; Value = new PrimativeValue(value); }
	PrimativeAst(std::wstring value) : Ast() { Type = AstType::PRIMATIVE; Value = new PrimativeValue(value); }
	PrimativeAst(bool value) : Ast() { Type = AstType::PRIMATIVE; Value = new PrimativeValue(value); }
};

class ControlAst : public Ast
{
public:
	Ast* Left{nullptr};
	Ast* Right{nullptr};
	EASY_OPERATOR_TYPE Op;
	ControlAst() : Op(EASY_OPERATOR_TYPE::OPERATOR_NONE), Left(nullptr), Right(nullptr) { Type = AstType::CONTROL_OPERATION; }
};

class BlockAst : public Ast
{
public:
	std::shared_ptr <std::vector<Ast*>> Blocks;
	BlockAst() { Type = AstType::BLOCK; Blocks = std::make_shared<std::vector<Ast*>>(); }
};

class BinaryAst : public Ast
{
public:
    Ast* Left{nullptr};
    Ast* Right{nullptr};
    EASY_OPERATOR_TYPE Op;
	BinaryAst() : Op(EASY_OPERATOR_TYPE::OPERATOR_NONE), Left(nullptr), Right(nullptr) { Type = AstType::BINARY_OPERATION; }
};

class IfStatementAst : public Ast
{
public:
    Ast* ControlOpt{nullptr};
    Ast* True{nullptr};
    Ast* False{nullptr};
    IfStatementAst() { Type = AstType::IF_STATEMENT; }
};

class FunctionCallAst : public Ast
{
public:
    std::wstring Function;
    std::vector<Ast*> Args;
    FunctionCallAst() { Type = AstType::FUNCTION_CALL; }
};

class AstParserImpl;
class AstParser
{
public:
    AstParser();
	void AddMethod(std::wstring const & method, MethodCallback callback);
    void Parse(std::shared_ptr<std::vector<Token*>> tokens, std::shared_ptr<std::vector<Ast*>> asts);
	void Dump(std::shared_ptr<std::vector<Ast*>> asts);

private:
    AstParserImpl* impl{nullptr};
};
#endif //EASYLANG_ASTS_H
