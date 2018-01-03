#include "ASTs.h"
#include "Exceptions.h"
#include "System.h"
#include "PrimativeValue.h"

#define GET_ITEM(NAME, TYPE, IN, OUT) inline OUT get##NAME (Token* token)\
	{\
	return static_cast<IN*>(token)->Value;\
	}\
	inline OUT get##NAME ()\
	{\
		return static_cast<IN*>(peek())->Value;\
	}\
	inline bool is##NAME (Token* token)\
	{\
		return token != nullptr && token->GetType() == TYPE ;\
	}\
	inline bool is##NAME ()\
	{\
		return peek() != nullptr && peek()->GetType() == TYPE ;\
	}
#define AS_EXPR(ast) static_cast<ExprAst*>(ast)
#define AS_STMT(ast) static_cast<StmtAst*>(ast)
#define AS_AST(ast) static_cast<Ast*>(ast)
#define AS_TOKEN(token) static_cast<Token*>(token)

class AstParserImpl
{
public:
	std::shared_ptr<std::vector<Token*>> tokens;
	std::shared_ptr<std::vector<Ast*>> asts;
	std::unordered_map<string_type, MethodCallback> userMethods;

	size_t tokensCount;
	size_t index;

	GET_ITEM(Keyword, EASY_TOKEN_TYPE::KEYWORD, KeywordToken, EASY_KEYWORD_TYPE);
	GET_ITEM(Operator, EASY_TOKEN_TYPE::OPERATOR, OperatorToken, EASY_OPERATOR_TYPE);
	GET_ITEM(Integer, EASY_TOKEN_TYPE::INTEGER, IntegerToken, int);
	GET_ITEM(Double, EASY_TOKEN_TYPE::DOUBLE, DoubleToken, double);
	GET_ITEM(Text, EASY_TOKEN_TYPE::TEXT, TextToken, string_type);
	GET_ITEM(Symbol, EASY_TOKEN_TYPE::SYMBOL, SymbolToken, string_type);
	GET_ITEM(Variable, EASY_TOKEN_TYPE::VARIABLE, VariableToken, string_type);
    
    inline bool isPrimative(Token * token)
    {
        return token != nullptr && (isInteger(token) ||
                                    isText(token) ||
                                    isDouble(token) ||
                                    (isKeyword(token) && getKeyword(token) == EASY_KEYWORD_TYPE::BOOL_TRUE) ||
                                    (isKeyword(token) && getKeyword(token) == EASY_KEYWORD_TYPE::BOOL_FALSE));
    }
    
    inline Token* advance() {
        if (!isAtEnd())
            ++index;
        
        return previous();
    }
    
    inline bool isAtEnd() {
        return peek()->GetType() == EASY_TOKEN_TYPE::END_OF_FILE;
    }
    
    inline Token* peek() {
        return tokens->at(index);
    }
    
    inline Token* previous() {
        return tokens->at(index - 1);
    }

	void dump(std::shared_ptr<std::vector<Ast*>> asts)
	{
        string_stream buffer;
        buffer << _T("digraph AST {");
		console_out << std::endl << std::endl << _T("### AST TREE ###") << std::endl << std::endl;

		auto astsEnd = asts->end();
		for (auto it = asts->begin(); it != astsEnd; ++it) {
			dumpLevel(*it, _T("main"), buffer);
		}
        
        buffer << _T("}\n\n");
        
		console_out << buffer.str();
	}

	void levelPadding(int level)
	{
		for (int i = 0; i < level; ++i)
			std::cout << _T("  ");
	}

    int dumpIndexer = 1;
    string_type getDumpIndex()
    {
        return _T("NODE") + AS_STRING(++dumpIndexer);
    }
    
