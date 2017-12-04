#include "ASTs.h"

class AstParserImpl
{
public:
    std::shared_ptr<std::vector<Token*>> tokens;
	std::shared_ptr<std::vector<Ast*>> asts;
	std::unordered_map<std::wstring, MethodCallback> systemMethods;
    //std::unordered_map<std::wstring, MethodCallback>::iterator systemMethodsEnd;

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

		if (token->GetType() == EASY_TOKEN_TYPE::SYMBOL && systemMethods.find(reinterpret_cast<SymbolToken*>(token)->Value) != systemMethods.end())
			return AstType::FUNCTION_CALL;

		if (isPrimative(token))
			return AstType::PRIMATIVE;

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

	inline Ast* parsePrimative()
	{
		return parsePrimative(getToken());
	}

	Ast* parsePrimative(Token* token)
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

		ast->Data = parsePrimative(token);
              
        return reinterpret_cast<Ast*>(ast);
    }

    Ast* parseIfStatement()
    {
        auto* ast = new IfStatementAst;
        ast->BinaryOpt = parseBinaryOperationStatement();
		index += 2;

        auto* token = getToken();
		ast->True = parseAst();
		++index;

		token = getToken();

		if (token->GetType() == EASY_TOKEN_TYPE::KEYWORD && reinterpret_cast<KeywordToken*>(token)->Value == EASY_KEYWORD_TYPE::ELSE)
		{
			++index;
			ast->False = parseAst();
		}

        return reinterpret_cast<Ast*>(ast);
    }

	Ast* parseFunctionCall()
	{
		auto* ast = new FunctionCallAst;
		auto* token = getToken();
		ast->Function = reinterpret_cast<SymbolToken*>(token)->Value;
		++index;
		ast->Args.push_back(parseAst());

		return reinterpret_cast<Ast*>(ast);
	}

    Ast* parseBinaryOperationStatement()
    {
		++index;
        auto* ast = new BinaryAst;
		auto* token = getToken();

		if (isPrimative(token))
			ast->Left = parsePrimative(token);
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
			ast->Right = parsePrimative(token);
		else if (token->GetType() == EASY_TOKEN_TYPE::SYMBOL)
			ast->Right = new VariableAst(reinterpret_cast<SymbolToken*>(token)->Value);

        return reinterpret_cast<Ast*>(ast);
    }

    Ast* parseAst()
    {
        auto* token = getToken();
        auto* tokenNext = getNextToken();

        Ast* ast = nullptr;

        if (detectType() == AstType::ASSIGNMENT)
            ast = parseAssignment();
        else if (detectType() == AstType::IF_STATEMENT)
            ast = parseIfStatement();
		else if (detectType() == AstType::FUNCTION_CALL)
			ast = parseFunctionCall();
		else if (detectType() == AstType::PRIMATIVE)
			ast = parsePrimative();

        return ast;
    }

    void parse()
    {
        tokensCount = tokens->size();
        index = 0;

        while(index < tokensCount)
        {
            asts->push_back(parseAst());
            ++index;
        }
    }

	void dump(std::shared_ptr<std::vector<Ast*>> asts)
	{
		auto astsEnd = asts->end();
		for (auto it = asts->begin(); it != astsEnd; ++it) {
			dumpLevel(*it, 0);
		}
	}

	void levelPadding(int level)
	{
		for (int i = 0; i < level; ++i)
			std::cout << "  ";
	}

	void dumpLevel(Ast* ast, int level)
	{
		if (ast == nullptr)
			return;

		levelPadding(level);
		switch(ast->GetType())
		{
			case AstType::IF_STATEMENT:
			{
				auto* ifStatement = reinterpret_cast<IfStatementAst*>(ast);

				dumpLevel(ifStatement->BinaryOpt, level+1);
				dumpLevel(ifStatement->True, level+1);
				dumpLevel(ifStatement->False, level+1);
			}
				break;

			case AstType::ASSIGNMENT: {
				auto *assignment = reinterpret_cast<AssignmentAst *>(ast);
				levelPadding(level);
				std::wcout << assignment->Name << std::endl;
				dumpLevel(assignment->Data, level+1);
			}
				break;

			case AstType::VARIABLE: {
				auto *variable = reinterpret_cast<VariableAst *>(ast);
				levelPadding(level);
				std::wcout << variable->Value << std::endl;
			}
				break;

			case AstType::PRIMATIVE: {
				auto *primative = reinterpret_cast<PrimativeAst *>(ast);
				levelPadding(level);
				switch (primative->ValueType) {
					case PrimativeValueType::PRI_INTEGER:
						std::wcout << primative->Value->Integer << std::endl;
						break;

					case PrimativeValueType::PRI_DOUBLE:
						std::wcout << primative->Value->Double << std::endl;
						break;

					case PrimativeValueType::PRI_STRING:
						std::wcout << primative->Value->String << std::endl;
						break;

					case PrimativeValueType::PRI_BOOL:
						std::wcout << primative->Value->Bool << std::endl;
						break;
				}
			}
				break;

			case AstType::BINARY_OPERATION: {
				auto *binary = reinterpret_cast<BinaryAst *>(ast);
				dumpLevel(binary->Left, level+1);
				levelPadding(level);
				std::wcout << EASY_OPERATOR_TYPEToString(binary->Op) << std::endl;
				dumpLevel(binary->Right, level+1);
			}
				break;

			case AstType::FUNCTION_CALL:
			{
				auto* functionCall = reinterpret_cast<FunctionCallAst*>(ast);
				levelPadding(level);
				std::wcout << functionCall->Function << std::endl;
				auto astsEnd = functionCall->Args.end();
				for (auto it = functionCall->Args.begin(); it != astsEnd; ++it)
					dumpLevel(*it, level+1);
			}
				break;
		}
	}
};

AstParser::AstParser()
{
    impl = new AstParserImpl;
}
void AstParser::AddMethod(std::wstring const & method, MethodCallback callback)
{
	impl->systemMethods[method] = callback;
    //impl->systemMethodsEnd = impl->systemMethods.end();
}

void AstParser::Parse(std::shared_ptr<std::vector<Token*>> tokens, std::shared_ptr<std::vector<Ast*>> asts)
{
	asts->clear();

	impl->tokens = tokens;
	impl->asts = asts;
	impl->parse();
}

void AstParser::Dump(std::shared_ptr<std::vector<Ast*>> asts)
{
	impl->dump(asts);
}
