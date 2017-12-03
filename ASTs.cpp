#include "ASTs.h"

class AstParserImpl
{
public:
    std::shared_ptr<std::vector<Token*>> tokens;
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

    void parse()
    {
        tokensCount = tokens->size();
        index = 0;
        std::vector<Ast*> astList;

        while(index < tokensCount)
        {
            auto* token = getToken();
            auto* tokenNext = getNextToken();

            if (token->GetType() == TokenType::SYMBOL &&
                    tokenNext->GetType() == TokenType::OPERATOR &&
                    reinterpret_cast<OperatorToken*>(tokenNext)->Value == OperatorType::SINGLE_QUOTES)
            {
                // erhan'a 15 ata
                auto* ast = new AssignmentAst;
                ast->Name = reinterpret_cast<SymbolToken*>(token)->Value;

                token = getSeekToken(3);
                tokenNext = getSeekToken(4);

                switch(token->GetType())
                {
                    case TokenType::INTEGER:
                        ast->Data = new IntegerAst(reinterpret_cast<IntegerToken*>(token)->Value);
                        break;

                    case TokenType::DOUBLE:
                        ast->Data = new DoubleAst(reinterpret_cast<DoubleToken*>(token)->Value);
                        break;

                    case TokenType::TEXT:
                        ast->Data = new TextAst(reinterpret_cast<TextToken*>(token)->Value);
                        break;
                }

                index += 4;
                astList.push_back(reinterpret_cast<Ast*>(ast));
            }

            ++index;
        }
    }
};

AstParser::AstParser()
{
    impl = new AstParserImpl;
}

void AstParser::Parse(std::shared_ptr<std::vector<Token *>> Tokens) {
    impl->tokens = Tokens;

    impl->parse();
}