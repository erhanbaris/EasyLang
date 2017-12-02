//https://github.com/catchorg/Catch2
//https://github.com/DeadMG/Wide/blob/master/Wide/Lexer/Lexer.cpp

#include <string>
#include <vector>
#include <memory>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <cmath>
#define CATCH_CONFIG_RUNNER

#include "Catch.h"

using namespace std;


/* Token */

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
    DOLLAR
};

class Token
{
public:
    TokenType GetType()
    {
        return Type;
    }

protected:
    TokenType Type;
};

class IntegerToken : Token {
public:
    int Value;
    IntegerToken()
    {
        Type = TokenType::INTEGER;
    }
};

class DoubleToken : Token {
public:
    double Value;
    DoubleToken()
    {
        Type = TokenType::DOUBLE;
    }
};

class OperatorToken : Token {
public:
    OperatorType Value;
    OperatorToken()
    {
        Type = TokenType::OPERATOR;
    }
};

class SymbolToken : Token {
public:
    std::wstring Value;
    SymbolToken()
    {
        Type = TokenType::SYMBOL;
    }
};

class TextToken : Token {
public:
    std::wstring Value;
    TextToken()
    {
        Type = TokenType::TEXT;
    }
};

class VariableToken : Token {
public:
    std::wstring Value;
    VariableToken()
    {
        Type = TokenType::VARIABLE;
    }
};

/* Token */

class Tokinizer
{
public:
    virtual void Parse(std::wstring const & data, std::shared_ptr<std::vector<Token*>> Tokens) = 0;
};

class StandartTokinizer : public Tokinizer
{
public:
    void Parse(std::wstring const & data, std::shared_ptr<std::vector<Token*>>Tokens)
    {
        content = std::move(data);
        contentLength = content.length();
        TokenList = Tokens;
        index = 0;
        TokenList.get()->clear();

        startParse();
    }

private:
    size_t line;
    size_t column;
    size_t index;

    size_t contentLength;
    std::wstring content;
    std::shared_ptr<std::vector<Token*>> TokenList;
    bool hasError;
    std::wstring errorMessage;

    void startParse()
    {
        while (contentLength > index)
        {
            wchar_t ch = getChar();

            if (isSymbol(ch))
            {
                getSymbol();
                continue;
            }
            else if (ch == '"')
            {
                getText();
                continue;
            }
            else if (ch == '$')
            {
                getVariable();
                continue;
            }
            else if ((ch >= '0' && ch <= '9') || ch == '.')
            {
                getNumber();
                continue;
            }
            else if (isWhitespace(ch))
            {
                ++index;
                continue;
            }
            else
            {
                getOperator();
                continue;
            }

            ++index;
        }
    }

