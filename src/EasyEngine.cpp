#include "EasyEngine.h"
#include "InterpreterBackend.h"
#include "VmBackend.h"
#include "Lexer.h"
#include "ASTs.h"
#include "Backend.h"

#include <memory>

class EasyEngineImpl {
public:
	Backend * backend { nullptr };
	Tokinizer* tokinizer { nullptr };
	AstParser* astParser { nullptr };

	std::shared_ptr<std::vector<Token*>> tokens;
	std::shared_ptr<std::vector<Ast*>> asts;

	EasyEngineImpl(Tokinizer* pTokinizer, AstParser* pAstParser, Backend* pBackend)
	{
		backend = pBackend;

		tokinizer = pTokinizer;
		astParser = pAstParser;
		tokens = std::make_shared<std::vector<Token*>>();
		asts = std::make_shared<std::vector<Ast*>>();
	}

	PrimativeValue* Execute(string_type const & code)
	{
		tokinizer->Parse(code, tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		return backend->Execute();
	}

	void Execute(std::vector<size_t> const & opcodes)
	{
		backend->Execute(opcodes);
	}

	void Compile(string_type const & code, std::vector<size_t> & opcodes)
	{
		tokinizer->Parse(code, tokens);
		astParser->Parse(tokens, asts);
		backend->Prepare(asts);
		backend->Compile(opcodes);
	}

	~EasyEngineImpl()
	{
		delete backend;
		delete tokinizer;
		delete astParser;
	}
};

template<class TTokinizer, class TAstParser, class TBackend>
EasyEngine<TTokinizer, TAstParser, TBackend>::EasyEngine()
{
	impl = new EasyEngineImpl(new TTokinizer, new TAstParser, new TBackend);
}

template<class TTokinizer, class TAstParser, class TBackend>
EasyEngine<TTokinizer, TAstParser, TBackend>::~EasyEngine()
{
	delete impl;
}

template<class TTokinizer, class TAstParser, class TBackend>
void EasyEngine<TTokinizer, TAstParser, TBackend>::Execute(std::vector<char> const & opcodes)
{
	impl->Execute(opcodes);
}

template<class TTokinizer, class TAstParser, class TBackend>
PrimativeValue* EasyEngine<TTokinizer, TAstParser, TBackend>::Execute(string_type const & code)
{
	return impl->Execute(code);
}

template<class TTokinizer, class TAstParser, class TBackend>
void EasyEngine<TTokinizer, TAstParser, TBackend>::Compile(string_type const & code, std::vector<char> & opcodes)
{
	impl->Compile(code, opcodes);
}

template class EasyEngine<StandartTokinizer, AstParser, VmBackend>;
template class EasyEngine<StandartTokinizer, AstParser, InterpreterBackend>;