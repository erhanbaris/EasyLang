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

	PrimativeValue getPrimative(Ast* ast)
	{
		auto* primative = reinterpret_cast<PrimativeAst*>(ast);
		return primative;
	}

	void getData(Ast* ast)
	{
		switch (ast->GetType())
		{
		case AstType::FUNCTION_CALL:
		{

		}
		break;
		}
	}

	void Execute() override
	{
		auto astsEnd = asts->cend();

		for (auto it = asts->cbegin(); astsEnd != it; ++it)
		{
			AstType type = (*it)->GetType();
			switch (type)
			{
			case AstType::ASSIGNMENT:
			{
				AssignmentAst* assignment = reinterpret_cast<AssignmentAst*>(*it);
				//variables[assignment->Name] = assignment->Data
			}
			break;

			case AstType::FUNCTION_CALL:
			{
				FunctionCallAst* callAst = reinterpret_cast<FunctionCallAst*>(*it);
			}
			break;


			case AstType::BINARY_OPERATION:
			{
				BinaryAst* callAst = reinterpret_cast<BinaryAst*>(*it);

				auto* lhs = reinterpret_cast<PrimativeAst*>(callAst->Left)->Value;
				auto* rhs = reinterpret_cast<PrimativeAst*>(callAst->Right)->Value;
				PrimativeValue* value;

				switch (callAst->Op)
				{
				case EASY_OPERATOR_TYPE::PLUS:
					value = (*lhs) + (*rhs);
					break;

				case EASY_OPERATOR_TYPE::MINUS:
					break;

				case EASY_OPERATOR_TYPE::MULTIPLICATION:
					value = (*lhs) * (*rhs);
					
					break;
				}

				switch (value->Type)
				{
				case PrimativeValue::Type::PRI_BOOL:
					std::wcout << value->Bool << std::endl;
					break;

				case PrimativeValue::Type::PRI_DOUBLE:
					std::wcout << value->Double << std::endl;
					break;

				case PrimativeValue::Type::PRI_INTEGER:
					std::wcout << value->Integer << std::endl;
					break;

				case PrimativeValue::Type::PRI_STRING:
					std::wcout << value->String << std::endl;
					break;
				}
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
		asd->Execute();
	}
};

#endif