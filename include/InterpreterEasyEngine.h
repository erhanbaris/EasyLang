#ifndef EASYLANG_INTERPRETEREASYENGINE_H
#define EASYLANG_INTERPRETEREASYENGINE_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "PrimativeValue.h"
#include "Backend.h"
#include "Definitions.h"
#include "InterpreterBackend.h"
#include "ASTs.h"
#include "Lexer.h"
#include "EasyEngine.h"

class InterpreterEasyEngine : public EasyEngine<StandartTokinizer, AstParser, InterpreterBackend> {
public:
};

#endif //EASYLANG_INTERPRETEREASYENGINE_H
