#ifndef EASYLANG_INTERPRETERTESTS_H
#define EASYLANG_INTERPRETERTESTS_H

#define CATCH_CONFIG_RUNNER
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>

#include "../Catch.h"
#include "../Lexer.h"
#include "../ASTs.h"
#include "../InterpreterBackend.h"

using namespace std;

#define PREPARE(text)\
    tokinizer->Parse( L##text , tokens);\
    astParser->Parse(tokens, asts);\
    backend->Prepare(asts);

TEST_CASE("Interpreter tests") {
    Tokinizer* tokinizer = new StandartTokinizer();
    AstParser* astParser = new AstParser();
    InterpreterBackend* backend = new InterpreterBackend();
    PrimativeValue* value = nullptr;
    
    std::shared_ptr<std::vector<Token* > > tokens = make_shared<std::vector<Token* > >();
    std::shared_ptr<std::vector<Ast* > > asts = make_shared<std::vector<Ast* > >();
    
    SECTION("if 100 > 15 then print 123") {
        PREPARE("if 100 > 15 then print 123");
        
        //value = backend->getData(asts->at(0));
        //REQUIRE(value != nullptr);
        //REQUIRE(value->IsInteger());
        //REQUIRE(value->Integer == 123);
    }
}

#endif //EASYLANG_INTERPRETERTESTS_H
