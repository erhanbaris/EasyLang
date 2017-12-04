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

    // atama erhan'a 15
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

        token = getToken();
        tokenNext = getNextToken();

        switch(token->GetType())
        {
            case EASY_TOKEN_TYPE::INTEGER:
                ast->Data = new PrimativeAst(reinterpret_cast<IntegerToken*>(token)->Value);
                break;

            case EASY_TOKEN_TYPE::DOUBLE:
				ast->Data = new PrimativeAst(reinterpret_cast<DoubleToken*>(token)->Value);
                break;

            case EASY_TOKEN_TYPE::TEXT:
				ast->Data = new PrimativeAst(reinterpret_cast<TextToken*>(token)->Value);
                break;
        }
		

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
        auto* ast = new BinaryAst;


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
	impl->tokens = tokens;
	impl->asts = asts;
	impl->parse();
}
