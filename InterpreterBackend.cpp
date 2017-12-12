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
    asts = pAsts;
}

PrimativeValue* InterpreterBackend::getPrimative(Ast* ast)
{
    auto* primative = reinterpret_cast<PrimativeAst*>(ast)->Value;
    return primative;
}

PrimativeValue* InterpreterBackend::getData(Ast* ast)
{
    if (ast == nullptr)
        return nullptr;
    
    switch (ast->GetType())
    {
        case EASY_AST_TYPE::PRIMATIVE:
            return reinterpret_cast<PrimativeAst*>(ast)->Value;
            break;
            
        case EASY_AST_TYPE::VARIABLE:
        {
            VariableAst* variable = reinterpret_cast<VariableAst*>(ast);
            
            if (variables.find(variable->Value) != variables.end())
                return variables[variable->Value];
            
            return nullptr;
        }
            break;
            
        case EASY_AST_TYPE::ASSIGNMENT:
        {
            AssignmentAst* assignment = reinterpret_cast<AssignmentAst*>(ast);
            auto* value = getData(assignment->Data);
			if (value == nullptr)
				throw NullException("Value not found");

            variables[assignment->Name] = value;
            std::wcout << L"Assign -> [" << assignment->Name << L"]" << std::endl;
        }
            break;
            
        case EASY_AST_TYPE::BLOCK:
        {
            BlockAst* block = reinterpret_cast<BlockAst*>(ast);
            std::vector<Ast*>::const_iterator blocksEnd = block->Blocks->cend();
            for (std::vector<Ast*>::const_iterator it = block->Blocks->cbegin(); it != blocksEnd; ++it)
            {
                Ast* blockAst = *it;
                getData(blockAst);
            }
            
        }
            break;
            
        case EASY_AST_TYPE::FUNCTION_CALL:
        {
            FunctionCallAst* call = reinterpret_cast<FunctionCallAst*>(ast);
            if (System::SystemMethods.find(call->Function) != System::SystemMethods.end())
            {
                MethodCallback function = System::SystemMethods[call->Function];
                PrimativeValue* returnValue = new PrimativeValue;;
                std::shared_ptr<std::vector<PrimativeValue*> > args = std::make_shared<std::vector<PrimativeValue*>>();
                
                std::vector<Ast*>::const_iterator argsEnd = call->Args.cend();
                for (std::vector<Ast*>::const_iterator it = call->Args.cbegin(); it != argsEnd; ++it)
                {
                    Ast* argAst = *it;
                    PrimativeValue* argItem = getData(argAst);
                    args->push_back(argItem);
                }
                
                function(args, *returnValue);
                return returnValue;
            }
        }
            break;
            
        case EASY_AST_TYPE::IF_STATEMENT:
        {
            IfStatementAst* ifStatement = reinterpret_cast<IfStatementAst*>(ast);
            auto* control = getData(ifStatement->ControlOpt);
            if (control != nullptr)
            {
                if (control->Bool)
                    getData(ifStatement->True);
                else
                    getData(ifStatement->False);
            }
        }
            break;
            
            
        case EASY_AST_TYPE::FOR:
        {
            ForStatementAst* forStatement = reinterpret_cast<ForStatementAst*>(ast);
            
            auto* startValue = getData(forStatement->Start);
            auto* endValue = getData(forStatement->End);
            
            variables[forStatement->Variable] = startValue;
            
            for (size_t i = startValue->Integer; i < endValue->Integer; ++i)
            {
                variables[forStatement->Variable]->SetInteger(i);
                getData(forStatement->Repeat);
            }
        }
            break;
            
        case EASY_AST_TYPE::BINARY_OPERATION:
        {
            BinaryAst* callAst = reinterpret_cast<BinaryAst*>(ast);
            
            PrimativeValue* lhs = getData(callAst->Left);
            PrimativeValue* rhs = getData(callAst->Right);
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
            
            PrimativeValue* lhs = getData(callAst->Left);
            PrimativeValue* rhs = getData(callAst->Right);
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

void InterpreterBackend::Execute()
{
	PrimativeValue* result = nullptr;
    auto astsEnd = asts->cend();
	for (auto it = asts->cbegin(); astsEnd != it; ++it)
	{
		result = getData(*it);

		if (result != nullptr)
		{
			std::wcout << result->Describe() << '\n';
		}
	}
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
