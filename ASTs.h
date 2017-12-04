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

class Ast
{
public:

};

class IntegerAst : public Ast
{
public:
    int Value;
    IntegerAst(int pValue): Value(pValue) {}
};

class DoubleAst : public Ast
{
public:
    double Value;
    DoubleAst(double pValue): Value(pValue) {}
};

class TextAst : public Ast
{
public:
    std::wstring Value;
    TextAst(std::wstring && pValue): Value(pValue) {}
    TextAst(std::wstring & pValue): Value(pValue) {}
};

class AssignmentAst : Ast {
public:
    std::wstring Name;
    Ast* Data{nullptr};
};

class VariableAst : Ast {
public:
    std::wstring Name;
};

class BinaryAst : Ast
{
public:
    Ast* Left{nullptr};
    Ast* Right{nullptr};
    OperatorType Op;
    BinaryAst(): Op(OperatorType::NONE), Left(nullptr), Right(nullptr) {}
};

class IfStatementAst : Ast
{
public:
    Ast* BinartOpt{nullptr};
    Ast* True{nullptr};
    Ast* False{nullptr};
    IfStatementAst() = default;;

};

class AstParserImpl;
class AstParser
{
public:
    AstParser();
    void Parse(std::shared_ptr<std::vector<Token*>> Tokens);

private:
    AstParserImpl* impl{nullptr};
};
#endif //EASYLANG_ASTS_H
