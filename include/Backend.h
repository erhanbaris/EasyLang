#ifndef EASYLANG_BACKEND_H
#define EASYLANG_BACKEND_H

#include <type_traits>

#include <vector>
#include <memory>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <cmath>
#include <unordered_map>

#include "ASTs.h"

class Backend {
public:
    virtual void Prepare(std::shared_ptr<std::vector<Ast*>> asts) = 0;
    virtual PrimativeValue* Execute() = 0;
	virtual void Execute(std::vector<size_t> const & opcodes) = 0;
	virtual void Compile(std::vector<size_t> & opcodes) = 0;
};

template <class T>
class BackendExecuter {
public:
    static_assert(std::is_base_of<Backend, T>::value, "T must be derived from Backend");
    BackendExecuter() {
        backend = new T;
    }
    
    PrimativeValue* Prepare(std::shared_ptr<std::vector<Ast*>> asts)
    {
        backend->Prepare(asts);
        return backend->Execute();
    }
    
    
private:
    Backend* backend;
};

#endif
