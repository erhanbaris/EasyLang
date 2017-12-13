#include "ASTs.h"
#include "Exceptions.h"
#include "System.h"

class AstParserImpl
{
public:
	std::shared_ptr<std::vector<Token*>> tokens;
	std::shared_ptr<std::vector<Ast*>> asts;
	std::unordered_map<std::wstring, MethodCallback> userMethods;

	size_t tokensCount;
	size_t index;

	Token* getToken()
	{
		if (index < tokensCount)
			return tokens->at(index);

		return nullptr;
	}

    Token* getNextToken(EASY_TOKEN_TYPE skipTokenType = EASY_TOKEN_TYPE::TOKEN_NONE)
	{
        if (skipTokenType == EASY_TOKEN_TYPE::TOKEN_NONE)
        {
            if (index + 1 < tokensCount)
                return tokens->at(index + 1);
        }
        else
        {
            size_t tmpIndexer = 1;
            while(index + tmpIndexer  < tokensCount)
            {
                if (index + tmpIndexer < tokensCount && tokens->at(index + tmpIndexer)->GetType() != skipTokenType)
                    return tokens->at(index + tmpIndexer);
                
                ++tmpIndexer;
            }
        }
        
        return nullptr;
	}
    
	EASY_AST_TYPE detectType()
	{
		auto* token = getToken();
		auto* tokenNext = getNextToken(EASY_TOKEN_TYPE::WHITESPACE);

        if (token->GetType() == EASY_TOKEN_TYPE::SYMBOL && tokenNext != nullptr && tokenNext->GetType() == EASY_TOKEN_TYPE::OPERATOR && reinterpret_cast<OperatorToken*>(tokenNext)->Value == EASY_OPERATOR_TYPE::OPERATION)
            return EASY_AST_TYPE::FOR;
        
		if (token->GetType() == EASY_TOKEN_TYPE::SYMBOL && tokenNext != nullptr && tokenNext->GetType() == EASY_TOKEN_TYPE::OPERATOR && reinterpret_cast<OperatorToken*>(tokenNext)->Value == EASY_OPERATOR_TYPE::ASSIGN)
			return EASY_AST_TYPE::ASSIGNMENT;

		if (token->GetType() == EASY_TOKEN_TYPE::KEYWORD && reinterpret_cast<KeywordToken*>(token)->Value == EASY_KEYWORD_TYPE::IF)
			return EASY_AST_TYPE::IF_STATEMENT;

		if (token->GetType() == EASY_TOKEN_TYPE::KEYWORD && reinterpret_cast<KeywordToken*>(token)->Value == EASY_KEYWORD_TYPE::FUNC)
			return EASY_AST_TYPE::FUNCTION_DECLERATION;

		if (token->GetType() == EASY_TOKEN_TYPE::KEYWORD && reinterpret_cast<KeywordToken*>(token)->Value == EASY_KEYWORD_TYPE::BLOCK_START)
			return EASY_AST_TYPE::BLOCK;

		if (token->GetType() == EASY_TOKEN_TYPE::SYMBOL && System::SystemMethods.find(reinterpret_cast<SymbolToken*>(token)->Value) != System::SystemMethods.end())
			return EASY_AST_TYPE::FUNCTION_CALL;

		if (tokenNext != nullptr && tokenNext->GetType() == EASY_TOKEN_TYPE::OPERATOR && BinaryOperators.find(reinterpret_cast<OperatorToken*>(tokenNext)->Value) != BinaryOperators.end())
			return EASY_AST_TYPE::BINARY_OPERATION;

		if (tokenNext != nullptr && tokenNext->GetType() == EASY_TOKEN_TYPE::OPERATOR && ControlOperators.find(reinterpret_cast<OperatorToken*>(tokenNext)->Value) != ControlOperators.end())
			return EASY_AST_TYPE::CONTROL_OPERATION;

		if (isPrimative(token))
			return EASY_AST_TYPE::PRIMATIVE;
        
        if (token->GetType() == EASY_TOKEN_TYPE::SYMBOL)
            return EASY_AST_TYPE::VARIABLE;

		return EASY_AST_TYPE::NONE;
	}

	inline bool isPrimative()
	{
		return isPrimative(getToken());
	}

