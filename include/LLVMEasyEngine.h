#ifndef EASYLANG_LLVMEASYENGINE_H
#define EASYLANG_LLVMEASYENGINE_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "PrimativeValue.h"
#include "Backend.h"
#include "Definitions.h"
#include "LLVMBackend.h"
#include "ASTs.h"
#include "Lexer.h"
#include "EasyEngine.h"

class LLVMEasyEngine: public EasyEngine<StandartTokinizer, AstParser, LLVMBackend> {
public:
	LLVMEasyEngine() : EasyEngine()
	{

	}
};

#endif //EASYLANG_LLVMEASYENGINE_H
