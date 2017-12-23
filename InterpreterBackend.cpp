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
    auto* primative = reinterpret_cast<PrimativeAst*>(ast)->Value;
    return primative;
}

PrimativeValue* InterpreterBackend::getData(Ast* ast, Scope & scope)
{
    if (ast == nullptr)
        return nullptr;
    
    switch (ast->GetType())
    {
        case EASY_AST_TYPE::PRIMATIVE:
            return reinterpret_cast<PrimativeAst*>(ast)->Value;
            break;
            
        case EASY_AST_TYPE::RETURN:
            return getData(reinterpret_cast<ReturnAst*>(ast)->Data, scope);
            break;

		case EASY_AST_TYPE::PARENTHESES_BLOCK:
			return getData(reinterpret_cast<ParenthesesGroupAst*>(ast)->Data, scope);
			break;
            
        case EASY_AST_TYPE::VARIABLE:
        {
            VariableAst* variable = reinterpret_cast<VariableAst*>(ast);
			return scope.GetVariable(variable->Value);
        }
            break;
            
        case EASY_AST_TYPE::ASSIGNMENT:
        {
            AssignmentAst* assignment = reinterpret_cast<AssignmentAst*>(ast);
            auto* value = getData(assignment->Data, scope);
			if (value == nullptr)
				throw NullException("Value not found");

			scope.SetVariable(assignment->Name, value);
        }
            break;
            
        case EASY_AST_TYPE::BLOCK:
        {
            Scope blockScope(&scope);

            BlockAst* block = reinterpret_cast<BlockAst*>(ast);
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
            FunctionDefinetionAst* func = reinterpret_cast<FunctionDefinetionAst*>(ast);
			FunctionInfo* info = new FunctionInfo;
			info->FunctionAst = func;
			info->Callback = [=](std::unordered_map<std::wstring, PrimativeValue*> const & args, PrimativeValue * returnValue, Scope & functionScope)
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
            auto * call = reinterpret_cast<FunctionCallAst*>(ast);
            auto * returnValue = new PrimativeValue;

            if (System::SystemPackages.find(call->Package) != System::SystemPackages.end() && System::SystemPackages[call->Package].find(call->Function) != System::SystemPackages[call->Package].end())
            {
                MethodCallback function = System::SystemPackages[call->Package][call->Function];
                std::shared_ptr<std::vector<PrimativeValue*> > args = std::make_shared<std::vector<PrimativeValue*>>();

                auto argsEnd = call->Args.cend();
                for (auto it = call->Args.cbegin(); it != argsEnd; ++it)
                {
                    Ast* argAst = *it;
                    auto argItem = getData(argAst, scope);
                    args->push_back(argItem);
                }
                
                function(args, *returnValue);
            }
            else if (System::UserMethods.find(call->Function) != System::UserMethods.end())
            {
                std::unordered_map<std::wstring, PrimativeValue*> args;
				auto* functionInfo = System::UserMethods[call->Function];

				size_t requiredParameterCount = functionInfo->FunctionAst->Args.size();
				size_t currentParameterCount = call->Args.size();

				if (requiredParameterCount != currentParameterCount)
				{
					std::string errorMessage("Function require " + std::to_string(requiredParameterCount) + " but received " + std::to_string(currentParameterCount));
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
            
            return returnValue;
        }
            break;
            
        case EASY_AST_TYPE::IF_STATEMENT:
        {
            IfStatementAst* ifStatement = reinterpret_cast<IfStatementAst*>(ast);
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
            ForStatementAst* forStatement = reinterpret_cast<ForStatementAst*>(ast);
            
            auto* startValue = getData(forStatement->Start, scope);
            auto* endValue = getData(forStatement->End, scope);
            
            variables[forStatement->Variable] = startValue;
            
            for (size_t i = startValue->Integer; i < endValue->Integer; ++i)
            {
                variables[forStatement->Variable]->SetInteger(i);
                getData(forStatement->Repeat, scope);
            }
        }
            break;
            
        case EASY_AST_TYPE::BINARY_OPERATION:
        {
            BinaryAst* callAst = reinterpret_cast<BinaryAst*>(ast);
            
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
            }
            
            return value;
        }
            break;
            
        case EASY_AST_TYPE::CONTROL_OPERATION:
        {
            ControlAst* callAst = reinterpret_cast<ControlAst*>(ast);
            
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
			std::wcout << result->Describe() << '\n';
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