	inline bool isPrimative(Token * token)
	{
		return token != nullptr && (token->GetType() == EASY_TOKEN_TYPE::INTEGER ||
			token->GetType() == EASY_TOKEN_TYPE::TEXT ||
			token->GetType() == EASY_TOKEN_TYPE::DOUBLE ||
			(token->GetType() == EASY_TOKEN_TYPE::KEYWORD && reinterpret_cast<KeywordToken*>(token)->Value == EASY_KEYWORD_TYPE::BOOL_TRUE) ||
			(token->GetType() == EASY_TOKEN_TYPE::KEYWORD && reinterpret_cast<KeywordToken*>(token)->Value == EASY_KEYWORD_TYPE::BOOL_FALSE));
	}
    
    inline void skip(EASY_TOKEN_TYPE skipToken)
    {
        Token* token = getToken();
        if (token != nullptr && token->GetType() == skipToken)
            ++index;
    }
    
    inline void skipWhiteSpace()
    {
        skip(EASY_TOKEN_TYPE::WHITESPACE);
    }
    
    inline void consumeToken(EASY_TOKEN_TYPE skipToken)
    {
        Token* token = getToken();
        if (token != nullptr && token->GetType() == skipToken)
            ++index;
        else
            throw ParseError("Syntax error.");
            
    }
    
    inline void consumeOperator(EASY_OPERATOR_TYPE skipOperator)
    {
        Token* token = getToken();
        if (token != nullptr && token->GetType() == EASY_TOKEN_TYPE::OPERATOR && reinterpret_cast<OperatorToken*>(token)->Value == skipOperator)
            ++index;
        else
            throw ParseError("Syntax error.");
        
    }
    
    inline void consumeKeyword(EASY_KEYWORD_TYPE skipKeyword)
    {
        Token* token = getToken();
        if (token != nullptr && token->GetType() == EASY_TOKEN_TYPE::KEYWORD && reinterpret_cast<KeywordToken*>(token)->Value == skipKeyword)
            ++index;
        else
            throw ParseError("Syntax error.");
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

		case EASY_TOKEN_TYPE::KEYWORD:
			if (reinterpret_cast<KeywordToken*>(token)->Value == EASY_KEYWORD_TYPE::BOOL_TRUE)
				ast = new PrimativeAst(true);
			else if (reinterpret_cast<KeywordToken*>(token)->Value == EASY_KEYWORD_TYPE::BOOL_FALSE)
				ast = new PrimativeAst(false);
			break;
		}