    bool isWhitespace(wchar_t ch)
    {
        return (ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t');
    }

    void getSymbol()
    {
        std::wstringstream stream;

        wchar_t ch = getChar();

        while (contentLength > index)
        {
            ch = getChar();

            if (isWhitespace(ch) || ch == '\'' || ch == '"')
            {
                break;
            }

            stream << ch;
            ++index;
        }

        SymbolToken *token = new SymbolToken;
        token->Value = stream.str();
        TokenList->push_back(reinterpret_cast<Token*>(token));
    }

    void getVariable()
    {
        std::wstringstream stream;
        ++index;
        wchar_t ch = getChar();

        while (contentLength > index)
        {
            ch = getChar();

            if (isWhitespace(ch) || ch == '\'' || ch == '"')
            {
                ++index;
                break;
            }

            stream << ch;
            ++index;
        }

        VariableToken *token = new VariableToken;
        token->Value = stream.str();
        TokenList->push_back(reinterpret_cast<Token*>(token));
    }

    void getText()
    {
        std::wstringstream stream;

        ++index;
        wchar_t ch = getChar();
        wchar_t chNext = getNextChar();

        while (contentLength > index && ch != '"')
        {
            ch = getChar();
            chNext = getNextChar();

            if (ch == '$')
                getSymbol();
            else if (ch == '\\' && chNext == '"')
            {
                stream << '"';
                ++index;
            }
            else if (ch == '"')
            {
                ++index;
                break;
            }
            else
                stream << ch;

            ++index;
        }

        TextToken *token = new TextToken;
        token->Value = stream.str();
        TokenList->push_back(reinterpret_cast<Token*>(token));
    }

    inline bool isInteger(wchar_t ch)
    {
        return (ch >= '0' && ch <= '9');
    }

    inline bool isSymbol(wchar_t ch)
    {
        return ((ch >= 'a' && ch <= 'z') ||
                (ch >= 'A' && ch <= 'Z') ||
                ch == L'Ğ' ||
                ch == L'ğ' ||
                ch == L'Ü' ||
                ch == L'ü' ||
                ch == L'ı' ||
                ch == L'İ' ||
                ch == L'Ö' ||
                ch == L'ö' ||
                ch == L'Ç' ||
                ch == L'ç');
    }

    void getOperator()
    {
        wchar_t ch = getChar();

        if (ch == '-' && (isInteger(getNextChar()) || getNextChar() == '.'))
            getNumber();
        else
        {
            auto chNext = getNextChar();
            OperatorToken* opt = new OperatorToken;
            switch (ch)
            {
                case '-':
                    opt->Value = OperatorType::MINUS;
                    break;

                case '+':
                    opt->Value = OperatorType::PLUS;
                    break;

                case '*':
                    opt->Value = OperatorType::MULTIPLICATION;
                    break;

                case '/':
                    opt->Value = OperatorType::DIVISION;
                    break;

                case '=':
                    opt->Value = OperatorType::EQUAL;
                    break;

                case '>':
                    if (chNext == '=')
                    {
                        opt->Value = OperatorType::GREATOR_EQUAL;
                        ++index;
                    }
                    else
                        opt->Value = OperatorType::GREATOR;
                    break;

                case '<':
                    if (chNext == '=')
                    {
                        opt->Value = OperatorType::LOWER_EQUAL;
                        ++index;
                    }
                    else
                        opt->Value = OperatorType::LOWER;
                    break;

                case '\'':
                    opt->Value = OperatorType::SINGLE_QUOTES;
                    break;

                case '"':
                    opt->Value = OperatorType::DOUBLE_QUOTES;
                    break;

                case '$':
                    opt->Value = OperatorType::DOLLAR;
                    break;
            }

            ++index;
            TokenList->push_back(reinterpret_cast<Token*>(opt));
        }
    }

    void getNumber()
    {
        bool isMinus = false;
        int dotPlace = 0;
        int beforeTheComma = 0;
        int afterTheComma = 0;

        bool isDouble = false;
        wchar_t ch = getChar();
        while (contentLength > index)
        {
            if (ch == '-')
            {
                if (isMinus || (beforeTheComma > 0 || afterTheComma > 0))
                    break;

                isMinus = true;
            }
            else if (ch == '.')
            {
                if (isDouble)
                {
                    error(L"Number problem");
                    break;
                }

                isDouble = true;
            }
            else if ((ch >= '0' && ch <= '9'))
            {
                if (isDouble)
                {
                    ++dotPlace;

                    afterTheComma *= pow(10, 1);
                    afterTheComma += ch - '0';
                }
                else {
                    beforeTheComma *= pow(10, 1);
                    beforeTheComma += ch - '0';
                }
            }
            else
                break;

            ++index;
            ch = getChar();
        }

        if (isDouble)
        {
            DoubleToken* token = new DoubleToken;
            token->Value = beforeTheComma + (afterTheComma * pow(10, -1 * dotPlace));
            token->Value *= isMinus ? -1 : 1;
            TokenList->push_back(reinterpret_cast<Token*>(token));
        }
        else {
            IntegerToken* token = new IntegerToken;
            token->Value = beforeTheComma;
            token->Value *= isMinus ? -1 : 1;
            TokenList->push_back(reinterpret_cast<Token*>(token));
        }
    }

    void error(std::wstring && message)
    {
        hasError = true;
        errorMessage = message;
    }

    wchar_t getChar()
    {
        if (contentLength > index)
            return content[index];

        return '\0';
    }
    //erhan'a 5 ata

    wchar_t getNextChar()
    {
        if (contentLength > (index + 1))
            return content[index + 1];

        return '\0';
    }
};

/* AST */

class Ast
{
public:

};

class AssignmentAst : Ast {
public:
};


/* AST */

TEST_CASE( "Double test" ) {
Tokinizer* parser = new StandartTokinizer();
auto tokens = make_shared<std::vector<Token*>>();
parser->Parse(L".1", tokens);
REQUIRE(tokens.get()->at(0)->GetType() == TokenType::DOUBLE);
REQUIRE(reinterpret_cast<DoubleToken*>(tokens.get()->at(0))->Value == 0.1);

parser->Parse(L"1.1", tokens);
REQUIRE(tokens.get()->at(0)->GetType() == TokenType::DOUBLE);
REQUIRE(reinterpret_cast<DoubleToken*>(tokens.get()->at(0))->Value == 1.1);

parser->Parse(L"1990.0001", tokens);
REQUIRE(tokens.get()->at(0)->GetType() == TokenType::DOUBLE);
REQUIRE(reinterpret_cast<DoubleToken*>(tokens.get()->at(0))->Value == 1990.0001);

parser->Parse(L"-10.1", tokens);
REQUIRE(tokens.get()->at(0)->GetType() == TokenType::DOUBLE);
REQUIRE(reinterpret_cast<DoubleToken*>(tokens.get()->at(0))->Value == -10.1);

parser->Parse(L"-.1", tokens);
REQUIRE(tokens.get()->at(0)->GetType() == TokenType::DOUBLE);
REQUIRE(reinterpret_cast<DoubleToken*>(tokens.get()->at(0))->Value == -0.1);
}

TEST_CASE( "Integer test" ) {
Tokinizer* parser = new StandartTokinizer();
auto tokens = make_shared<std::vector<Token*>>();
parser->Parse(L"1", tokens);
REQUIRE(tokens.get()->at(0)->GetType() == TokenType::INTEGER);
REQUIRE(reinterpret_cast<IntegerToken*>(tokens.get()->at(0))->Value == 1);

parser->Parse(L"12345", tokens);
REQUIRE(tokens.get()->at(0)->GetType() == TokenType::INTEGER);
REQUIRE(reinterpret_cast<IntegerToken*>(tokens.get()->at(0))->Value == 12345);

parser->Parse(L"-12345", tokens);
REQUIRE(tokens.get()->at(0)->GetType() == TokenType::INTEGER);
REQUIRE(reinterpret_cast<IntegerToken*>(tokens.get()->at(0))->Value == -12345);
}

TEST_CASE( "String test" ) {
Tokinizer* parser = new StandartTokinizer();
auto tokens = make_shared<std::vector<Token*>>();
parser->Parse(L"\"erhan\"", tokens);
REQUIRE(tokens.get()->at(0)->GetType() == TokenType::TEXT);
REQUIRE(reinterpret_cast<TextToken*>(tokens.get()->at(0))->Value == L"erhan");

parser->Parse(L"\"'erhan'\"", tokens);
REQUIRE(tokens.get()->at(0)->GetType() == TokenType::TEXT);
REQUIRE(reinterpret_cast<TextToken*>(tokens.get()->at(0))->Value == L"'erhan'");

parser->Parse(L"\"\\\"erhan\\\"\"", tokens);
REQUIRE(tokens.get()->at(0)->GetType() == TokenType::TEXT);
REQUIRE(reinterpret_cast<TextToken*>(tokens.get()->at(0))->Value == L"\"erhan\"");

SECTION( "Atama" ) {
parser->Parse(L"erhan'a 15", tokens);
REQUIRE(tokens.get()->size() == 4);
REQUIRE(tokens.get()->at(0)->GetType() == TokenType::SYMBOL);
REQUIRE(reinterpret_cast<TextToken*>(tokens.get()->at(0))->Value == L"erhan");

REQUIRE(tokens.get()->at(1)->GetType() == TokenType::OPERATOR);
REQUIRE(reinterpret_cast<OperatorToken*>(tokens.get()->at(1))->Value == OperatorType::SINGLE_QUOTES);

REQUIRE(tokens.get()->at(2)->GetType() == TokenType::SYMBOL);
REQUIRE(reinterpret_cast<TextToken*>(tokens.get()->at(2))->Value == L"a");

REQUIRE(tokens.get()->at(3)->GetType() == TokenType::INTEGER);
REQUIRE(reinterpret_cast<IntegerToken*>(tokens.get()->at(3))->Value == 15);
}

SECTION( "Sembol" ) {
parser->Parse(L"17 <= 40", tokens);
REQUIRE(tokens.get()->size() == 3);
REQUIRE(tokens.get()->at(0)->GetType() == TokenType::INTEGER);
REQUIRE(reinterpret_cast<IntegerToken*>(tokens.get()->at(0))->Value == 17);

REQUIRE(tokens.get()->at(1)->GetType() == TokenType::OPERATOR);
REQUIRE(reinterpret_cast<OperatorToken*>(tokens.get()->at(1))->Value == OperatorType::LOWER_EQUAL);

REQUIRE(tokens.get()->at(2)->GetType() == TokenType::INTEGER);
REQUIRE(reinterpret_cast<IntegerToken*>(tokens.get()->at(2))->Value == 40);
}

SECTION( "17 büyüktür 40" ) {
parser->Parse(L"17 büyüktür 40", tokens);
REQUIRE(tokens.get()->size() == 3);
REQUIRE(tokens.get()->at(0)->GetType() == TokenType::INTEGER);
REQUIRE(reinterpret_cast<IntegerToken*>(tokens.get()->at(0))->Value == 17);

REQUIRE(tokens.get()->at(1)->GetType() == TokenType::SYMBOL);
REQUIRE(reinterpret_cast<SymbolToken*>(tokens.get()->at(1))->Value == L"büyüktür");

REQUIRE(tokens.get()->at(2)->GetType() == TokenType::INTEGER);
REQUIRE(reinterpret_cast<IntegerToken*>(tokens.get()->at(2))->Value == 40);
}
}


int main( int argc, char* argv[] ) {
    int result = Catch::Session().run( argc, argv );

    /* eğer 127 > 80 sonra yaz büyük değilse yaz küçük */
    Tokinizer* parser = new StandartTokinizer();
    auto Tokens = make_shared<std::vector<Token*>>();
    parser->Parse(L"n'a 15 ata eger erhan buyuk 13 sonra yaz \"buyuk\" degilse yaz \"kucuk\"", Tokens);

    auto TokenEnd = Tokens->cend();

    for (auto it = Tokens->cbegin(); it != TokenEnd; ++it)
    {
        switch ((*it)->GetType())
        {
            case TokenType::DOUBLE:
                std::wcout << L"DOUBLE : " << reinterpret_cast<DoubleToken*>(*it)->Value << std::endl;
                break;

            case TokenType::INTEGER:
                std::wcout << L"INTEGER : " << reinterpret_cast<IntegerToken*>(*it)->Value << std::endl;
                break;

            case TokenType::OPERATOR:
                std::wcout << L"OPERATOR : " << (int)(reinterpret_cast<OperatorToken*>(*it)->Value) << std::endl;
                break;

            case TokenType::SYMBOL:
                std::wcout << L"SYMBOL : " << reinterpret_cast<SymbolToken*>(*it)->Value << std::endl;
                break;

            case TokenType::TEXT:
                std::wcout << L"TEXT : " << reinterpret_cast<TextToken*>(*it)->Value << std::endl;
                break;

            case TokenType::VARIABLE:
                std::wcout << L"VARIABLE : " << reinterpret_cast<VariableToken*>(*it)->Value << std::endl;
                break;
        }
    }

    getchar();
    return 0;
}