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
	virtual void Execute() = 0;
};

class InterpreterBackend : public Backend {
public:
	void Prepare(std::shared_ptr<std::vector<Ast*>> pAsts) override
	{
		asts = pAsts;
	}

	void Execute() override
	{
		auto astsEnd = asts->cend();

		for (auto it = asts->cbegin(); astsEnd != it; ++it)
		{
			switch ((*it)->GetType())
			{
			case AstType::ASSIGNMENT:
			{
				AssignmentAst* assignment = reinterpret_cast<AssignmentAst*>(*it);
				//variables[assignment->Name] = assignment->Data
			}
				break;

			default:
				break;
			}
		}
	}



private:
	std::shared_ptr<std::vector<Ast*>> asts;
	std::unordered_map<std::string, PrimativeValue*> variables;
};

template <class T = Backend>
class BackendExecuter {
public:
	static_assert(std::is_base_of<Backend, T>::value, "T must be derived from Backend");
	void Prepare(std::shared_ptr<std::vector<Ast*>> asts)
	{
		Backend* asd = new T;
		asd->Prepare(asts);
	}
};

class tester
{
public:
	void test()
	{
		BackendExecuter<InterpreterBackend> executer;
	}
};
#endif