		++index;
		return reinterpret_cast<Ast*>(ast);
	}

	Ast* parseAssignment()
	{
        skipWhiteSpace();
		auto* token = getToken();
       
		auto* ast = new AssignmentAst;
		ast->Name = reinterpret_cast<SymbolToken*>(token)->Value;

		++index;
		skipWhiteSpace();

		consumeOperator(EASY_OPERATOR_TYPE::ASSIGN);

        skipWhiteSpace();
		token = getToken();

        if (token == nullptr)
            throw ParseError("Value required");

		ast->Data = parseAst();

		return reinterpret_cast<Ast*>(ast);
	}

	Ast* parseIfStatement()
	{
		auto* ast = new IfStatementAst;
        
        ++index;
		ast->ControlOpt = parseControlOperationStatement();
		++index;

        skipWhiteSpace();
        consumeKeyword(EASY_KEYWORD_TYPE::THEN);
		skipWhiteSpace();
        
        auto* token = getToken();
        ast->True = parseAst();
        skipWhiteSpace();
		token = getToken();

		if (token != nullptr && token->GetType() == EASY_TOKEN_TYPE::KEYWORD && reinterpret_cast<KeywordToken*>(token)->Value == EASY_KEYWORD_TYPE::ELSE)
		{
			++index;
            skipWhiteSpace();
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
        skipWhiteSpace();
		token = getToken();
		if (token->GetType() == EASY_TOKEN_TYPE::OPERATOR && reinterpret_cast<OperatorToken*>(token)->Value == EASY_OPERATOR_TYPE::LEFT_PARENTHESES)
		{
			while (index < tokensCount)
			{
				++index;
				skipWhiteSpace();
				ast->Args.push_back(parseAst());
				skipWhiteSpace();
				token = getToken();
				if (token->GetType() == EASY_TOKEN_TYPE::OPERATOR && reinterpret_cast<OperatorToken*>(token)->Value == EASY_OPERATOR_TYPE::RIGHT_PARENTHESES)
					break;
				else if (token->GetType() == EASY_TOKEN_TYPE::OPERATOR && reinterpret_cast<OperatorToken*>(token)->Value == EASY_OPERATOR_TYPE::COMMA)
					continue;

				throw ParseError("',' required");
			}

			skipWhiteSpace();
			consumeOperator(EASY_OPERATOR_TYPE::RIGHT_PARENTHESES);
		}
		else if (token->GetType() == EASY_TOKEN_TYPE::KEYWORD && reinterpret_cast<KeywordToken*>(token)->Value == EASY_KEYWORD_TYPE::EMPTY_PARAMETER)
		{
			ast->Args.push_back(reinterpret_cast<Ast*>(new PrimativeAst()));
			++index;
		}
		else 
			ast->Args.push_back(parseAst());

		return reinterpret_cast<Ast*>(ast);
	}

	Ast* parseBinaryOperationStatement()
	{
		auto* ast = new BinaryAst;
        skipWhiteSpace();
        auto* token = getToken();

		if (isPrimative(token))
			ast->Left = parsePrimative(token);
		else if (token->GetType() == EASY_TOKEN_TYPE::SYMBOL)
        {
            ast->Left = new VariableAst(reinterpret_cast<SymbolToken*>(token)->Value);
            ++index;
        }
        
        if (ast->Left == nullptr)
            throw ParseError("Binary operation left argument is empty.");

        skipWhiteSpace();
		token = getToken();
		if (token->GetType() == EASY_TOKEN_TYPE::OPERATOR &&
			BinaryOperators.find(reinterpret_cast<OperatorToken*>(token)->Value) != BinaryOperatorsEnd)
			ast->Op = reinterpret_cast<OperatorToken*>(token)->Value;
		else
			ast->Op = EASY_OPERATOR_TYPE::OPERATOR_NONE;
        
        if (ast->Op == EASY_OPERATOR_TYPE::OPERATOR_NONE)
            throw ParseError("Binary operation operator is empty.");
        
        ++index;
        skipWhiteSpace();
		token = getToken();
		if (isPrimative(token))
			ast->Right = parsePrimative(token);
		else if (token != nullptr && token->GetType() == EASY_TOKEN_TYPE::SYMBOL)
			ast->Right = new VariableAst(reinterpret_cast<SymbolToken*>(token)->Value);

        if (ast->Right == nullptr)
            throw ParseError("Binary operation right argument is empty.");
        
		return reinterpret_cast<Ast*>(ast);
	}

	Ast* parseBlock()
	{
		BlockAst* block = new BlockAst();
        skipWhiteSpace();
		++index;
		while (index < tokensCount)
		{
            skipWhiteSpace();
			auto* token = getToken();
			if (token->GetType() == EASY_TOKEN_TYPE::KEYWORD && reinterpret_cast<KeywordToken*>(token)->Value == EASY_KEYWORD_TYPE::BLOCK_END)
			{
				++index;
                skipWhiteSpace();
				break;
			}

			block->Blocks->push_back(parseAst());
		}

		return reinterpret_cast<Ast*>(block);
	}

	Ast* parseControlOperationStatement()
	{
		auto* ast = new ControlAst;
        skipWhiteSpace();
		auto* token = getToken();

		if (isPrimative(token))
			ast->Left = parsePrimative(token);
		else if (token->GetType() == EASY_TOKEN_TYPE::SYMBOL)
        {
			ast->Left = new VariableAst(reinterpret_cast<SymbolToken*>(token)->Value);
            ++index;
        }
        
        if (ast->Left == nullptr)
            throw ParseError("Binary operation left argument is empty.");
        
        skipWhiteSpace();
        token = getToken();

		if (token->GetType() == EASY_TOKEN_TYPE::OPERATOR &&
			ControlOperators.find(reinterpret_cast<OperatorToken*>(token)->Value) != ControlOperatorsEnd)
			ast->Op = reinterpret_cast<OperatorToken*>(token)->Value;
		else
			ast->Op = EASY_OPERATOR_TYPE::OPERATOR_NONE;

        if (ast->Op == EASY_OPERATOR_TYPE::OPERATOR_NONE)
            throw ParseError("Binary operation operator is empty.");
        
		++index;
        skipWhiteSpace();
		token = getToken();
		if (isPrimative(token))
			ast->Right = parsePrimative(token);
		else if (token->GetType() == EASY_TOKEN_TYPE::SYMBOL)
			ast->Right = new VariableAst(reinterpret_cast<SymbolToken*>(token)->Value);

        if (ast->Right == nullptr)
            throw ParseError("Binary operation right argument is empty.");
        
		return reinterpret_cast<Ast*>(ast);
	}
	
	Ast* parseFunctionDecleration()
	{
		auto* ast = new FunctionDefinetionAst;
		skipWhiteSpace();
		auto* token = getToken();

		ast->Name = reinterpret_cast<SymbolToken*>(token)->Value;
		++index; 
		skipWhiteSpace();

		skipWhiteSpace();
		++index;
		skipWhiteSpace();
		token = getToken();

		if (token->GetType() == EASY_TOKEN_TYPE::DOUBLE)
			ast->Start = new PrimativeAst(reinterpret_cast<DoubleToken*>(token)->Value);
		else if (token->GetType() == EASY_TOKEN_TYPE::INTEGER)
			ast->Start = new PrimativeAst(reinterpret_cast<IntegerToken*>(token)->Value);
		else if (token->GetType() == EASY_TOKEN_TYPE::VARIABLE)
			ast->Start = new VariableAst(reinterpret_cast<VariableToken*>(token)->Value);
		else
			throw ParseError("For repeat works with variable, double and integer");
		skipWhiteSpace();
		++index;
		skipWhiteSpace();
		++index;
		skipWhiteSpace();
		token = getToken();

		if (token->GetType() == EASY_TOKEN_TYPE::DOUBLE)
			ast->End = new PrimativeAst(reinterpret_cast<DoubleToken*>(token)->Value);
		else if (token->GetType() == EASY_TOKEN_TYPE::INTEGER)
			ast->End = new PrimativeAst(reinterpret_cast<IntegerToken*>(token)->Value);
		else if (token->GetType() == EASY_TOKEN_TYPE::VARIABLE)
			ast->End = new VariableAst(reinterpret_cast<VariableToken*>(token)->Value);
		else
			throw ParseError("For repeat works with variable, double and integer");

		if (getNextToken() == nullptr)
			throw ParseError("Repeat block missing");

		++index;
		skipWhiteSpace();
		ast->Repeat = parseAst();


		return reinterpret_cast<Ast*>(ast);
	}

    Ast* parseForStatement()
    {
        auto* ast = new ForStatementAst;
        skipWhiteSpace();
        auto* token = getToken();
        
        ast->Variable = reinterpret_cast<SymbolToken*>(token)->Value;
        skipWhiteSpace();
        ++index;
        skipWhiteSpace();
        ++index;
        skipWhiteSpace();
        token = getToken();
        
        if (token->GetType() == EASY_TOKEN_TYPE::DOUBLE)
            ast->Start = new PrimativeAst(reinterpret_cast<DoubleToken*>(token)->Value);
        else if (token->GetType() == EASY_TOKEN_TYPE::INTEGER)
            ast->Start = new PrimativeAst(reinterpret_cast<IntegerToken*>(token)->Value);
        else if (token->GetType() == EASY_TOKEN_TYPE::VARIABLE)
            ast->Start = new VariableAst(reinterpret_cast<VariableToken*>(token)->Value);
        else
            throw ParseError("For repeat works with variable, double and integer");
        skipWhiteSpace();
        ++index;
        skipWhiteSpace();
        ++index;
        skipWhiteSpace();
        token = getToken();
        
        if (token->GetType() == EASY_TOKEN_TYPE::DOUBLE)
            ast->End = new PrimativeAst(reinterpret_cast<DoubleToken*>(token)->Value);
        else if (token->GetType() == EASY_TOKEN_TYPE::INTEGER)
            ast->End = new PrimativeAst(reinterpret_cast<IntegerToken*>(token)->Value);
        else if (token->GetType() == EASY_TOKEN_TYPE::VARIABLE)
            ast->End = new VariableAst(reinterpret_cast<VariableToken*>(token)->Value);
        else
            throw ParseError("For repeat works with variable, double and integer");
        
        if (getNextToken() == nullptr)
            throw ParseError("Repeat block missing");
        
        ++index;
        skipWhiteSpace();
        ast->Repeat = parseAst();

        
        return reinterpret_cast<Ast*>(ast);
    }

	Ast* parseAst()
	{
		Ast* ast = nullptr;
        
        skipWhiteSpace();
        auto* token = getToken();
        
        if (token == nullptr)
            return nullptr;

		EASY_AST_TYPE type = detectType();
		switch (type) {
		case EASY_AST_TYPE::ASSIGNMENT:
			ast = parseAssignment();
			break;

		case EASY_AST_TYPE::IF_STATEMENT:
			ast = parseIfStatement();
			break;

		case EASY_AST_TYPE::FUNCTION_DECLERATION:
			ast = parseFunctionDecleration();
			break;

		case EASY_AST_TYPE::FUNCTION_CALL:
			ast = parseFunctionCall();
			break;

		case EASY_AST_TYPE::PRIMATIVE:
			ast = parsePrimative();
			break;

		case EASY_AST_TYPE::BLOCK:
			ast = parseBlock();
			break;

		case EASY_AST_TYPE::BINARY_OPERATION:
			ast = parseBinaryOperationStatement();
			break;
                
        case EASY_AST_TYPE::CONTROL_OPERATION:
            ast = parseControlOperationStatement();
            break;
                
        case EASY_AST_TYPE::FOR:
            ast = parseForStatement();
            break;
                
        case EASY_AST_TYPE::VARIABLE:
            {
                auto* variableAst = new VariableAst;
                variableAst->Value = reinterpret_cast<VariableToken*>(getToken())->Value;
                ast = reinterpret_cast<Ast*>(variableAst);
                ++index;
            }
            break;

		default:
                throw ParseError("#ERROR");
			break;
		}

		return ast;
	}

	void parse()
	{
		tokensCount = tokens->size();
		index = 0;

		while (index < tokensCount)
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

		switch (ast->GetType())
		{
		case EASY_AST_TYPE::IF_STATEMENT:
		{
			auto* ifStatement = reinterpret_cast<IfStatementAst*>(ast);

			std::wcout << "#IF STATEMENT : " << std::endl;
			std::wcout << "  Control Operation : ";
			dumpLevel(ifStatement->ControlOpt, level + 1, false);
			std::wcout << "  True : " << std::endl;
			dumpLevel(ifStatement->True, level + 1, printPadding);
			std::wcout << " False : " << std::endl;
			dumpLevel(ifStatement->False, level + 1, printPadding);
		}
		break;

		case EASY_AST_TYPE::ASSIGNMENT: {
			auto *assignment = reinterpret_cast<AssignmentAst *>(ast);
			levelPadding(level);

			std::wcout << "#ASSIGNMENT : " << assignment->Name << " ";
			dumpLevel(assignment->Data, level + 1, false);
		}
								  break;

		case EASY_AST_TYPE::VARIABLE: {
			auto *variable = reinterpret_cast<VariableAst *>(ast);
			if (printPadding)
				levelPadding(level);

			std::wcout << "$" << variable->Value;
		}
								break;

		case EASY_AST_TYPE::PRIMATIVE: {
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

		case EASY_AST_TYPE::BINARY_OPERATION: {
			auto *binary = reinterpret_cast<BinaryAst *>(ast);
			dumpLevel(binary->Left, level + 1, false);

			std::wcout << " " << EASY_OPERATOR_TYPEToString(binary->Op) << " ";
			dumpLevel(binary->Right, level + 1, false);
		}
										break;

		case EASY_AST_TYPE::CONTROL_OPERATION: {
			auto *binary = reinterpret_cast<ControlAst *>(ast);
			dumpLevel(binary->Left, level + 1, false);

			std::wcout << " " << EASY_OPERATOR_TYPEToString(binary->Op) << " ";
			dumpLevel(binary->Right, level + 1, false);
		}
										 break;

		case EASY_AST_TYPE::FUNCTION_CALL:
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

		case EASY_AST_TYPE::BLOCK:
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

void AstParser::Dump(std::shared_ptr<std::vector<Ast*>> asts)
{
	impl->dump(asts);
}
