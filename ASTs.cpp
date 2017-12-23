#include "ASTs.h"
#include "Exceptions.h"
#include "System.h"

#define GET_ITEM(NAME, TYPE, IN, OUT) inline OUT get##NAME (Token* token)\
	{\
	return reinterpret_cast<IN*>(token)->Value;\
	}\
	inline OUT get##NAME ()\
	{\
		return reinterpret_cast<IN*>(getToken())->Value;\
	}\
	inline bool is##NAME (Token* token)\
	{\
		return token != nullptr && token->GetType() == TYPE ;\
	}\
	inline bool is##NAME ()\
	{\
		return getToken() != nullptr && getToken()->GetType() == TYPE ;\
	}
#define AS_AST(ast) reinterpret_cast<Ast*>(ast)

class AstParserImpl
{
public:
	std::shared_ptr<std::vector<Token*>> tokens;
	std::shared_ptr<std::vector<Ast*>> asts;
	std::unordered_map<std::wstring, MethodCallback> userMethods;

	size_t tokensCount;
	size_t index;

	Token* current_token{ nullptr };
	Token* next_token{ nullptr };

	GET_ITEM(Keyword, EASY_TOKEN_TYPE::KEYWORD, KeywordToken, EASY_KEYWORD_TYPE);
	GET_ITEM(Operator, EASY_TOKEN_TYPE::OPERATOR, OperatorToken, EASY_OPERATOR_TYPE);
	GET_ITEM(Integer, EASY_TOKEN_TYPE::INTEGER, IntegerToken, int);
	GET_ITEM(Double, EASY_TOKEN_TYPE::DOUBLE, DoubleToken, double);
	GET_ITEM(Text, EASY_TOKEN_TYPE::TEXT, TextToken, std::wstring);
	GET_ITEM(Symbol, EASY_TOKEN_TYPE::SYMBOL, SymbolToken, std::wstring);
	GET_ITEM(Variable, EASY_TOKEN_TYPE::VARIABLE, VariableToken, std::wstring);

	Token* getToken()
	{
		if (index < tokensCount)
			return tokens->at(index);

		return nullptr;
	}

	void checkToken(std::string const & message)
	{
		if (getToken() == nullptr)
			throw ParseError(message);
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
			while (index + tmpIndexer < tokensCount)
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

		checkToken("Parse error");
		if (isOperator(token) && getOperator(token) == EASY_OPERATOR_TYPE::LEFT_PARENTHESES)
			return EASY_AST_TYPE::PARENTHESES_BLOCK;

		if (token->GetType() == EASY_TOKEN_TYPE::SYMBOL && tokenNext != nullptr && isOperator(tokenNext) && getOperator(tokenNext) == EASY_OPERATOR_TYPE::OPERATION)
			return EASY_AST_TYPE::FOR;

		if (token->GetType() == EASY_TOKEN_TYPE::SYMBOL && ((tokenNext != nullptr && isOperator(tokenNext) && getOperator(tokenNext) == EASY_OPERATOR_TYPE::LEFT_PARENTHESES) || System::UserMethods.find(getSymbol(token)) != System::UserMethods.end() || System::SystemMethods.find(getSymbol(token)) != System::SystemMethods.end()))
			return EASY_AST_TYPE::FUNCTION_CALL;

		if (token->GetType() == EASY_TOKEN_TYPE::SYMBOL && tokenNext != nullptr && isOperator(tokenNext) && getOperator(tokenNext) == EASY_OPERATOR_TYPE::ASSIGN)
			return EASY_AST_TYPE::ASSIGNMENT;

		if (isKeyword(token) && getKeyword(token) == EASY_KEYWORD_TYPE::RETURN)
			return EASY_AST_TYPE::RETURN;

		if (isKeyword(token) && getKeyword(token) == EASY_KEYWORD_TYPE::IF)
			return EASY_AST_TYPE::IF_STATEMENT;

		if (isKeyword(token) && getKeyword(token) == EASY_KEYWORD_TYPE::FUNC)
			return EASY_AST_TYPE::FUNCTION_DECLERATION;

		if (isKeyword(token) && getKeyword(token) == EASY_KEYWORD_TYPE::BLOCK_START)
			return EASY_AST_TYPE::BLOCK;

		if (tokenNext != nullptr && isOperator(tokenNext) && BinaryOperators.find(getOperator(tokenNext)) != BinaryOperators.end())
			return EASY_AST_TYPE::BINARY_OPERATION;

		if (tokenNext != nullptr && isOperator(tokenNext) && ControlOperators.find(getOperator(tokenNext)) != ControlOperators.end())
			return EASY_AST_TYPE::CONTROL_OPERATION;

		if (isPrimative(token))
			return EASY_AST_TYPE::PRIMATIVE;

		if (token->GetType() == EASY_TOKEN_TYPE::SYMBOL)
			return EASY_AST_TYPE::VARIABLE;

		throw ParseError("Parse error");
	}

