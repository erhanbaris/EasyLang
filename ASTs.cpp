#include "ASTs.h"

enum class AstType {
    NONE,
    IF_STATEMENT,
    ASSIGNMENT,
    BINARY_OPERATION
};

class AstParserImpl
{
public:
    std::unordered_map<std::wstring, TokenType> reservedWords { {L"atama", TokenType:: }, L"eğer", L"büyükise"};
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

    AstType detectType()
    {
        auto* token = getToken();
        auto* tokenNext = getNextToken();

        if (token->GetType() == TokenType::SYMBOL && reinterpret_cast<SymbolToken*>(token)->Value == L"atama")
            return AstType::ASSIGNMENT;

        if (token->GetType() == TokenType::SYMBOL && reinterpret_cast<SymbolToken*>(token)->Value == L"eğer")
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

        if (tokenNext->GetType() == TokenType::OPERATOR)
        {
            if (reinterpret_cast<OperatorToken*>(tokenNext)->Value == OperatorType::SINGLE_QUOTES)
                index += 3;
            else
                index += 1;
        }

        token = getToken();
        tokenNext = getNextToken();

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
        std::vector<Ast*> astList;

        while(index < tokensCount)
        {
            auto* token = getToken();
            auto* tokenNext = getNextToken();

            if (detectType() == AstType::ASSIGNMENT)
                astList.push_back(parseAssignment());
            else if (detectType() == AstType::IF_STATEMENT)
                astList.push_back(parseIfStatement());

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
