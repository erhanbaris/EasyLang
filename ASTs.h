#ifndef EASYLANG_ASTS_H
#define EASYLANG_ASTS_H


#include <string>
#include <vector>
#include <memory>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <cmath>
#include <unordered_map>

#include "Lexer.h"

enum class AstType {
	NONE,
	IF_STATEMENT,
	ASSIGNMENT,
	VARIABLE,
	PRIMATIVE,
	BINARY_OPERATION
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

enum class PrimativeValueType {
	PRI_NONE,
	PRI_INTEGER,
	PRI_DOUBLE,
	PRI_STRING,
	PRI_BOOL
};

union PrimativeValue {
	int Integer;
	double Double;
	std::wstring String;
	bool Bool;

	PrimativeValue(int value) { Integer = value; }
	PrimativeValue(double value) { Double = value; }
	PrimativeValue(std::wstring value) { new (&String) std::wstring(value); }
	PrimativeValue(bool value) { Bool = value; }
	~PrimativeValue() {}
};

class PrimativeAst : public Ast {
public:
	PrimativeValue* Value;
	PrimativeValueType ValueType;

	PrimativeAst() { Type = AstType::PRIMATIVE; }
	PrimativeAst(int value) : Ast() { Type = AstType::PRIMATIVE; Value = new PrimativeValue(value); ValueType = PrimativeValueType::PRI_INTEGER; }
	PrimativeAst(double value) : Ast() { Type = AstType::PRIMATIVE; Value = new PrimativeValue(value); ValueType = PrimativeValueType::PRI_DOUBLE; }
	PrimativeAst(std::wstring value) : Ast() { Type = AstType::PRIMATIVE; Value = new PrimativeValue(value); ValueType = PrimativeValueType::PRI_STRING; }
	PrimativeAst(bool value) : Ast() { Type = AstType::PRIMATIVE; Value = new PrimativeValue(value); ValueType = PrimativeValueType::PRI_BOOL; }
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
    Ast* BinartOpt{nullptr};
    Ast* True{nullptr};
    Ast* False{nullptr};
	IfStatementAst() { Type = AstType::IF_STATEMENT; }

};

class AstParserImpl;
class AstParser
{
public:
    AstParser();
    void Parse(std::shared_ptr<std::vector<Token*>> tokens, std::shared_ptr<std::vector<Ast*>> asts);

private:
    AstParserImpl* impl{nullptr};
};
#endif //EASYLANG_ASTS_H
