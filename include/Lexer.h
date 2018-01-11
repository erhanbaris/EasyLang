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
#include "Definitions.h"

DECLARE_ENUM(EASY_TOKEN_TYPE, TOKEN_NONE,
			 INTEGER,
			 DOUBLE,
			 SYMBOL,
			 OPERATOR,
			 TEXT,
			 VARIABLE,
			 KEYWORD,
             END_OF_FILE)

DECLARE_ENUM(EASY_OPERATOR_TYPE,
	OPERATOR_NONE,
    OPERATION,
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
	RIGHT_PARENTHESES,
	SQUARE_BRACKET_START,
	SQUARE_BRACKET_END,
	BLOCK_START,
	BLOCK_END,
	OR,
	AND,
	COMMA,
	ASSIGN,
	SINGLE_COLON,
	DOUBLE_COLON,
	UNDERLINE,
    APPEND,
	INDEXER)

DECLARE_ENUM(EASY_KEYWORD_TYPE,
	KEYWORD_NONE,
	IF,
    FOR,
	IN_KEYWORD,
	TO_KEYWORD,
	STEP_KEYWORD,
	ASSIGNMENT,
	ELSE,
	THEN,
	ASSIGNMENT_SUFFIX,
    BOOL_TRUE,
    BOOL_FALSE,
	FUNC,
	RETURN,
    PACKAGE,
	TYPE_BOOL,
	TYPE_INT,
	TYPE_DOUBLE,
	TYPE_STRING,
	TYPE_ARRAY,
	TYPE_DICTIONARY)

struct EnumClassHash
{
	template <typename T>
	std::size_t operator()(T t) const
	{
		return static_cast<std::size_t>(t);
	}
};

static std::unordered_set<EASY_OPERATOR_TYPE, EnumClassHash> BinaryOperators{
	EASY_OPERATOR_TYPE::MINUS,
	EASY_OPERATOR_TYPE::PLUS,
	EASY_OPERATOR_TYPE::DIVISION,
	EASY_OPERATOR_TYPE::MULTIPLICATION
};
static std::unordered_set<EASY_OPERATOR_TYPE, EnumClassHash>::const_iterator BinaryOperatorsEnd = BinaryOperators.cend();

static std::unordered_set<EASY_OPERATOR_TYPE, EnumClassHash> StructOperators{
	EASY_OPERATOR_TYPE::INDEXER,
	EASY_OPERATOR_TYPE::APPEND
};
static std::unordered_set<EASY_OPERATOR_TYPE, EnumClassHash>::const_iterator StructOperatorsEnd = StructOperators.cend();

static std::unordered_set<EASY_OPERATOR_TYPE, EnumClassHash> ControlOperators{
	EASY_OPERATOR_TYPE::EQUAL,
	EASY_OPERATOR_TYPE::NOT_EQUAL,
	EASY_OPERATOR_TYPE::LOWER,
	EASY_OPERATOR_TYPE::LOWER_EQUAL,
	EASY_OPERATOR_TYPE::GREATOR,
	EASY_OPERATOR_TYPE::GREATOR_EQUAL,
	EASY_OPERATOR_TYPE::OR,
	EASY_OPERATOR_TYPE::AND,
};
static std::unordered_set<EASY_OPERATOR_TYPE, EnumClassHash>::const_iterator ControlOperatorsEnd = ControlOperators.cend();

static std::unordered_map<string_type, EASY_KEYWORD_TYPE> Keywords {
	{ _T("func"), EASY_KEYWORD_TYPE::FUNC },
	{ _T("return"), EASY_KEYWORD_TYPE::RETURN },
	{ _T("if"), EASY_KEYWORD_TYPE::IF },
    { _T("var"), EASY_KEYWORD_TYPE::ASSIGNMENT },
    { _T("else"), EASY_KEYWORD_TYPE::ELSE },
    { _T("then"), EASY_KEYWORD_TYPE::THEN },
    { _T("true"), EASY_KEYWORD_TYPE::BOOL_TRUE },
    { _T("false"), EASY_KEYWORD_TYPE::BOOL_FALSE },
	{ _T("package"), EASY_KEYWORD_TYPE::PACKAGE },
	{ _T("for"), EASY_KEYWORD_TYPE::FOR },
	{ _T("in"), EASY_KEYWORD_TYPE::IN_KEYWORD },
	{ _T("to"), EASY_KEYWORD_TYPE::TO_KEYWORD },
	{ _T("step"), EASY_KEYWORD_TYPE::STEP_KEYWORD },
	{ _T("bool"), EASY_KEYWORD_TYPE::TYPE_BOOL },
	{ _T("int"), EASY_KEYWORD_TYPE::TYPE_INT },
	{ _T("double"), EASY_KEYWORD_TYPE::TYPE_DOUBLE },
	{ _T("string"), EASY_KEYWORD_TYPE::TYPE_STRING },
	{ _T("array"), EASY_KEYWORD_TYPE::TYPE_ARRAY },
	{ _T("dict"), EASY_KEYWORD_TYPE::TYPE_DICTIONARY },
};
static std::unordered_map<string_type, EASY_KEYWORD_TYPE>::const_iterator KeywordsEnd = Keywords.cend();