    void dumpLevel(Ast* ast, string_type const & main, string_stream & buffer)
	{
		if (ast == nullptr)
		{
			return;
		}
        
        string_type name;
		switch (ast->GetType())
		{
		case EASY_AST_TYPE::IF_STATEMENT:
		{
			auto* ifStatement = static_cast<IfStatementAst*>(ast);
            name = getDumpIndex();
            
            buffer << _T("subgraph ") << getDumpIndex() << _T(" {\n");
			dumpLevel(ifStatement->ControlOpt,  name, buffer);
            buffer << _T("}\n");
            
            buffer << name << _T("[label=\"IF STATEMENT") << _T("\"]\n");
            buffer << main << _T(" -> " << name << _T(";\n");
            
			dumpLevel(ifStatement->True, name, buffer);
			dumpLevel(ifStatement->False, name,buffer);
		}
		break;

		case EASY_AST_TYPE::ASSIGNMENT: {
			auto *assignment = static_cast<AssignmentAst *>(ast);
            name = getDumpIndex();
            buffer << name << _T("[label=\"") << assignment->Name << _T("\"]\n");
            buffer << main << _T(" -> ") << name << _T(";\n");
            
			dumpLevel(assignment->Data, name, buffer);
		}
                break;

		case EASY_AST_TYPE::VARIABLE: {
			auto *variable = static_cast<VariableAst *>(ast);
            name = getDumpIndex();
            buffer << main << _T(" -> ") << name << _T(";\n");
		}
									  break;

		case EASY_AST_TYPE::PRIMATIVE: {
			auto *primative = static_cast<PrimativeAst *>(ast);
            name = getDumpIndex();
            
			switch (primative->Value->Type) {
			case PrimativeValue::Type::PRI_INTEGER:
                buffer << name << _T("[label=\"") << primative->Value->Integer << _T("\"]\n");
				buffer << main << _T(" -> ") << name << _T(";\n");
				break;

			case PrimativeValue::Type::PRI_DOUBLE:
                buffer << name << _T("[label=\"") << primative->Value->Double << "\"]\n");
				buffer << main << _T(" -> ") << name << _T(";\n");
				break;

			case PrimativeValue::Type::PRI_STRING:
                buffer << name << _T("[label=\"") << primative->Value->String << _T("\"]\n");
				buffer << main << _T(" -> ") << name << _T(";");
				break;

			case PrimativeValue::Type::PRI_BOOL:
                buffer << name << _T("[label=\"") << primative->Value->Bool << _T("\"]\n");
                buffer << main << _T(" -> ") << name << _T(";\n");
				break;
			}
		}
									   break;

		case EASY_AST_TYPE::BINARY_OPERATION: {
			auto *binary = static_cast<BinaryAst *>(ast);
            auto op = getDumpIndex();
            
            buffer << op << _T("[label=\"") << EASY_OPERATOR_TYPEToString(binary->Op) << _T("\"]\n");
            buffer << main << _T(" -> ") << op << _T(";\n");
            
            dumpLevel(binary->Left, op, buffer);
            dumpLevel(binary->Right, op, buffer);
		}
											  break;

		case EASY_AST_TYPE::CONTROL_OPERATION: {
			auto *binary = static_cast<ControlAst *>(ast);
            auto op = getDumpIndex();
            
            buffer << op << _T("[label=\"") << EASY_OPERATOR_TYPEToString(binary->Op) << _T("\"]\n");
            buffer << main << _T(" -> ") << op << _T(";\n");
            
            dumpLevel(binary->Left, op, buffer);
            dumpLevel(binary->Right, op, buffer);
		}
											   break;

		case EASY_AST_TYPE::FUNCTION_CALL:
		{
			auto* functionCall = static_cast<FunctionCallAst*>(ast);
			console_out << _T("#METHOD CALL : ") << functionCall->Function << _T(" ");
			auto astsEnd = functionCall->Args.end();
			for (auto it = functionCall->Args.begin(); it != astsEnd; ++it)
			{
				//dumpLevel(*it, buffer);
				console_out << " ";
			}
		}
		break;

		case EASY_AST_TYPE::BLOCK:
		{
			auto* block = static_cast<BlockAst*>(ast);
			auto blockEnd = block->Blocks->cend();

			for (auto it = block->Blocks->cbegin(); it != blockEnd; ++it)
			{
				//dumpLevel(*it, buffer);
			}
		}
		break;
		}
	}

	/* TEMPORARY */
	ExprAst* asPrimative(Token* token)
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

