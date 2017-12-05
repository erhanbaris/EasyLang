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
#include <unordered_set>

#include "Macros.h"

DECLARE_ENUM(EASY_TOKEN_TYPE, TOKEN_NONE,
			 INTEGER,
			 DOUBLE,
			 SYMBOL,
			 OPERATOR,
			 TEXT,
			 VARIABLE,
			 KEYWORD)

DECLARE_ENUM(EASY_OPERATOR_TYPE,
	OPERATOR_NONE,
	PLUS,
	MINUS,
	MULTIPLICATION,
	DIVISION,
	EQUAL,
	NOT_EQUAL,
	GREATOR,
	LOWER,
	GREATOR_EQUAL,
	LOWER_EQUAL,
	SINGLE_QUOTES,
	DOUBLE_QUOTES,
	LEFT_PARENTHESES,
	RIGHT_PARENTHESES)

DECLARE_ENUM(EASY_KEYWORD_TYPE,
	KEYWORD_NONE,
	IF,
	ASSIGNMENT,
	ELSE,
	THEN,
	ASSIGNMENT_SUFFIX,
	OR,
	AND)

struct EnumClassHash
{
	template <typename T>
	std::size_t operator()(T t) const
	{
		return static_cast<std::size_t>(t);
	}
};

static std::unordered_map<std::wstring, EASY_OPERATOR_TYPE> Operators {
        { L"topla", EASY_OPERATOR_TYPE::PLUS },
		{ L"toplam", EASY_OPERATOR_TYPE::PLUS },
		{ L"eşit", EASY_OPERATOR_TYPE::EQUAL },
		{ L"eşittir", EASY_OPERATOR_TYPE::EQUAL },
		{ L"eşitdeğil", EASY_OPERATOR_TYPE::NOT_EQUAL },
		{ L"eşitdeğildir", EASY_OPERATOR_TYPE::NOT_EQUAL },
		{ L"ekle", EASY_OPERATOR_TYPE::PLUS },
		{ L"çıkar", EASY_OPERATOR_TYPE::DIVISION},
		{ L"çıkart", EASY_OPERATOR_TYPE::DIVISION },
        { L"küçük", EASY_OPERATOR_TYPE::LOWER },
		{ L"küçükse", EASY_OPERATOR_TYPE::LOWER },
		{ L"küçükise", EASY_OPERATOR_TYPE::LOWER },
		{ L"küçüktür", EASY_OPERATOR_TYPE::LOWER },
		{ L"küçükeşit", EASY_OPERATOR_TYPE::LOWER_EQUAL },
		{ L"büyük", EASY_OPERATOR_TYPE::GREATOR },
		{ L"büyükse", EASY_OPERATOR_TYPE::GREATOR },
		{ L"büyükise", EASY_OPERATOR_TYPE::GREATOR },
		{ L"büyüktür", EASY_OPERATOR_TYPE::GREATOR },
		{ L"küçükeşit", EASY_OPERATOR_TYPE::GREATOR_EQUAL },
};
static std::unordered_map<std::wstring, EASY_OPERATOR_TYPE>::const_iterator OperatorsEnd = Operators.cend();

static std::unordered_set<EASY_OPERATOR_TYPE, EnumClassHash> BinaryOperators {
	EASY_OPERATOR_TYPE::MINUS,
	EASY_OPERATOR_TYPE::PLUS,
	EASY_OPERATOR_TYPE::DIVISION,
	EASY_OPERATOR_TYPE::MULTIPLICATION
};
static std::unordered_set<EASY_OPERATOR_TYPE, EnumClassHash>::const_iterator BinaryOperatorsEnd = BinaryOperators.cend();

static std::unordered_set<EASY_OPERATOR_TYPE, EnumClassHash> ControlOperators{
	EASY_OPERATOR_TYPE::EQUAL,
	EASY_OPERATOR_TYPE::NOT_EQUAL,
	EASY_OPERATOR_TYPE::LOWER,
	EASY_OPERATOR_TYPE::LOWER_EQUAL,
	EASY_OPERATOR_TYPE::GREATOR,
	EASY_OPERATOR_TYPE::GREATOR_EQUAL,
};
static std::unordered_set<EASY_OPERATOR_TYPE, EnumClassHash>::const_iterator ControlOperatorsEnd = ControlOperators.cend();

static std::unordered_map<std::wstring, EASY_KEYWORD_TYPE> Keywords {
	{ L"eğer", EASY_KEYWORD_TYPE::IF },
	{ L"atama", EASY_KEYWORD_TYPE::ASSIGNMENT },
	{ L"değilse", EASY_KEYWORD_TYPE::ELSE },
	{ L"sonra", EASY_KEYWORD_TYPE::THEN },
	{ L"a", EASY_KEYWORD_TYPE::ASSIGNMENT_SUFFIX },
	{ L"e", EASY_KEYWORD_TYPE::ASSIGNMENT_SUFFIX },
	{ L"ve", EASY_KEYWORD_TYPE::AND},
	{ L"veya", EASY_KEYWORD_TYPE::OR}
};
static std::unordered_map<std::wstring, EASY_KEYWORD_TYPE>::const_iterator KeywordsEnd = Keywords.cend();

class Token
{
public:
	EASY_TOKEN_TYPE GetType() { return Type; }

protected:
	EASY_TOKEN_TYPE Type;
};

class IntegerToken : Token {
public:
    int Value;
    IntegerToken() : Token() { Type = EASY_TOKEN_TYPE::INTEGER; }
};

class DoubleToken : Token {
public:
    double Value;
    DoubleToken() : Token() { Type = EASY_TOKEN_TYPE::DOUBLE; }
};

class OperatorToken : Token {
public:
    EASY_OPERATOR_TYPE Value;
    OperatorToken() : Token() { Type = EASY_TOKEN_TYPE::OPERATOR; }
};

class SymbolToken : Token {
public:
    std::wstring Value;
    SymbolToken() : Token() { Type = EASY_TOKEN_TYPE::SYMBOL; }
};

class TextToken : Token {
public:
    std::wstring Value;
    TextToken() : Token() { Type = EASY_TOKEN_TYPE::TEXT; }
};

class VariableToken : Token {
public:
    std::wstring Value;
    VariableToken() : Token() { Type = EASY_TOKEN_TYPE::VARIABLE; }
};

class KeywordToken : Token {
public:
	EASY_KEYWORD_TYPE Value;
	KeywordToken() : Token() { Type = EASY_TOKEN_TYPE::KEYWORD; }
};

class Tokinizer
{
public:
    virtual void Parse(std::wstring const & data, std::shared_ptr<std::vector<Token*>> Tokens) = 0;
	virtual void Dump(std::shared_ptr <std::vector<Token *>> Tokens) = 0;
};

class StandartTokinizerImpl;
class StandartTokinizer : public Tokinizer {
public:
    StandartTokinizer();
    void Parse(std::wstring const &data, std::shared_ptr <std::vector<Token *>> Tokens) override;
	void Dump(std::shared_ptr <std::vector<Token *>> Tokens) override;
    bool HasError();
    std::wstring ErrorMessage();

private:
    StandartTokinizerImpl *impl;
};


#endif //EASYLANG_LEXER_H
