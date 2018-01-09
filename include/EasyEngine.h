#ifndef EASYLANG_EASYENGINE_H
#define EASYLANG_EASYENGINE_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "PrimativeValue.h"
#include "Backend.h"
#include "Definitions.h"

class EasyEngineImpl;

template<class TTokinizer, class TAstParser, class TBackend>
class EasyEngine {
public:
	static_assert(std::is_base_of<Tokinizer, TTokinizer>::value, "T must be derived from Tokinizer");
	static_assert(std::is_base_of<AstParser, TAstParser>::value, "T must be derived from AstParser");
	static_assert(std::is_base_of<Backend, TBackend>::value, "T must be derived from Backend");
	
	EasyEngine();
	PrimativeValue* Execute(string_type const & code);
	void Execute(std::vector<size_t> const & opcodes);
	void Compile(string_type const & code, std::vector<size_t> & opcodes);
	~EasyEngine();

private:
	EasyEngineImpl* impl;
};

#endif //EASYLANG_EASYENGINE_H
