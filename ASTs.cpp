#include "ASTs.h"

class AstParserImpl
{
public:
    std::shared_ptr<std::vector<Token*>> tokens;
	std::shared_ptr<std::vector<Ast*>> asts;
	std::unordered_map<std::wstring, MethodCallback> systemMethods;
	std::unordered_map<std::wstring, MethodCallback> userMethods;
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

		if (token->GetType() == EASY_TOKEN_TYPE::KEYWORD && reinterpret_cast<KeywordToken*>(token)->Value == EASY_KEYWORD_TYPE::BLOCK_START)
			return AstType::BLOCK;

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
		
		++index;
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

		if (tokenNext != nullptr && tokenNext->GetType() == EASY_TOKEN_TYPE::OPERATOR)
		{
			auto* operatorToken = reinterpret_cast<OperatorToken*>(tokenNext);
			if (BinaryOperators.find(operatorToken->Value) != BinaryOperatorsEnd)
			{
				// 1 + 1
				--index;
				ast->Data = parseBinaryOperationStatement();
			}
			else if (ControlOperators.find(operatorToken->Value) != ControlOperatorsEnd) {
				// TODO: FIXIT 1 < 2
			}
		}
		else if (isPrimative())
			ast->Data = parsePrimative(token);
              
        return reinterpret_cast<Ast*>(ast);
    }

    Ast* parseIfStatement()
    {
        auto* ast = new IfStatementAst;
        ast->ControlOpt = parseControlOperationStatement();
		++index;

        auto* token = getToken();
		ast->True = parseAst();
		token = getToken();

		if (token != nullptr && token->GetType() == EASY_TOKEN_TYPE::KEYWORD && reinterpret_cast<KeywordToken*>(token)->Value == EASY_KEYWORD_TYPE::ELSE)
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
		{
			ast->Left = new VariableAst(reinterpret_cast<SymbolToken*>(token)->Value);
			++index;
		}

		token = getToken();
		if (token->GetType() == EASY_TOKEN_TYPE::OPERATOR &&
			BinaryOperators.find(reinterpret_cast<OperatorToken*>(token)->Value) != BinaryOperatorsEnd)
			ast->Op = reinterpret_cast<OperatorToken*>(token)->Value;
		else
			ast->Op = EASY_OPERATOR_TYPE::OPERATOR_NONE;
			
		++index;

		token = getToken();
		if (isPrimative(token))
			ast->Right = parsePrimative(token);
		else if (token->GetType() == EASY_TOKEN_TYPE::SYMBOL)
			ast->Right = new VariableAst(reinterpret_cast<SymbolToken*>(token)->Value);

        return reinterpret_cast<Ast*>(ast);
    }

	Ast* parseBlock()
	{
		BlockAst* block = new BlockAst();
		
		++index;
		while (index < tokensCount)
		{
			auto* token = getToken();
			if (token->GetType() == EASY_TOKEN_TYPE::KEYWORD && reinterpret_cast<KeywordToken*>(token)->Value == EASY_KEYWORD_TYPE::BLOCK_END)
			{
				++index;
				break;
			}

			block->Blocks->push_back(parseAst());
		}

		return reinterpret_cast<Ast*>(block);
	}

	Ast* parseControlOperationStatement()
	{
		++index;
		auto* ast = new ControlAst;
		auto* token = getToken();

		if (isPrimative(token))
			ast->Left = parsePrimative(token);
		else if (token->GetType() == EASY_TOKEN_TYPE::SYMBOL)
			ast->Left = new VariableAst(reinterpret_cast<SymbolToken*>(token)->Value);

		++index;
		token = getToken();

		if (token->GetType() == EASY_TOKEN_TYPE::OPERATOR &&
			ControlOperators.find(reinterpret_cast<OperatorToken*>(token)->Value) != ControlOperatorsEnd)
			ast->Op = reinterpret_cast<OperatorToken*>(token)->Value;
		else
			ast->Op = EASY_OPERATOR_TYPE::OPERATOR_NONE;

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

		AstType astType = detectType();
		if (astType == AstType::ASSIGNMENT)
			ast = parseAssignment();
		else if (astType == AstType::IF_STATEMENT)
			ast = parseIfStatement();
		else if (astType == AstType::FUNCTION_CALL)
			ast = parseFunctionCall();
		else if (astType == AstType::PRIMATIVE)
			ast = parsePrimative();
		else if (astType == AstType::BLOCK)
			ast = parseBlock();
		else
			++index;

        return ast;
    }

    void parse()
    {
        tokensCount = tokens->size();
        index = 0;

        while(index < tokensCount)
        {
            asts->push_back(parseAst());
        }
    }

	void dump(std::shared_ptr<std::vector<Ast*>> asts)
	{
		std::wcout << std::endl << std::endl << "### AST TREE ###" << std::endl << std::endl;

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

	void dumpLevel(Ast* ast, int level, bool printPadding = true)
	{
		if (ast == nullptr)
		{
			std::cout << "[NONE]";
			return;
		}

		if (printPadding)
			levelPadding(level);

		switch(ast->GetType())
		{
			case AstType::IF_STATEMENT:
			{
				auto* ifStatement = reinterpret_cast<IfStatementAst*>(ast);

				std::wcout << "#IF STATEMENT : " << std::endl;
				std::wcout << "  Control Operation : ";
				dumpLevel(ifStatement->ControlOpt, level+1, false);
				std::wcout << "  True : " << std::endl;
				dumpLevel(ifStatement->True, level+1, printPadding);
				std::wcout << " False : " << std::endl;
				dumpLevel(ifStatement->False, level+1, printPadding);
			}
				break;

			case AstType::ASSIGNMENT: {
				auto *assignment = reinterpret_cast<AssignmentAst *>(ast);
				levelPadding(level);

				std::wcout << "#ASSIGNMENT : " << assignment->Name << " ";
				dumpLevel(assignment->Data, level+1, false);
			}
				break;

			case AstType::VARIABLE: {
				auto *variable = reinterpret_cast<VariableAst *>(ast);
				if (printPadding)
					levelPadding(level);

				std::wcout << "$" << variable->Value;
			}
				break;

			case AstType::PRIMATIVE: {
				auto *primative = reinterpret_cast<PrimativeAst *>(ast);
				if (printPadding)
					levelPadding(level);

				switch (primative->Value->Type) {
				case PrimativeValue::Type::PRI_INTEGER:
						std::wcout << primative->Value->Integer << " [INTEGER]" << std::endl;
						break;

					case PrimativeValue::Type::PRI_DOUBLE:
						std::wcout << primative->Value->Double << " [DOUBLE]" << std::endl;
						break;

					case PrimativeValue::Type::PRI_STRING:
						std::wcout << "'" << primative->Value->String << "'" << " [STRING]" << std::endl;
						break;

					case PrimativeValue::Type::PRI_BOOL:
						std::wcout << primative->Value->Bool << " [BOOL]" << std::endl;
						break;
				}
			}
				break;

			case AstType::BINARY_OPERATION: {
				auto *binary = reinterpret_cast<BinaryAst *>(ast);
				dumpLevel(binary->Left, level+1, false);

				std::wcout << " " << EASY_OPERATOR_TYPEToString(binary->Op) << " ";
				dumpLevel(binary->Right, level+1, false);
			}
				break;

			case AstType::CONTROL_OPERATION: {
				auto *binary = reinterpret_cast<ControlAst *>(ast);
				dumpLevel(binary->Left, level+1, false);

				std::wcout << " " << EASY_OPERATOR_TYPEToString(binary->Op) << " ";
				dumpLevel(binary->Right, level+1, false);
			}
				break;

			case AstType::FUNCTION_CALL:
			{
				auto* functionCall = reinterpret_cast<FunctionCallAst*>(ast);
				if (printPadding)
					levelPadding(level);

				std::wcout << "#METHOD CALL : " << functionCall->Function << " ";
				auto astsEnd = functionCall->Args.end();
				for (auto it = functionCall->Args.begin(); it != astsEnd; ++it)
				{
					dumpLevel(*it, level + 1, false);
					std::wcout << " ";
				}
			}
				break;

			case AstType::BLOCK:
			{
				auto* block = reinterpret_cast<BlockAst*>(ast);
				auto blockEnd = block->Blocks->cend();

				for (auto it = block->Blocks->cbegin(); it != blockEnd; ++it)
				{
					dumpLevel(*it, level + 1);
				}
			}
			break;
		}
	}
};

namespace {
	void print(std::wstring const & message)
	{
		std::wcout << message << std::endl;
	}
}

AstParser::AstParser()
{
    impl = new AstParserImpl;
	this->AddMethod(L"yaz", &print);

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
