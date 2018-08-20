#include "EasyEngine.h"
#include "InterpreterBackend.h"
#include "VmBackend.h"
#include "LLVMBackend.h"
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

    void Execute(std::vector<char_type> const & opcodes)
    {
        backend->Execute(opcodes);
    }

    void Compile(string_type const & code, std::vector<char_type> & opcodes)
    {
        tokinizer->Parse(code, tokens);
        astParser->Parse(tokens, asts);
        backend->Prepare(asts);
        backend->Compile(opcodes);

        size_t totalToken = tokens->size();
        for (size_t i = 0; i < totalToken; ++i)
            delete tokens->at(i);
        
        tokens.reset();
/*
        size_t totalAsts = asts->size();
        for (size_t i = 0; i < totalAsts; ++i)
            delete asts->at(i);*/

        asts.reset();
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
void EasyEngine<TTokinizer, TAstParser, TBackend>::Execute(std::vector<char_type> const & opcodes)
{
    impl->Execute(opcodes);
}

template<class TTokinizer, class TAstParser, class TBackend>
PrimativeValue* EasyEngine<TTokinizer, TAstParser, TBackend>::Execute(string_type const & code)
{
    return impl->Execute(code);
}

template<class TTokinizer, class TAstParser, class TBackend>
void EasyEngine<TTokinizer, TAstParser, TBackend>::Compile(string_type const & code, std::vector<char_type> & opcodes)
{
    impl->Compile(code, opcodes);
}

template class EasyEngine<StandartTokinizer, AstParser, VmBackend>;
template class EasyEngine<StandartTokinizer, AstParser, InterpreterBackend>;
#if defined(EASYLANG_JIT_ACTIVE)
template class EasyEngine<StandartTokinizer, AstParser, LLVMBackend>;
#endif