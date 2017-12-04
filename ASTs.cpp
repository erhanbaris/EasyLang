#include "ASTs.h"

class AstParserImpl
{
public:
    std::shared_ptr<std::vector<Token*>> tokens;
	std::shared_ptr<std::vector<Ast*>> asts;

    size_t tokensCount;
    size_t index;

    Token* getToken()
    {
        if (index < tokensCount)
            return tokens->at(index);

        return nullptr;
    }

    Token* getSeekToken(size_t seek)
    {
        if (index + seek < tokensCount)
            return tokens->at(index + seek);

        return nullptr;
    }


    Token* getNextToken()
    {
        return getSeekToken(1);
    }

    AstType detectType()
    {
        auto* token = getToken();
        auto* tokenNext = getNextToken();

        if (token->GetType() == EASY_TOKEN_TYPE::KEYWORD && reinterpret_cast<KeywordToken*>(token)->Value == EASY_KEYWORD_TYPE::ASSIGNMENT)
            return AstType::ASSIGNMENT;

        if (token->GetType() == EASY_TOKEN_TYPE::KEYWORD && reinterpret_cast<KeywordToken*>(token)->Value == EASY_KEYWORD_TYPE::IF)
            return AstType::IF_STATEMENT;

        return AstType ::NONE;
    }

	inline bool isPrimative()
	{
		return isPrimative(getToken());
	}

	inline bool isPrimative(Token * token)
	{
		return (token->GetType() == EASY_TOKEN_TYPE::INTEGER ||
			token->GetType() == EASY_TOKEN_TYPE::TEXT ||
			token->GetType() == EASY_TOKEN_TYPE::DOUBLE);
	}

	inline Ast* getPrimative()
	{
		return getPrimative(getToken());
	}

	Ast* getPrimative(Token* token)
	{
		PrimativeAst* ast = nullptr;

		switch (token->GetType())
		{
		case EASY_TOKEN_TYPE::INTEGER:
			ast = new PrimativeAst(reinterpret_cast<IntegerToken*>(token)->Value);
			break;

		case EASY_TOKEN_TYPE::DOUBLE:
			ast = new PrimativeAst(reinterpret_cast<DoubleToken*>(token)->Value);
			break;

		case EASY_TOKEN_TYPE::TEXT:
			ast = new PrimativeAst(reinterpret_cast<TextToken*>(token)->Value);
			break;
		}

		return reinterpret_cast<Ast*>(ast);
	}

    Ast* parseAssignment()
    {
        ++index;
        auto* token = getToken();
        auto* tokenNext = getNextToken();

        auto* ast = new AssignmentAst;
        ast->Name = reinterpret_cast<SymbolToken*>(token)->Value;

		if (tokenNext->GetType() == EASY_TOKEN_TYPE::OPERATOR)
		{
			if (reinterpret_cast<OperatorToken*>(tokenNext)->Value == EASY_OPERATOR_TYPE::SINGLE_QUOTES)
				index += 3;
			else
				index += 1;
		}
		else if (isPrimative(tokenNext))
			++index;

        token = getToken();
        tokenNext = getNextToken();

		ast->Data = getPrimative(token);
              
        return reinterpret_cast<Ast*>(ast);
    }

    Ast* parseIfStatement()
    {
        auto* ast = new IfStatementAst;
        ast->BinartOpt = parseBinaryOperationStatement();
        return reinterpret_cast<Ast*>(ast);
    }

    Ast* parseBinaryOperationStatement()
    {
		++index;
        auto* ast = new BinaryAst;
		auto* token = getToken();

		if (isPrimative(token))
			ast->Left = getPrimative(token);
		else if (token->GetType() == EASY_TOKEN_TYPE::SYMBOL)
			ast->Left = new VariableAst(reinterpret_cast<SymbolToken*>(token)->Value);

		++index;
		token = getToken();

		if (token->GetType() == EASY_TOKEN_TYPE::OPERATOR &&
			BinaryOperators.find(reinterpret_cast<OperatorToken*>(token)->Value) != BinaryOperatorsEnd)
			ast->Op = reinterpret_cast<OperatorToken*>(token)->Value;
		else
			ast->Op == EASY_OPERATOR_TYPE::OPERATOR_NONE;

		++index;
		token = getToken();
		if (isPrimative(token))
			ast->Right = getPrimative(token);
		else if (token->GetType() == EASY_TOKEN_TYPE::SYMBOL)
			ast->Right = new VariableAst(reinterpret_cast<SymbolToken*>(token)->Value);

        return reinterpret_cast<Ast*>(ast);
    }

    void parse()
    {
        tokensCount = tokens->size();
        index = 0;

        while(index < tokensCount)
        {
            auto* token = getToken();
            auto* tokenNext = getNextToken();

            if (detectType() == AstType::ASSIGNMENT)
                asts->push_back(parseAssignment());
            else if (detectType() == AstType::IF_STATEMENT)
				asts->push_back(parseIfStatement());

            ++index;
        }
    }
};

AstParser::AstParser()
{
    impl = new AstParserImpl;
}

void AstParser::Parse(std::shared_ptr<std::vector<Token*>> tokens, std::shared_ptr<std::vector<Ast*>> asts)
{
	asts->clear();

	impl->tokens = tokens;
	impl->asts = asts;
	impl->parse();
}
