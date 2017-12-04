#ifndef EASYLANG_LEXER_H
#define EASYLANG_LEXER_H


#include <string>
#include <vector>
#include <memory>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <cmath>
#include <unordered_map>

enum class TokenType
{
    NONE,
    INTEGER,
    DOUBLE,
    SYMBOL,
    OPERATOR,
    TEXT,
    VARIABLE
};

enum class OperatorType {
    NONE,
    PLUS,
    MINUS,
    MULTIPLICATION,
    DIVISION,
    EQUAL,
    GREATOR,
    LOWER,
    GREATOR_EQUAL,
    LOWER_EQUAL,
    SINGLE_QUOTES,
    DOUBLE_QUOTES,
    DOLLAR,
    LEFT_PARENTHESES,
    RIGHT_PARENTHESES
};

std::unordered_map<std::wstring, OperatorType> BinaryOperators {
        { L"topla", OperatorType::PLUS },
        { L"küçük", OperatorType::LOWER },
        { L"küçükeşit", OperatorType::LOWER_EQUAL },
        { L"küçükeşit", OperatorType::GREATOR_EQUAL },
        { L"e", OperatorType::LOWER },
};

class Token
{
public:
    TokenType GetType() { return Type; }

protected:
    TokenType Type;
};

class IntegerToken : Token {
public:
    int Value;
    IntegerToken() : Token() { Type = TokenType::INTEGER; }
};

class DoubleToken : Token {
public:
    double Value;
    DoubleToken() : Token() { Type = TokenType::DOUBLE; }
};

class OperatorToken : Token {
public:
    OperatorType Value;
    OperatorToken() : Token() { Type = TokenType::OPERATOR; }
};

class SymbolToken : Token {
public:
    std::wstring Value;
    SymbolToken() : Token() { Type = TokenType::SYMBOL; }
};

class TextToken : Token {
public:
    std::wstring Value;
    TextToken() : Token() { Type = TokenType::TEXT; }
};

class VariableToken : Token {
public:
    std::wstring Value;
    VariableToken() : Token() { Type = TokenType::VARIABLE; }
};

class Tokinizer
{
public:
    virtual void Parse(std::wstring const & data, std::shared_ptr<std::vector<Token*>> Tokens) = 0;
};

class StandartTokinizerImpl;
class StandartTokinizer : public Tokinizer {
public:
    StandartTokinizer();
    void Parse(std::wstring const &data, std::shared_ptr <std::vector<Token *>> Tokens) override;
    bool HasError();
    std::wstring ErrorMessage();

private:
    StandartTokinizerImpl *impl;
};


#endif //EASYLANG_LEXER_H