	inline bool isPrimative()
	{
		return isPrimative(getToken());
	}

	inline bool isPrimative(Token * token)
	{
		checkToken("Parse error");
		return token != nullptr && (isInteger(token) ||
			isText(token) ||
			isDouble(token) ||
			(isKeyword(token) && getKeyword(token) == EASY_KEYWORD_TYPE::BOOL_TRUE) ||
			(isKeyword(token) && getKeyword(token) == EASY_KEYWORD_TYPE::BOOL_FALSE));
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

	inline Token* increaseAndClear()
	{
        current_token = getToken();
        next_token = getNextToken(EASY_TOKEN_TYPE::WHITESPACE);

		++index;
		skip(EASY_TOKEN_TYPE::WHITESPACE);
		return current_token;
	}

	inline void consumeToken(EASY_TOKEN_TYPE type)
	{
		Token* token = getToken();
		if (token != nullptr && token->GetType() == type)
			++index;
		else
			throw ParseError("Syntax error.");

	}

	inline void consumeOperator(EASY_OPERATOR_TYPE type)
	{
		Token* token = getToken();
		if (token != nullptr && isOperator(token) && getOperator(token) == type)
			++index;
		else
			throw ParseError("Syntax error.");

	}

	inline void consumeKeyword(EASY_KEYWORD_TYPE type)
	{
		Token* token = getToken();
		if (token != nullptr && isKeyword(token) && getKeyword(token) == type)
			++index;
		else
			throw ParseError("Syntax error.");
	}

	inline void checkToken(EASY_TOKEN_TYPE type)
	{
		Token* token = getToken();
		if (token == nullptr || token->GetType() != type)
			throw ParseError("Syntax error.");

	}

	inline void checkOperator(EASY_OPERATOR_TYPE type)
	{
		Token* token = getToken();
		if (token == nullptr || token->GetType() != EASY_TOKEN_TYPE::OPERATOR || getOperator(token) != type)
			throw ParseError("Syntax error.");

	}

	inline void checkKeyword(EASY_KEYWORD_TYPE type)
	{
		Token* token = getToken();
		if (!isKeyword(token) || getKeyword(token) != type)
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
			ast = new PrimativeAst(getInteger(token));
			break;

		case EASY_TOKEN_TYPE::DOUBLE:
			ast = new PrimativeAst(getDouble(token));
			break;

		case EASY_TOKEN_TYPE::TEXT:
			ast = new PrimativeAst(getText(token));
			break;

		case EASY_TOKEN_TYPE::KEYWORD:
			if (getKeyword(token) == EASY_KEYWORD_TYPE::BOOL_TRUE)
				ast = new PrimativeAst(true);
			else if (getKeyword(token) == EASY_KEYWORD_TYPE::BOOL_FALSE)
				ast = new PrimativeAst(false);
			break;
		}

		++index;
		return AS_AST(ast);
	}

	Ast* parseReturn()
	{
		skipWhiteSpace();
		auto* ast = new ReturnAst;
		increaseAndClear();
		ast->Data = parseAst();
		return AS_AST(ast);
	}

	Ast* parseParenthesesGroup()
	{
		skipWhiteSpace();
		Ast* ast = expr();
		return AS_AST(ast);
	}

	Ast* parseAssignment()
	{
		skipWhiteSpace();
		auto* token = getToken();
		checkToken("Parse error");

		auto* ast = new AssignmentAst;
		ast->Name = getSymbol(token);

		++index;
		skipWhiteSpace();

		consumeOperator(EASY_OPERATOR_TYPE::ASSIGN);

		skipWhiteSpace();
		token = getToken();

		if (token == nullptr)
			throw ParseError("Value required");

		ast->Data = parseAst();

		return AS_AST(ast);
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

		ast->True = parseAst();
		skipWhiteSpace();
		auto* token = getToken();

		if (token != nullptr && isKeyword(token) && getKeyword(token) == EASY_KEYWORD_TYPE::ELSE)
		{
			++index;
			skipWhiteSpace();
			ast->False = parseAst();
		}

		return AS_AST(ast);
	}