static std::unordered_set<EASY_KEYWORD_TYPE, EnumClassHash> Types{
	EASY_KEYWORD_TYPE::TYPE_BOOL,
	EASY_KEYWORD_TYPE::TYPE_STRING,
	EASY_KEYWORD_TYPE::TYPE_ARRAY,
	EASY_KEYWORD_TYPE::TYPE_DICTIONARY,
	EASY_KEYWORD_TYPE::TYPE_DOUBLE,
	EASY_KEYWORD_TYPE::TYPE_INT
};
static std::unordered_set<EASY_KEYWORD_TYPE, EnumClassHash>::const_iterator TypesEnd = Types.cend();


class Token
{
public:
	EASY_TOKEN_TYPE GetType() { return Type; }
    size_t Line;
    size_t Current;

protected:
	EASY_TOKEN_TYPE Type;
};

class EofToken : public Token {
public:
    int Value;
    EofToken() : Token() { Type = EASY_TOKEN_TYPE::END_OF_FILE; }
};

class IntegerToken : public Token {
public:
    int Value;
	IntegerToken() : Token() { Type = EASY_TOKEN_TYPE::INTEGER; }
	IntegerToken(int value) : Token() { Type = EASY_TOKEN_TYPE::INTEGER; Value = value; }
};

class DoubleToken : public Token {
public:
    double Value;
    DoubleToken() : Token() { Type = EASY_TOKEN_TYPE::DOUBLE; }
	DoubleToken(int value) : Token() { Type = EASY_TOKEN_TYPE::DOUBLE; Value = value; }
};

class OperatorToken : public Token {
public:
    EASY_OPERATOR_TYPE Value;
	OperatorToken() : Token() { Type = EASY_TOKEN_TYPE::OPERATOR; }
    OperatorToken(EASY_OPERATOR_TYPE value, size_t line = 0, size_t current = 0) : Token() { Type = EASY_TOKEN_TYPE::OPERATOR; Value = value; Line = line; Current = current; }
};

class SymbolToken : public Token {
public:
    string_type Value;
    SymbolToken() : Token() { Type = EASY_TOKEN_TYPE::SYMBOL; }
    SymbolToken(string_type symbol) : Token() { Type = EASY_TOKEN_TYPE::SYMBOL; Value = symbol; }
};

class TextToken : public Token {
public:
    string_type Value;
    TextToken() : Token() { Type = EASY_TOKEN_TYPE::TEXT; }
};

class VariableToken : public Token {
public:
    string_type Value;
    VariableToken() : Token() { Type = EASY_TOKEN_TYPE::VARIABLE; }
};

class KeywordToken : public Token {
public:
	EASY_KEYWORD_TYPE Value;
	KeywordToken() : Token() { Type = EASY_TOKEN_TYPE::KEYWORD; }
};

class Tokinizer
{
public:
    virtual void Parse(string_type const & data, std::shared_ptr<std::vector<Token*>> Tokens) = 0;
	virtual void Dump(std::shared_ptr <std::vector<Token *>> Tokens) = 0;
};

class StandartTokinizerImpl;
class StandartTokinizer : public Tokinizer {
public:
    StandartTokinizer();
	~StandartTokinizer();
    void Parse(string_type const &data, std::shared_ptr <std::vector<Token *>> Tokens) override;
	void Dump(std::shared_ptr <std::vector<Token *>> Tokens) override;
    bool HasError();
    string_type ErrorMessage();

private:
    StandartTokinizerImpl *impl;
};


#endif //EASYLANG_LEXER_H
