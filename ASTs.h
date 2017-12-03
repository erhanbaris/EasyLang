#ifndef EASYLANG_ASTS_H
#define EASYLANG_ASTS_H


#include <string>
#include <vector>
#include <memory>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <cmath>

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
    Ast* Data;
};

class VariableAst : Ast {
public:
    std::wstring Name;
};

class BinaryAst : Ast
{
public:
    Ast* Left;
    Ast* Right;
    char Op;
    BinaryAst(char pOp, Ast* pLeft, Ast* pRight): Op(pOp), Left(pLeft), Right(pRight) {}
};

class IfStatementAst : Ast
{
public:
    Ast* BinartOpt;
    Ast* True;
    Ast* False;
    IfStatementAst(Ast* pBinartOpt, Ast* pTrue, Ast* pFalse): BinartOpt(pBinartOpt), True(pTrue), False(pFalse) {}
    IfStatementAst(Ast* pBinartOpt, Ast* pTrue): BinartOpt(pBinartOpt), True(pTrue){ False = nullptr; }
};

class AstParserImpl;
class AstParser
{
public:
    AstParser();
    void Parse(std::shared_ptr<std::vector<Token*>> Tokens);

private:
    AstParserImpl* impl;
};
#endif //EASYLANG_ASTS_H