	Ast* parseFunctionCall()
	{
		auto* ast = new FunctionCallAst;
		auto* token = getToken();
		checkToken("Parse error");

		ast->Function = getSymbol(token);
		++index;
		skipWhiteSpace();
		token = getToken();

		if (token == nullptr)
			throw ParseError("Function call exception");

		if (isOperator(token) && getOperator(token) == EASY_OPERATOR_TYPE::LEFT_PARENTHESES)
		{
			while (index < tokensCount)
			{
				++index;
				skipWhiteSpace();
				token = getToken();
				checkToken("Parse error");

				if (isOperator(token) && getOperator(token) == EASY_OPERATOR_TYPE::RIGHT_PARENTHESES)
					break;

				ast->Args.push_back(parseAst());
				skipWhiteSpace();
				token = getToken();
				checkToken("Parse error");

				if (isOperator(token) && getOperator(token) == EASY_OPERATOR_TYPE::RIGHT_PARENTHESES)
					break;
				else if (isOperator(token) && getOperator(token) == EASY_OPERATOR_TYPE::COMMA)
					continue;

				throw ParseError("',' required");
			}

			skipWhiteSpace();
			consumeOperator(EASY_OPERATOR_TYPE::RIGHT_PARENTHESES);
		}
		else if (isKeyword(token) && getKeyword(token) == EASY_KEYWORD_TYPE::EMPTY_PARAMETER)
		{
			//ast->Args.push_back(AS_AST(new PrimativeAst()));
			++index;
		}
		else
			ast->Args.push_back(parseAst());

		return AS_AST(ast);
	}

	Ast* parseBinaryOperationStatement(Ast* left = nullptr)
	{
		return expr();
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
			checkToken("Parse error");

			if (isKeyword(token) && getKeyword(token) == EASY_KEYWORD_TYPE::BLOCK_END)
			{
				++index;
				skipWhiteSpace();
				break;
			}

			block->Blocks->push_back(parseAst());
		}

