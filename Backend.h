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
    
    PrimativeValue* getPrimative(Ast* ast)
    {
        auto* primative = reinterpret_cast<PrimativeAst*>(ast)->Value;
        return primative;
    }
    
    PrimativeValue* getData(Ast* ast)
    {
        if (ast == nullptr)
            return nullptr;
        
        switch (ast->GetType())
        {
            case AstType::PRIMATIVE:
                return reinterpret_cast<PrimativeAst*>(ast)->Value;
                break;
                
            case AstType::VARIABLE:
            {
                VariableAst* variable = reinterpret_cast<VariableAst*>(ast);
                
                if (variables.find(variable->Value) != variables.end())
                    return variables[variable->Value];
                
                return nullptr;
            }
                break;
                
            case AstType::ASSIGNMENT:
            {
                AssignmentAst* assignment = reinterpret_cast<AssignmentAst*>(ast);
                auto* value = getData(assignment->Data);
                variables[assignment->Name] = value;
                std::wcout << L"Assign -> [" << assignment->Name << L"]" << std::endl;
            }
                break;
       
            case AstType::BLOCK:
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
                
            case AstType::FUNCTION_CALL:
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
                    //function(
                    //call->Function
                }
            }
                break;
                
            case AstType::IF_STATEMENT:
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
                
            case AstType::BINARY_OPERATION:
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
                
                switch (value->Type)
                {
                    case PrimativeValue::Type::PRI_BOOL:
                        std::wcout << L"(bool) " << (value->Bool ? L"evet" : L"hayır") << std::endl;
                        break;
                        
                    case PrimativeValue::Type::PRI_DOUBLE:
                        std::wcout << L"(double) " << value->Double << std::endl;
                        break;
                        
                    case PrimativeValue::Type::PRI_INTEGER:
                        std::wcout << L"(integer) " << value->Integer << std::endl;
                        break;
                        
                    case PrimativeValue::Type::PRI_STRING:
                        std::wcout << L"(string) " << value->String << std::endl;
                        break;
                }
            }
                break;
                
            case AstType::CONTROL_OPERATION:
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
                
                switch (value->Type)
                {
                    case PrimativeValue::Type::PRI_BOOL:
                        std::wcout << L"(bool) " << (value->Bool ? L"evet" : L"hayır") << std::endl;
                        break;
                        
                    case PrimativeValue::Type::PRI_DOUBLE:
                        std::wcout << L"(double) " << value->Double << std::endl;
                        break;
                        
                    case PrimativeValue::Type::PRI_INTEGER:
                        std::wcout << L"(integer) " << value->Integer << std::endl;
                        break;
                        
                    case PrimativeValue::Type::PRI_STRING:
                        std::wcout << L"(string) " << value->String << std::endl;
                        break;
                }
            }
                break;
        }
        
        return nullptr;
    }
    
    void Execute() override
    {
        auto astsEnd = asts->cend();
        for (auto it = asts->cbegin(); astsEnd != it; ++it)
            getData(*it);
    }
    
    
    
private:
    std::shared_ptr<std::vector<Ast*>> asts;
    std::unordered_map<std::wstring, PrimativeValue*> variables;
};

template <class T = Backend>
class BackendExecuter {
public:
    BackendExecuter() {
        backend = new T;
    }
    
    static_assert(std::is_base_of<Backend, T>::value, "T must be derived from Backend");
    void Prepare(std::shared_ptr<std::vector<Ast*>> asts)
    {
        backend->Prepare(asts);
        backend->Execute();
    }
    
private:
    Backend* backend;
};

#endif
