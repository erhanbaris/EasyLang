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
                return variables[variable->Value];
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
                
            case AstType::FUNCTION_CALL:
            {
                
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