		return AS_AST(block);
	}

	Ast* parseControlOperationStatement(Ast* left = nullptr)
	{
		auto* ast = new ControlAst;
		skipWhiteSpace();
		auto* token = getToken();

		if (left == nullptr)
		{
			if (isPrimative(token))
				ast->Left = parsePrimative(token);
			else if (token->GetType() == EASY_TOKEN_TYPE::SYMBOL)
			{
				ast->Left = new VariableAst(getSymbol(token));
				++index;
			}
		}
		else
			ast->Left = left;

		if (ast->Left == nullptr)
			throw ParseError("Binary operation left argument is empty.");

		skipWhiteSpace();
		token = getToken();
		checkToken("Parse error");

		if (isOperator(token) &&
			ControlOperators.find(getOperator(token)) != ControlOperatorsEnd)
			ast->Op = getOperator(token);
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
			ast->Right = new VariableAst(getSymbol(token));
		else if (token != nullptr && isOperator(token) && getOperator(token) == EASY_OPERATOR_TYPE::LEFT_PARENTHESES)
			ast->Right = parseParenthesesGroup();

		if (ast->Right == nullptr)
			throw ParseError("Binary operation right argument is empty.");

		return AS_AST(ast);
	}

	/*
	 func test() return 1
	 func test() { return 1 }
	 func test (value) { return 1 }
	 func test (value) { return value }
	 */
	Ast* parseFunctionDecleration()
	{
		auto* ast = new FunctionDefinetionAst;
		increaseAndClear();

		checkToken(EASY_TOKEN_TYPE::SYMBOL);
		auto* token = getToken();

		ast->Name = getSymbol(token);
		increaseAndClear();
		checkOperator(EASY_OPERATOR_TYPE::LEFT_PARENTHESES);

		while (index < tokensCount)
		{
			increaseAndClear();
			token = getToken();
			checkToken("Parse error");

			if (isOperator(token) && getOperator(token) == EASY_OPERATOR_TYPE::RIGHT_PARENTHESES)
				break;

			ast->Args.push_back(getSymbol(token));

			increaseAndClear();
			token = getToken();
			checkToken("Parse error");

			if (isOperator(token) && getOperator(token) == EASY_OPERATOR_TYPE::RIGHT_PARENTHESES)
				break;
			else if (isOperator(token) && getOperator(token) == EASY_OPERATOR_TYPE::COMMA)
				continue;

			throw ParseError("',' required");
		}

		skipWhiteSpace();
		consumeOperator(EASY_OPERATOR_TYPE::RIGHT_PARENTHESES);

		increaseAndClear();
		token = getToken();
		checkToken("Parse error");

		ast->Body = parseAst();

		return AS_AST(ast);
	}

	Ast* parseForStatement()
	{
		auto* ast = new ForStatementAst;
		skipWhiteSpace();
		auto* token = getToken();
		checkToken("Parse error");

		ast->Variable = getSymbol(token);
		skipWhiteSpace();
		++index;
		skipWhiteSpace();
		++index;
		skipWhiteSpace();
		token = getToken();
		checkToken("Parse error");

		if (token->GetType() == EASY_TOKEN_TYPE::DOUBLE)
			ast->Start = new PrimativeAst(getDouble(token));
		else if (token->GetType() == EASY_TOKEN_TYPE::INTEGER)
			ast->Start = new PrimativeAst(getInteger(token));
		else if (token->GetType() == EASY_TOKEN_TYPE::VARIABLE)
			ast->Start = new VariableAst(getVariable(token));
		else
			throw ParseError("For repeat works with variable, double and integer");
		skipWhiteSpace();
		++index;
		skipWhiteSpace();
		++index;
		skipWhiteSpace();
		token = getToken();
		checkToken("Parse error");

		if (token->GetType() == EASY_TOKEN_TYPE::DOUBLE)
			ast->End = new PrimativeAst(getDouble(token));
		else if (token->GetType() == EASY_TOKEN_TYPE::INTEGER)
			ast->End = new PrimativeAst(getInteger(token));
		else if (token->GetType() == EASY_TOKEN_TYPE::VARIABLE)
			ast->End = new VariableAst(getVariable(token));
		else
			throw ParseError("For repeat works with variable, double and integer");

		if (getNextToken() == nullptr)
			throw ParseError("Repeat block missing");

		++index;
		skipWhiteSpace();
		ast->Repeat = parseAst();


		return AS_AST(ast);
	}

	/*
	 * (10 * 1 + (data - 10 / 8) - 11)
	 * */
	Ast* parseTerm()
	{
		//auto* token = parseFactor();
		skipWhiteSpace();
		auto* tokenNext = getNextToken();

		while (index < tokensCount)
		{
			++index;
			if (isOperator(tokenNext) && (getOperator(tokenNext) == EASY_OPERATOR_TYPE::DIVISION ||
				getOperator(tokenNext) == EASY_OPERATOR_TYPE::MULTIPLICATION))
			{
				auto* ast = new BinaryAst;
				skipWhiteSpace();
				auto* token = getToken();

				if (isPrimative(token))
					ast->Left = parsePrimative(token);
				else if (token->GetType() == EASY_TOKEN_TYPE::SYMBOL)
				{
					ast->Left = new VariableAst(getSymbol(token));
					++index;
				}

				if (ast->Left == nullptr)
					throw ParseError("Binary operation left argument is empty.");

				skipWhiteSpace();
				token = getToken();
				checkToken("Parse error");

				if (isOperator(token) &&
					BinaryOperators.find(getOperator(token)) != BinaryOperatorsEnd)
					ast->Op = getOperator(token);
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
				{
					ast->Right = new VariableAst(getSymbol(token));
					++index;
				}
				else if (token != nullptr && isOperator(token) && getOperator(token) == EASY_OPERATOR_TYPE::LEFT_PARENTHESES)
					ast->Right = parseParenthesesGroup();

				if (ast->Right == nullptr)
					throw ParseError("Binary operation right argument is empty.");

			}
		}
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

		case EASY_AST_TYPE::PARENTHESES_BLOCK:
			ast = parseParenthesesGroup();
			break;

		case EASY_AST_TYPE::IF_STATEMENT:
			ast = parseIfStatement();
			break;


		case EASY_AST_TYPE::RETURN:
			ast = parseReturn();
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
			checkToken("Parse error");
			variableAst->Value = getVariable();
			ast = AS_AST(variableAst);
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

		current_token = getToken();
		next_token = getNextToken(EASY_TOKEN_TYPE::WHITESPACE);

		while (index < tokensCount)
			asts->push_back(parseAst());
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

	/* TEMPORARY */
	Ast* asPrimative(Token* token)
	{
		PrimativeAst* ast = nullptr;

		switch (token->GetType())
		{
		case EASY_TOKEN_TYPE::INTEGER:
			ast = new PrimativeAst(getInteger(token));
			break;

		case EASY_TOKEN_TYPE::DOUBLE:
			ast = new PrimativeAst(getDouble(token));
			break;

		case EASY_TOKEN_TYPE::TEXT:
			ast = new PrimativeAst(getText(token));
			break;

		case EASY_TOKEN_TYPE::KEYWORD:
			if (getKeyword(token) == EASY_KEYWORD_TYPE::BOOL_TRUE)
				ast = new PrimativeAst(true);
			else if (getKeyword(token) == EASY_KEYWORD_TYPE::BOOL_FALSE)
				ast = new PrimativeAst(false);
			break;
		}

		return AS_AST(ast);
	}

	Token* eat(EASY_OPERATOR_TYPE opt)
	{
		increaseAndClear();

		if (getToken() != nullptr && getToken()->GetType() == EASY_TOKEN_TYPE::WHITESPACE)
			increaseAndClear();
		
		if (isOperator(current_token) && getOperator(current_token) == opt)
			return current_token;

		throw ParseError(std::string(EASY_OPERATOR_TYPEToString(opt)) + " Required");
	}

	inline bool isTerm(Token* token)
	{
		return getOperator(token) == EASY_OPERATOR_TYPE::MULTIPLICATION ||
			getOperator(token) == EASY_OPERATOR_TYPE::DIVISION;
	}

	inline bool isExpr(Token* token)
	{
		return getOperator(token) == EASY_OPERATOR_TYPE::PLUS ||
			getOperator(token) == EASY_OPERATOR_TYPE::MINUS;
	}

    Ast* factor()
    {
        auto* token = getToken();

		if (isPrimative(token))
		{
			increaseAndClear();
			return asPrimative(token);
		}
		else if (isSymbol(token))
		{
			increaseAndClear();
			return new VariableAst(getSymbol(token));
		}

        if (isOperator(token) && getOperator() == EASY_OPERATOR_TYPE::LEFT_PARENTHESES) {
			increaseAndClear();
            Ast* ast = expr();
            eat(EASY_OPERATOR_TYPE::RIGHT_PARENTHESES);
            return ast;
        }

        throw ParseError("Problem with parse");
    }

    Ast* term()
    {
        Ast* ast = factor();

        while (current_token != nullptr && next_token != nullptr)
        {
            if (isOperator(next_token) && isTerm(next_token))
            {
                auto* binary = new BinaryAst();
                binary->Left = ast;
                eat(getOperator(next_token));
                binary->Op = getOperator(current_token);
                binary->Right = factor();
                ast = AS_AST(binary);
            }
            else if (isOperator(current_token) && isTerm(current_token))
            {
                auto* binary = new BinaryAst();
                binary->Left = ast;
                binary->Op = getOperator(current_token);
                binary->Right = factor();
                ast = AS_AST(binary);
            }
            else
                break;
        }

        return ast;
    }

	Ast* expr()
	{
		auto* ast = term();

		if (current_token == nullptr)
			return ast;

		while (current_token != nullptr && next_token != nullptr) {
			if (isOperator(next_token) && isExpr(next_token)) {
				auto *binary = new BinaryAst();
				binary->Left = ast;
				eat(getOperator(next_token));
				binary->Op = getOperator(current_token);
				binary->Right = term();
				ast = AS_AST(binary);
			} 
			else if (isOperator(current_token) && isExpr(current_token)) {
				auto *binary = new BinaryAst();
				binary->Left = ast;
				binary->Op = getOperator(current_token);
				binary->Right = term();
				ast = AS_AST(binary);
			} else
				break;
		}
	
		return ast;
	}

	void tempParse()
	{
		tokensCount = tokens->size();
		index = 0;

        current_token = getToken();
        next_token = getNextToken(EASY_TOKEN_TYPE::WHITESPACE);

		asts->push_back(expr());
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


void AstParser::TempParse(std::shared_ptr<std::vector<Token*>> tokens, std::shared_ptr<std::vector<Ast*>> asts)
{
	asts->clear();

	impl->tokens = tokens;
	impl->asts = asts;
	impl->tempParse();
}

void AstParser::Dump(std::shared_ptr<std::vector<Ast*>> asts)
{
	impl->dump(asts);
}
