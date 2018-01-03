#include "EasyEngine.h"
#include "InterpreterBackend.h"
#include "Lexer.h"
#include "ASTs.h"

#include <memory>

class EasyEngineImpl {
public:
	Backend * backend { nullptr };
	Tokinizer* tokinizer { nullptr };
	AstParser* astParser { nullptr };

	std::shared_ptr<std::vector<Token*>> tokens;
	std::shared_ptr<std::vector<Ast*>> asts;

	EasyEngineImpl(Backend * pBackend)
	{
		backend = pBackend;

		tokinizer = new StandartTokinizer();
		astParser = new AstParser;
		BackendExecuter<InterpreterBackend> executer;
		tokens = std::make_shared<std::vector<Token*>>();
		asts = std::make_shared<std::vector<Ast*>>();
	}

	PrimativeValue* Execute(string_type const & code)
	{
		tokinizer->Parse(code, tokens);
        //tokinizer->Dump(tokens);
		astParser->Parse(tokens, asts);
        //astParser->Dump(asts);
		backend->Prepare(asts);
		return backend->Execute();
	}

	~EasyEngineImpl()
	{
		delete backend;
		delete tokinizer;
		delete astParser;
	}
};

EasyEngine * EasyEngine::Interpreter()
{
	return new EasyEngine(new InterpreterBackend);
}

EasyEngine::EasyEngine(Backend * backend)
{
	impl = new EasyEngineImpl(backend);
}

EasyEngine::~EasyEngine()
{
	delete impl;
}

PrimativeValue* EasyEngine::Execute(string_type const & code)
{
	return impl->Execute(code);
}
