#include "Exceptions.h"
#include "InterpreterBackend.h"
#include "System.h"

class NullBuffer : public std::streambuf
{
public:
    int overflow(int c) { return c; }
};

void InterpreterBackend::Prepare(std::shared_ptr<std::vector<Ast*>> pAsts)
{
	temporaryAsts.clear();
	temporaryAsts.insert(temporaryAsts.end(), pAsts.get()->begin(), pAsts.get()->end());
}

PrimativeValue* InterpreterBackend::getPrimative(Ast* ast)
{
    auto* primative = static_cast<PrimativeAst*>(ast)->Value;
    return primative;
}

PrimativeValue* InterpreterBackend::getData(Ast* ast, Scope & scope)
{
    if (ast == nullptr)
        return nullptr;
    
    switch (ast->GetType())
    {
        case EASY_AST_TYPE::PRIMATIVE:
            return static_cast<PrimativeAst*>(ast)->Value;
            break;
            
        case EASY_AST_TYPE::RETURN:
            return getData(static_cast<ReturnAst*>(ast)->Data, scope);
            break;

		case EASY_AST_TYPE::PARENTHESES_BLOCK:
			return getData(static_cast<ParenthesesGroupAst*>(ast)->Data, scope);
			break;
            
        case EASY_AST_TYPE::VARIABLE:
        {
            VariableAst* variable = static_cast<VariableAst*>(ast);
			return scope.GetVariable(variable->Value);
        }
            break;
            
        case EASY_AST_TYPE::ASSIGNMENT:
        {
            AssignmentAst* assignment = static_cast<AssignmentAst*>(ast);
            auto* value = getData(assignment->Data, scope);
			if (value == nullptr)
				throw NullException(_T("Value not found"));

			scope.SetVariable(assignment->Name, value);
        }
            break;
            
        case EASY_AST_TYPE::BLOCK:
        {
            Scope blockScope(&scope);

            BlockAst* block = static_cast<BlockAst*>(ast);
            std::vector<Ast*>::const_iterator blocksEnd = block->Blocks->cend();
            for (std::vector<Ast*>::const_iterator it = block->Blocks->cbegin(); it != blocksEnd; ++it)
            {
                Ast* blockAst = *it;
                PrimativeValue* result = getData(blockAst, blockScope);

				if (blockAst->GetType() == EASY_AST_TYPE::RETURN || (result != nullptr && !result->IsNull()))
					return result;
            }
        }
            break;
            
        case EASY_AST_TYPE::FUNCTION_DECLERATION:
        {
            FunctionDefinetionAst* func = static_cast<FunctionDefinetionAst*>(ast);
			FunctionInfo* info = new FunctionInfo;
			info->FunctionAst = func;
			info->Callback = [=](std::unordered_map<string_type, PrimativeValue*> const & args, PrimativeValue * returnValue, Scope & functionScope)
			{
                Scope innerScope(&functionScope);

                auto argsEnd = args.end();
                for (auto it = args.begin(); it != argsEnd; ++it)
                    innerScope.SetVariable(it->first, it->second);

                auto* result = this->getData(func->Body, innerScope);
                if (result != nullptr)
                    *returnValue = *result;
			};

			System::UserMethods[func->Name] = info;
        }
            break;

            // func fibonacci(num) { if num <= 1 then return 1 left = fibonacci(num - 1) right = fibonacci(num - 2) return left + right }
        case EASY_AST_TYPE::FUNCTION_CALL:
        {
            auto * call = static_cast<FunctionCallAst*>(ast);
            auto * returnValue = new PrimativeValue;

			if (System::SystemPackages.find(call->Package) != System::SystemPackages.end() && System::SystemPackages[call->Package].find(call->Function) != System::SystemPackages[call->Package].end())
			{
				Caller* function = System::SystemPackages[call->Package][call->Function];
				std::vector<Any> args;

				auto argsEnd = call->Args.cend();
				for (auto it = call->Args.cbegin(); it != argsEnd; ++it)
				{
					Ast* argAst = *it;
					auto* argItem = getData(argAst, scope);
					if (argItem != nullptr)
					{
						Any* anyType = new Any(*argItem);
						args.push_back(*anyType);
					}
				}

				returnValue = new PrimativeValue;
				if (args.size() > 0)
				{
					Any result = function->Call(&args[0]);
					returnValue->FromAny(result);
				}
				else
					returnValue->SetInteger(0);
			}
			else if (System::UserMethods.find(call->Function) != System::UserMethods.end())
			{
				std::unordered_map<string_type, PrimativeValue*> args;
				auto* functionInfo = System::UserMethods[call->Function];

				size_t requiredParameterCount = functionInfo->FunctionAst->Args.size();
				size_t currentParameterCount = call->Args.size();

				if (requiredParameterCount != currentParameterCount)
				{
					string_type errorMessage(_T("Function require ") + AS_STRING(requiredParameterCount) + _T(" but received ") + AS_STRING(currentParameterCount));
					throw ParameterError(errorMessage);
				}

				for (size_t i = 0; i < currentParameterCount; ++i)
				{
					Ast* argAst = call->Args[i];
					PrimativeValue* argItem = getData(argAst, scope);
					args[functionInfo->FunctionAst->Args[i]] = argItem;
				}

				functionInfo->Callback(args, returnValue, *Scope::GlobalScope);
			}
			else
				throw ParseError("Unknown method");
            
            return returnValue;
        }
            break;
            
        case EASY_AST_TYPE::IF_STATEMENT:
        {
            IfStatementAst* ifStatement = static_cast<IfStatementAst*>(ast);
            auto* control = getData(ifStatement->ControlOpt, scope);
            if (control != nullptr)
            {
                if (control->Bool)
                    return getData(ifStatement->True, scope);
                else
                    return getData(ifStatement->False, scope);
            }
        }
            break;
            
            
        case EASY_AST_TYPE::FOR:
        {
            ForStatementAst* forStatement = static_cast<ForStatementAst*>(ast);
            
            auto* startValue = getData(forStatement->Start, scope);
            auto* endValue = getData(forStatement->End, scope);
            
			Scope forScope(&scope);
			forScope.SetVariable(forStatement->Variable, startValue);

            for (size_t i = startValue->Integer; i <= endValue->Integer; ++i)
            {
                getData(forStatement->Repeat, forScope);

				PrimativeValue* incrementSymbol = forScope.GetVariable(forStatement->Variable);
				++incrementSymbol->Integer;
				forScope.SetVariable(forStatement->Variable, incrementSymbol);
            }
        }
            break;
            
        case EASY_AST_TYPE::BINARY_OPERATION:
        {
            BinaryAst* callAst = static_cast<BinaryAst*>(ast);
            
            PrimativeValue* lhs = getData(callAst->Left, scope);
            PrimativeValue* rhs = getData(callAst->Right, scope);
            PrimativeValue* value = nullptr;
            
            switch (callAst->Op)
            {
                case EASY_OPERATOR_TYPE::PLUS:
                    value = (*lhs) + (*rhs);
                    break;
                    
                case EASY_OPERATOR_TYPE::MINUS:
                    value = (*lhs) - (*rhs);
                    break;
                    
                case EASY_OPERATOR_TYPE::MULTIPLICATION:
                    value = (*lhs) * (*rhs);
                    break;
                    
                    
                case EASY_OPERATOR_TYPE::DIVISION:
                    value = (*lhs) / (*rhs);
                    break;
                    
                case EASY_OPERATOR_TYPE::APPEND:
                    lhs->Append(rhs->Clone());
                    
                    value = lhs;
                    break;
            }
            
            return value;
        }
            break;

		case EASY_AST_TYPE::STRUCT_OPERATION:
		{
			StructAst* callAst = static_cast<StructAst*>(ast);

			PrimativeValue* target = getData(callAst->Target, scope);
			PrimativeValue* source1 = getData(callAst->Source1, scope);
			PrimativeValue* source2 = nullptr;

			if (callAst->Source2 != nullptr)
				source2 = getData(callAst->Source2, scope);

			PrimativeValue* value = nullptr;

			switch (callAst->Op)
			{
			case EASY_OPERATOR_TYPE::APPEND:
				target->Append(source1->Clone());

				value = target;
				break;

			case EASY_OPERATOR_TYPE::INDEXER:
				if (source1->Type != PrimativeValue::Type::PRI_INTEGER)
					throw ParseError("Indexer must be integer.");

				switch (target->Type)
				{
					case PrimativeValue::Type::PRI_ARRAY:
					{
						if (target->Array->size() <= source1->Integer)
							throw ParseError(AS_STRING(source1->Integer) + " bigger than array size");
						
						if (source1->Integer < 0)
							throw ParseError(AS_STRING(source1->Integer) + " can not be smaller than zero");

						if (source2 != nullptr)
						{
							if (target->Array->size() <= source2->Integer)
								throw ParseError(AS_STRING(source2->Integer) + " bigger than array size");

							if (source2->Integer < 0)
								throw ParseError(AS_STRING(source2->Integer) + " can not be smaller than zero");

							value = PrimativeValue::CreateArray();
							for (size_t i = source1->Integer; i < source2->Integer + source1->Integer; ++i)
								value->Array->push_back(target->Array->at(i));
						}
						else 
							value = target->Array->at(source1->Integer);
					}
					break;

					case PrimativeValue::Type::PRI_STRING:
					{
						if (target->String->size() <= source1->Integer)
							throw ParseError(AS_STRING(source1->Integer) + " bigger than string size");

						if (source1->Integer < 0)
							throw ParseError(AS_STRING(source1->Integer) + " can not be smaller than zero");

						if (source2 != nullptr)
						{
							if (target->Array->size() <= source2->Integer)
								throw ParseError(AS_STRING(source2->Integer) + " bigger than array size");
							
							if (source2->Integer < 0)
								throw ParseError(AS_STRING(source2->Integer) + " can not be smaller than zero");

							value = PrimativeValue::CreateString(target->String->substr(source1->Integer, source2->Integer));
						}
						else
						{
							string_type data(1, (*target->String)[source1->Integer]);
							value = new PrimativeValue(data);
						}
					}
					break;

					default:
						break;
				}

				break;
			}

			return value;
		}
		break;
            
        case EASY_AST_TYPE::CONTROL_OPERATION:
        {
            ControlAst* callAst = static_cast<ControlAst*>(ast);
            
            PrimativeValue* lhs = getData(callAst->Left, scope);
            PrimativeValue* rhs = getData(callAst->Right, scope);
            PrimativeValue* value = nullptr;
            
            switch (callAst->Op)
            {
                case EASY_OPERATOR_TYPE::GREATOR:
                    value = (*lhs) > (*rhs);
                    break;
                    
                case EASY_OPERATOR_TYPE::GREATOR_EQUAL:
                    value = (*lhs) >= (*rhs);
                    break;
                    
                case EASY_OPERATOR_TYPE::LOWER:
                    value = (*lhs) < (*rhs);
                    break;
                    
                case EASY_OPERATOR_TYPE::LOWER_EQUAL:
                    value = (*lhs) <= (*rhs);
                    break;
                    
                case EASY_OPERATOR_TYPE::EQUAL:
                    value = (*lhs) == (*rhs);
                    break;

				case EASY_OPERATOR_TYPE::NOT_EQUAL:
					value = (*lhs) != (*rhs);
					break;

				case EASY_OPERATOR_TYPE::OR:
					value = (*lhs) || (*rhs);
					break;

				case EASY_OPERATOR_TYPE::AND:
					value = (*lhs) && (*rhs);
					break;
            }
            
            return value;
        }
            break;
    }
    
    return nullptr;
}

PrimativeValue* InterpreterBackend::Execute()
{
	PrimativeValue* result = nullptr;
    auto astsEnd = temporaryAsts.cend();
	for (auto it = temporaryAsts.cbegin(); astsEnd != it; ++it)
	{
		result = getData(*it, *Scope::GlobalScope);

		if (result != nullptr)
		{
			console_out << result->Describe() << '\n';
		}

		asts.push_back(*it);
	}

	temporaryAsts.clear();

	return result;
}

InterpreterBackend::InterpreterBackend()
{
    Scope::GlobalScope = new Scope;
}

InterpreterBackend::~InterpreterBackend()
{

}
