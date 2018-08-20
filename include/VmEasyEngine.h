#ifndef EASYLANG_VMEASYENGINE_H
#define EASYLANG_VMEASYENGINE_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "PrimativeValue.h"
#include "Backend.h"
#include "Definitions.h"
#include "VmBackend.h"
#include "ASTs.h"
#include "Lexer.h"
#include "EasyEngine.h"

class VmEasyEngine: public EasyEngine<StandartTokinizer, AstParser, VmBackend> {
public:
    VmEasyEngine() : EasyEngine()
    {

    }
};

#endif //EASYLANG_VMEASYENGINE_H