		return ast;
	}

	ExprAst* primaryExpr()
	{
        if (isPrimative(peek()))
        {
            advance();
            return asPrimative(previous());
        }
        
        if (match({EASY_OPERATOR_TYPE::LEFT_PARENTHESES}))
		{
			ExprAst* expr = expression();
			match({ EASY_OPERATOR_TYPE::RIGHT_PARENTHESES });
			return expr;
		}
        
        if (match({EASY_OPERATOR_TYPE::SQUARE_BRACKET_START}))
        {
            std::vector<PrimativeValue*>* args = new std::vector<PrimativeValue*>;
            if (!check(EASY_OPERATOR_TYPE::SQUARE_BRACKET_END)) {
                do {
                    ExprAst* item = primaryExpr();
                    
                    if (item->GetType() != EASY_AST_TYPE::PRIMATIVE)
                        throw ParseError(_T("Array init must be contain primative value"));
                    
                    args->push_back(static_cast<PrimativeAst*>(item)->Value);
                    
                } while (match({EASY_OPERATOR_TYPE::COMMA}));
            }
            
			match({ EASY_OPERATOR_TYPE::SQUARE_BRACKET_END });
            return new PrimativeAst(args);
        }

        if (isSymbol(peek()))
        {
            advance();
            return new VariableAst(getSymbol(previous()));
        }
		
        if (isVariable(peek()))
        {
            advance();
            return new VariableAst(getVariable(previous()));
        }
        
        Token* token = peek();
        throw ParseError(_T("Expect expression."));
	}

    ExprAst* finishCallExpr(string_type const & package, string_type const & function) {
        std::vector<ExprAst*> arguments;
		if (previous()->GetType() == EASY_TOKEN_TYPE::OPERATOR && static_cast<OperatorToken*>(previous())->Value == EASY_OPERATOR_TYPE::UNDERLINE)
			return new FunctionCallAst(package, function, arguments);

        if (!check(EASY_OPERATOR_TYPE::RIGHT_PARENTHESES)) {
            do {
                arguments.push_back(expression());
            } while (match({EASY_OPERATOR_TYPE::COMMA}));
        }

        consume(EASY_OPERATOR_TYPE::RIGHT_PARENTHESES, _T("Expect ')' after arguments."));
        return new FunctionCallAst(package, function, arguments);
    }


	ExprAst* call()
	{
		ExprAst* expr = primaryExpr();
        
        while(true)
        {
            if (match({EASY_OPERATOR_TYPE::LEFT_PARENTHESES, EASY_OPERATOR_TYPE::UNDERLINE }))
            {
				string_type function = static_cast<VariableAst*>(expr)->Value;
                expr = finishCallExpr(_T(""), function);
            }
            else if (match({EASY_OPERATOR_TYPE::DOUBLE_COLON}))
            {
                string_type package = static_cast<VariableAst*>(expr)->Value;
                string_type function = getSymbol(advance());
                advance();
                
                expr = finishCallExpr(package, function);
            }
            else
                break;
        }
        
        return expr;
	}

	ExprAst* unaryExpr()
	{
		if (match({ EASY_OPERATOR_TYPE ::MINUS})){
            EASY_OPERATOR_TYPE opt = getOperator(previous());
			ExprAst* right = unaryExpr();
            return new UnaryAst(opt, right);
		}

		return call();
	}

	ExprAst* multiplicationExpr()
	{
		ExprAst* expr = unaryExpr();
        
        while (match({ EASY_OPERATOR_TYPE ::DIVISION, EASY_OPERATOR_TYPE::MULTIPLICATION })) {
            EASY_OPERATOR_TYPE opt = getOperator(previous());
            ExprAst* right = unaryExpr();
            expr = new BinaryAst(expr, opt, right);
        }
        
        return expr;
	}

	ExprAst* additionExpr()
	{
		ExprAst* expr = multiplicationExpr();
        
        while (match({ EASY_OPERATOR_TYPE::PLUS, EASY_OPERATOR_TYPE::MINUS })) {
            EASY_OPERATOR_TYPE opt = getOperator(previous());
            ExprAst* right = multiplicationExpr();
            expr = new BinaryAst(expr, opt, right);
        }
        
		return expr;
	}

	ExprAst* comparisonExpr()
	{
		ExprAst* expr = additionExpr();

		while (match({ EASY_OPERATOR_TYPE::GREATOR, EASY_OPERATOR_TYPE::GREATOR_EQUAL, EASY_OPERATOR_TYPE::LOWER , EASY_OPERATOR_TYPE::LOWER_EQUAL })) {
			EASY_OPERATOR_TYPE opt = getOperator(previous());
			ExprAst* right = additionExpr();
			expr = new ControlAst(expr, opt, right);
		}

		return expr;
	}

	ExprAst* equalityExpr()
	{
		ExprAst* expr = comparisonExpr();

		while (match({ EASY_OPERATOR_TYPE::EQUAL, EASY_OPERATOR_TYPE::NOT_EQUAL })) {
			EASY_OPERATOR_TYPE opt = getOperator(previous());
			ExprAst* right = comparisonExpr();
			expr = new ControlAst(expr, opt, right);
		}

		return expr;
	}

	ExprAst* andExpr()
	{
		ExprAst* expr = equalityExpr();

		while (match({ EASY_OPERATOR_TYPE::AND })) {
			EASY_OPERATOR_TYPE opt = getOperator(previous());
			ExprAst* right = equalityExpr();
			expr = new ControlAst(expr, opt, right);
		}

		return expr;
	}

	ExprAst* orExpr()
	{
		ExprAst* expr = andExpr();

		while (match({ EASY_OPERATOR_TYPE::OR })) {
			EASY_OPERATOR_TYPE opt = getOperator(previous());
			ExprAst* right = andExpr();
			expr = new ControlAst(expr, opt, right);
		}

		return expr;
	}

	ExprAst* assignmentExpr()
	{
		ExprAst* expr = orExpr();
		if (match({ EASY_OPERATOR_TYPE::ASSIGN }))
		{
			ExprAst* value = orExpr();
			if (expr->GetType() == EASY_AST_TYPE::VARIABLE)
				return new AssignmentAst(static_cast<VariableAst*>(expr)->Value, value);

			throw ParseError(_T("Invalid assignment"));
		}
        
        if (match({ EASY_OPERATOR_TYPE::APPEND }))
        {
            EASY_OPERATOR_TYPE opt = static_cast<OperatorToken*>(previous())->Value;
            ExprAst* value = orExpr();
            if (expr->GetType() == EASY_AST_TYPE::VARIABLE)
                return new BinaryAst(expr, opt, value);
            
            throw ParseError(_T("Invalid append"));
        }
        

		return expr;
	}

	ExprAst* expression()
	{
		return assignmentExpr();
	}

	StmtAst* expressionStmt()
	{
		ExprAst* expr = expression();
		return new ExprStatementAst(expr);
	}

	StmtAst* returnStmt()
	{
		ExprAst* returnData = expression();
		return new ReturnAst(returnData);
	}

    StmtAst* ifStmt()
    {
        ExprAst* condition = orExpr();
        if (condition == nullptr)
            throw ParseError(_T("If condition required"));
        
        consume(EASY_KEYWORD_TYPE::THEN, _T("'then' keyword required"));
        
        StmtAst* trueStmt = nullptr;
        StmtAst* falseStmt = nullptr;
        
		if (match({ EASY_OPERATOR_TYPE::BLOCK_START }))
            trueStmt = block();
        else if (check(EASY_KEYWORD_TYPE::RETURN))
        {
            advance();
            trueStmt = returnStmt();
        }
        else
            trueStmt = expressionStmt();
        
        if (check(EASY_KEYWORD_TYPE::ELSE))
        {
            advance();
			if (match({ EASY_OPERATOR_TYPE::BLOCK_START }))
                falseStmt = block();
            else if (check(EASY_KEYWORD_TYPE::RETURN))
            {
                advance();
                falseStmt = returnStmt();
            }
            else
                falseStmt = expressionStmt();
        }
        
        return new IfStatementAst(condition, trueStmt, falseStmt);
    }
    
	StmtAst* forStmt()
	{
		ExprAst* parameter = primaryExpr();
		if (parameter == nullptr || parameter->GetType() != EASY_AST_TYPE::VARIABLE)
			throw ParseError(_T("For iterator variable required"));

		consume(EASY_KEYWORD_TYPE::IN_KEYWORD, _T("'in' keyword required"));

		ExprAst* startExpr = orExpr();
		if (startExpr == nullptr)
			throw ParseError(_T("For start item required"));

		consume(EASY_KEYWORD_TYPE::TO_KEYWORD, _T("'to' keyword required"));

		ExprAst* endExpr = orExpr();
		if (startExpr == nullptr)
			throw ParseError(_T("For to item required"));

		consume(EASY_KEYWORD_TYPE::THEN, _T("'then' keyword required"));

		StmtAst* body = nullptr;
		if (match({ EASY_OPERATOR_TYPE::BLOCK_START }))
			body = block();
		else
			body = expressionStmt();

        return new ForStatementAst(static_cast<VariableAst*>(parameter)->Value, startExpr, endExpr, body);
	}
                                 
                                 
                                 
	StmtAst* functionStmt()
	{
		Token* funcName = consume(EASY_TOKEN_TYPE::SYMBOL, _T("Function name required"));
		consume(EASY_OPERATOR_TYPE::LEFT_PARENTHESES, _T("'(' required"));

		std::vector<string_type> args;
		if (!check(EASY_OPERATOR_TYPE::RIGHT_PARENTHESES))
			do {
				Token* arg = consume(EASY_TOKEN_TYPE::SYMBOL, _T("Only string variable name allowed"));
				args.push_back(static_cast<SymbolToken*>(arg)->Value);
			} while (match({ EASY_OPERATOR_TYPE::COMMA }));

			StmtAst* body = nullptr;
			consume(EASY_OPERATOR_TYPE::RIGHT_PARENTHESES, _T("')' required"));
			if (match({ EASY_OPERATOR_TYPE::BLOCK_START }))
				body = block();
			else {
				consume(EASY_KEYWORD_TYPE::RETURN, _T("'return' keyword required"));
				body = returnStmt();
			}

		return new FunctionDefinetionAst(static_cast<SymbolToken*>(funcName)->Value, args, body);
	}

	StmtAst* block() {
		BlockAst* block = new BlockAst;
		std::vector<StmtAst*> statements;
		while (!check(EASY_OPERATOR_TYPE::BLOCK_END) && !isAtEnd()) {
			block->Blocks->push_back(declarationStmt());
		}

		consume(EASY_OPERATOR_TYPE::BLOCK_END, _T("'}' required"));
		return block;
	}

	StmtAst* declarationStmt()
	{
		if (match({ EASY_KEYWORD_TYPE::FUNC }))
			return functionStmt();
		else if (match({ EASY_KEYWORD_TYPE::RETURN }))
			return returnStmt();
        else if (match({EASY_OPERATOR_TYPE::BLOCK_START}))
            return block();
        else if (match({EASY_KEYWORD_TYPE::FOR}))
            return forStmt();
        else if (match({EASY_KEYWORD_TYPE::IF}))
            return ifStmt();

		return expressionStmt();
	}


	void parse()
	{
		tokensCount = tokens->size();
		index = 0;
		while (!isAtEnd())
			asts->push_back(declarationStmt());
	}

	inline bool check(EASY_TOKEN_TYPE type)
	{
		Token* token = peek();
		return token != nullptr && token->GetType() == type;
	}

	inline bool check(EASY_OPERATOR_TYPE type)
	{
		Token* token = peek();
		return token != nullptr && token->GetType() == EASY_TOKEN_TYPE::OPERATOR && static_cast<OperatorToken*>(token)->Value == type;
	}

	inline bool check(EASY_KEYWORD_TYPE type)
	{
		Token* token = peek();
		return token != nullptr && token->GetType() == EASY_TOKEN_TYPE::KEYWORD && static_cast<KeywordToken*>(token)->Value == type;
	}

	bool match(std::initializer_list<EASY_TOKEN_TYPE> types)
	{
		for (EASY_TOKEN_TYPE type : types) {
			if (check(type)) {
				advance();
				return true;
			}
		}
		return false;
	}

	bool match(std::initializer_list<EASY_OPERATOR_TYPE> types)
	{
		for (EASY_OPERATOR_TYPE type : types) {
			if (check(type)) {
				advance();
				return true;
			}
		}
		return false;
	}

	bool match(std::initializer_list<EASY_KEYWORD_TYPE> types)
	{
		for (EASY_KEYWORD_TYPE type : types) {
			if (check(type)) {
				advance();
				return true;
			}
		}
		return false;
	}

    Token* consume(EASY_OPERATOR_TYPE type, string_type const & message) {
        if (check(type)) return advance();

        throw ParseError(message);
    }

	Token* consume(EASY_TOKEN_TYPE type, string_type const & message) {
		if (check(type)) return advance();

		throw ParseError(message);
	}

	Token* consume(EASY_KEYWORD_TYPE type, string_type const & message) {
		if (check(type)) return advance();

		throw ParseError(message);
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
