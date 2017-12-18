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
            //std::wcout << L"[" << assignment->Name << L"]" << std::endl;
        }
            break;
            
        case EASY_AST_TYPE::BLOCK:
        {
            BlockAst* block = reinterpret_cast<BlockAst*>(ast);
            std::vector<Ast*>::const_iterator blocksEnd = block->Blocks->cend();
            for (std::vector<Ast*>::const_iterator it = block->Blocks->cbegin(); it != blocksEnd; ++it)
            {
                Ast* blockAst = *it;
                PrimativeValue* result = getData(blockAst, scope);

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
			info->Callback = [=](std::shared_ptr<std::vector<PrimativeValue*> > const &, PrimativeValue & returnValue, Scope & functionScope)
			{
				returnValue = *this->getData(func->Body, functionScope);
			};

			System::UserMethods[func->Name] = info;
        }
            break;

            // func fibonacci(num) { if num <= 1 then return 1 left = fibonacci(num - 1) right = fibonacci(num - 2) return left + right }
        case EASY_AST_TYPE::FUNCTION_CALL:
        {
            FunctionCallAst* call = reinterpret_cast<FunctionCallAst*>(ast);
            PrimativeValue* returnValue = new PrimativeValue;
			Scope functionScope(&scope);

            if (System::SystemMethods.find(call->Function) != System::SystemMethods.end())
            {
                MethodCallback function = System::SystemMethods[call->Function];
                std::shared_ptr<std::vector<PrimativeValue*> > args = std::make_shared<std::vector<PrimativeValue*>>();
                
                std::vector<Ast*>::const_iterator argsEnd = call->Args.cend();
                for (std::vector<Ast*>::const_iterator it = call->Args.cbegin(); it != argsEnd; ++it)
                {
                    Ast* argAst = *it;
                    PrimativeValue* argItem = getData(argAst, functionScope);
                    args->push_back(argItem);
                }
                
                function(args, *returnValue);
            }
            else if (System::UserMethods.find(call->Function) != System::UserMethods.end())
            {
                std::shared_ptr<std::vector<PrimativeValue*> > args = std::make_shared<std::vector<PrimativeValue*>>();
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
                    PrimativeValue* argItem = getData(argAst, functionScope);
                    args->push_back(argItem);

					functionScope.SetVariable(functionInfo->FunctionAst->Args[i], argItem);
                }
                
                functionInfo->Callback(args, *returnValue, functionScope);
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
		result = getData(*it, globalScope);

		if (result != nullptr)
		{
			std::wcout << result->Describe() << '\n';
		}

		asts.push_back(*it);
	}

	temporaryAsts.clear();

	return result;
}

InterpreterBackend::InterpreterBackend() : stream(std::cout)
{
    
}

InterpreterBackend::InterpreterBackend(std::ostream & pStream) : stream(pStream)
{
    
}

InterpreterBackend::~InterpreterBackend()
{

}
