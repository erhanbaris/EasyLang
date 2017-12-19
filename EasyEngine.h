#ifndef EASYLANG_EASYENGINE_H
#define EASYLANG_EASYENGINE_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "PrimativeValue.h"
#include "Backend.h"

class EasyEngineImpl;
class EasyEngine {
public:
	static EasyEngine * Interpreter();
	PrimativeValue* Execute(std::wstring const & code);
	~EasyEngine();

private:
	EasyEngine(Backend * backend);
	EasyEngineImpl* impl;
};

#endif //EASYLANG_EASYENGINE_H
