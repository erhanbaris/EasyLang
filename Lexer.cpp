#include "Lexer.h"

class StandartTokinizerImpl {
public:
    size_t line{};
    size_t column{};
    size_t index{};

    size_t contentLength{};
    std::wstring content;
    std::shared_ptr<std::vector<Token*>> TokenList;
    bool hasError{};
    std::wstring errorMessage;

    void startParse()
    {
        while (contentLength > index)
        {
            wchar_t ch = getChar();

            if (isWhitespace(ch))
            {
                ++index;
                continue;
            }
            else if (isSymbol(ch))
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
            else
            {
                getOperator();
                continue;
            }
        }
    }

    bool isWhitespace(wchar_t ch)
    {
        return (ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t');
    }

    void getSymbol()
    {
        std::wstringstream stream;

        wchar_t ch;

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

		auto data = stream.str();
		if (Operators.find(data) != OperatorsEnd)
		{
			auto *token = new OperatorToken;
			token->Value = Operators.find(data)->second;
			TokenList->push_back(reinterpret_cast<Token*>(token));
		}
		else if (Keywords.find(data) != KeywordsEnd)
		{
			auto *token = new KeywordToken;
			token->Value = Keywords.find(data)->second;
			TokenList->push_back(reinterpret_cast<Token*>(token));
		}
		else
		{
			auto *token = new SymbolToken;
			token->Value = stream.str();
			TokenList->push_back(reinterpret_cast<Token*>(token));
		}
    }

    void getVariable()
    {
        std::wstringstream stream;
        ++index;
        wchar_t ch;

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

        auto *token = new VariableToken;
        token->Value = stream.str();
        TokenList->push_back(reinterpret_cast<Token*>(token));
    }

    void getText()
    {
        std::wstringstream stream;

        ++index;
        wchar_t ch = getChar();
        wchar_t chNext;

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

        auto *token = new TextToken;
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
            auto* opt = new OperatorToken;
            switch (ch)
            {
                case '-':
                    opt->Value = EASY_OPERATOR_TYPE::MINUS;
                    break;

                case '+':
                    opt->Value = EASY_OPERATOR_TYPE::PLUS;
                    break;

                case '*':
                    opt->Value = EASY_OPERATOR_TYPE::MULTIPLICATION;
                    break;

                case '/':
                    opt->Value = EASY_OPERATOR_TYPE::DIVISION;
                    break;

                case '=':
                    opt->Value = EASY_OPERATOR_TYPE::EQUAL;
                    break;

                case '>':
                    if (chNext == '=')
                    {
                        opt->Value = EASY_OPERATOR_TYPE::GREATOR_EQUAL;
                        ++index;
                    }
                    else
                        opt->Value = EASY_OPERATOR_TYPE::GREATOR;
                    break;

                case '<':
                    if (chNext == '=')
                    {
                        opt->Value = EASY_OPERATOR_TYPE::LOWER_EQUAL;
                        ++index;
                    }
                    else
                        opt->Value = EASY_OPERATOR_TYPE::LOWER;
                    break;

                case '\'':
                    opt->Value = EASY_OPERATOR_TYPE::SINGLE_QUOTES;
                    break;

                case '"':
                    opt->Value = EASY_OPERATOR_TYPE::DOUBLE_QUOTES;
                    break;

                case '(':
                    opt->Value = EASY_OPERATOR_TYPE::LEFT_PARENTHESES;
                    break;

                case ')':
                    opt->Value = EASY_OPERATOR_TYPE::RIGHT_PARENTHESES;
                    break;

                default:
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
            auto* token = new DoubleToken;
            token->Value = beforeTheComma + (afterTheComma * pow(10, -1 * dotPlace));
            token->Value *= isMinus ? -1 : 1;
            TokenList->push_back(reinterpret_cast<Token*>(token));
        }
        else {
            auto* token = new IntegerToken;
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

    wchar_t getNextChar()
    {
        if (contentLength > (index + 1))
            return content[index + 1];

        return '\0';
    }
};

StandartTokinizer::StandartTokinizer()
{
    impl = new StandartTokinizerImpl;
}

void StandartTokinizer::Parse(std::wstring const & data, std::shared_ptr<std::vector<Token*>>Tokens)
{
    impl->content = data;
    impl->contentLength = impl->content.length();
    impl->TokenList = Tokens;
    impl->index = 0;
    impl->TokenList->clear();

    impl->startParse();
}

void StandartTokinizer::Dump(std::shared_ptr <std::vector<Token *>> Tokens)
{
	auto TokenEnd = Tokens->cend();

	for (auto it = Tokens->cbegin(); it != TokenEnd; ++it)
	{
		switch ((*it)->GetType())
		{
		case EASY_TOKEN_TYPE::DOUBLE:
			std::wcout << L"DOUBLE : " << reinterpret_cast<DoubleToken*>(*it)->Value << std::endl;
			break;

		case EASY_TOKEN_TYPE::INTEGER:
			std::wcout << L"INTEGER : " << reinterpret_cast<IntegerToken*>(*it)->Value << std::endl;
			break;

		case EASY_TOKEN_TYPE::OPERATOR:
			std::wcout << L"OPERATOR : " << EASY_OPERATOR_TYPEToString (reinterpret_cast<OperatorToken*>(*it)->Value) << std::endl;
			break;

		case EASY_TOKEN_TYPE::SYMBOL:
			std::wcout << L"SYMBOL : " << reinterpret_cast<SymbolToken*>(*it)->Value << std::endl;
			break;

		case EASY_TOKEN_TYPE::TEXT:
			std::wcout << L"TEXT : " << reinterpret_cast<TextToken*>(*it)->Value << std::endl;
			break;

		case EASY_TOKEN_TYPE::VARIABLE:
			std::wcout << L"VARIABLE : " << reinterpret_cast<VariableToken*>(*it)->Value << std::endl;
			break;

		case EASY_TOKEN_TYPE::KEYWORD:
			std::wcout << L"KEYWORD : " << EASY_KEYWORD_TYPEToString(reinterpret_cast<KeywordToken*>(*it)->Value) << std::endl;
			break;

		case EASY_TOKEN_TYPE::TOKEN_NONE:
			break;
		}
	}
}

bool StandartTokinizer::HasError()
{
    return impl->hasError;
}

std::wstring StandartTokinizer::ErrorMessage()
{
    return impl->errorMessage;
}
