#include "Lexer.h"
#include "Exceptions.h"

class StandartTokinizerImpl {
public:
    size_t line{};
    size_t column{};
    size_t index{};

    size_t contentLength{};
    string_type content;
    std::shared_ptr<std::vector<Token*>> TokenList;
    bool hasError{};
    string_type errorMessage;

    void startParse()
    {
        while (contentLength > index)
        {
            char_type ch = getChar();
            char_type chNext = getNextChar();

            if (isWhitespace(ch))
            {
                while (contentLength > index && isWhitespace(ch))
                {
                    ++index;
                    ch = getChar();
                }
                
                continue;
            }
			else if (ch == '_' && (chNext == '\0' || (isSymbol(ch) == false && ch >= '0' && ch <= '9')))
			{
				auto* opt = new OperatorToken;
				opt->Value = EASY_OPERATOR_TYPE ::UNDERLINE;
				TokenList->push_back(reinterpret_cast<Token*>(opt));
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

    bool isWhitespace(char_type ch)
    {
        return (ch == ' ' || ch == '\r' || ch == '\n' || ch == '\t');
    }

    void getSymbol()
    {
        string_stream stream;

        char_type ch;

        while (contentLength > index)
        {
            ch = getChar();

            if (!isSymbol(ch) && !isInteger(ch) && ch != '_')
                break;
            
            if (isWhitespace(ch) || ch == '\'' || ch == '"')
                break;

            stream << ch;
            ++index;
        }

		auto data = stream.str();
		if (Keywords.find(data) != KeywordsEnd)
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
        string_stream stream;
        ++index;
        char_type ch;

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
        string_stream stream;

        ++index;
        char_type ch = getChar();
        char_type chNext;

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

		if (ch != '"')
			throw ParseError(_T("Text has ends with '\"'"));

        auto *token = new TextToken;
        token->Value = stream.str();
        TokenList->push_back(reinterpret_cast<Token*>(token));
    }

    inline bool isInteger(char_type ch)
    {
        return (ch >= '0' && ch <= '9');
    }

    inline bool isSymbol(char_type ch)
    {
        return ((ch >= 'a' && ch <= 'z') ||
                (ch >= 'A' && ch <= 'Z'));
    }

    void getOperator()
    {
        char_type ch = getChar();
        char_type chNext = getNextChar();

        if (ch == '-' && chNext == '>')
        {
            index += 2;
            
            auto* opt = new OperatorToken;
            opt->Value = EASY_OPERATOR_TYPE::OPERATION;
            TokenList->push_back(reinterpret_cast<Token*>(opt));
        }
        else if (ch == '-' && (isInteger(getNextChar()) || getNextChar() == '.'))
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
					if (chNext == '=')
					{
						opt->Value = EASY_OPERATOR_TYPE::EQUAL;
						++index;
					}
					else 
						opt->Value = EASY_OPERATOR_TYPE::ASSIGN;
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
                    else if (chNext == '+')
                    {
                        opt->Value = EASY_OPERATOR_TYPE::APPEND;
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

                case '{':
                    opt->Value = EASY_OPERATOR_TYPE::BLOCK_START;
                    break;

                case '}':
                    opt->Value = EASY_OPERATOR_TYPE::BLOCK_END;
                    break;

                case '[':
                    opt->Value = EASY_OPERATOR_TYPE::SQUARE_BRACKET_START;
                    break;

				case ']':
					opt->Value = EASY_OPERATOR_TYPE::SQUARE_BRACKET_END;
					break;   
				
				case '!':
						opt->Value = EASY_OPERATOR_TYPE::INDEXER;
						break;

				case ',':
					opt->Value = EASY_OPERATOR_TYPE::COMMA;
					break;

				case '&':
					if (chNext == '&')
					{
						opt->Value = EASY_OPERATOR_TYPE::AND;
						++index;
					}
					else
						throw ParseError(_T("Unknown char '&'"));
					break;

                case '|':
                    if (chNext == '|')
                    {
                        opt->Value = EASY_OPERATOR_TYPE::OR;
                        ++index;
                    }
                    else
                        throw ParseError(_T("Unknown char '|'"));
                    break;

                case ':':
                    if (chNext == ':')
                    {
                        opt->Value = EASY_OPERATOR_TYPE::DOUBLE_COLON;
                        ++index;
                    }
                    else
                        opt->Value = EASY_OPERATOR_TYPE::SINGLE_COLON;
                    break;


                default:
					throw ParseError(_T("Unknown char"));
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
        char_type ch = getChar();
        char_type chNext = getNextChar();
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
                if (chNext == '.')
                    break;
                
                if (isDouble)
                {
                    error(_T("Number problem"));
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
            chNext = getNextChar();
        }

        if (isDouble)
        {
            auto* token = new DoubleToken;
            token->Value = beforeTheComma + (afterTheComma * pow(10, -1 * dotPlace));

            if (isMinus)
                TokenList->push_back(reinterpret_cast<Token*>(new OperatorToken(EASY_OPERATOR_TYPE::MINUS)));

            TokenList->push_back(reinterpret_cast<Token*>(token));
        }
        else {
            auto* token = new IntegerToken;
            token->Value = beforeTheComma;

            if (isMinus)
                TokenList->push_back(reinterpret_cast<Token*>(new OperatorToken(EASY_OPERATOR_TYPE::MINUS)));

            TokenList->push_back(reinterpret_cast<Token*>(token));
        }
    }

    void error(string_type && message)
    {
        hasError = true;
        errorMessage = message;
    }

	char_type getChar()
    {
        if (contentLength > index)
            return content[index];

        return '\0';
    }

    char_type getNextChar()
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

void StandartTokinizer::Parse(string_type const & data, std::shared_ptr<std::vector<Token*>>Tokens)
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
			console_out << _T("DOUBLE : ") << reinterpret_cast<DoubleToken*>(*it)->Value << std::endl;
			break;

		case EASY_TOKEN_TYPE::INTEGER:
			console_out << _T("INTEGER : ") << reinterpret_cast<IntegerToken*>(*it)->Value << std::endl;
			break;

		case EASY_TOKEN_TYPE::OPERATOR:
			console_out << _T("OPERATOR : ") << EASY_OPERATOR_TYPEToString (reinterpret_cast<OperatorToken*>(*it)->Value) << std::endl;
			break;

		case EASY_TOKEN_TYPE::SYMBOL:
			console_out << _T("SYMBOL : ") << reinterpret_cast<SymbolToken*>(*it)->Value << std::endl;
			break;

		case EASY_TOKEN_TYPE::TEXT:
			console_out << _T("TEXT : ") << reinterpret_cast<TextToken*>(*it)->Value << std::endl;
			break;

		case EASY_TOKEN_TYPE::VARIABLE:
			console_out << _T("VARIABLE : ") << reinterpret_cast<VariableToken*>(*it)->Value << std::endl;
			break;

		case EASY_TOKEN_TYPE::KEYWORD:
			console_out << _T("KEYWORD : ") << EASY_KEYWORD_TYPEToString(reinterpret_cast<KeywordToken*>(*it)->Value) << std::endl;
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

string_type StandartTokinizer::ErrorMessage()
{
    return impl->errorMessage;
}
