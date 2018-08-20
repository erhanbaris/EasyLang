#if defined(EASYLANG_JIT_ACTIVE)
#define NOMINMAX
#include <fstream>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <limits>
#include <stack>

#include "Exceptions.h"
#include "LLVMBackend.h"
#include "System.h"

#include "llvm/ADT/APInt.h"
#include "llvm/IR/Verifier.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

class LLVMBackendImpl
{
public:
    std::unique_ptr<Module> module;
    LLVMContext context;
    std::stack<Value*> values;
    std::unordered_map<std::string, Function*> functions;

    LLVMBackendImpl()
    {
        //LLVM
        InitializeNativeTarget();
        InitializeNativeTargetAsmPrinter();

        module = std::unique_ptr<Module>(new Module("SYSTEM", context));
        LLVMContext Context;
    }

    ~LLVMBackendImpl()
    {
        //delete module;
    }

    llvm::Value* getConstant(PrimativeValue* astValue)
    {
        switch (astValue->Type) {
        case PrimativeValue::Type::PRI_INTEGER:
            return llvm::ConstantInt::get(this->context, APInt(32, astValue->Integer, false));

        case PrimativeValue::Type::PRI_DOUBLE:
            return llvm::ConstantFP::get(this->context, llvm::APFloat(astValue->Double));

        case PrimativeValue::Type::PRI_STRING:
            return llvm::ConstantDataArray::getString(this->context, AS_CHAR(astValue->String->c_str()), true);

        case PrimativeValue::Type::PRI_BOOL:
            return llvm::ConstantInt::get(this->context, APInt(1, astValue->Bool, false));
            break;

        case PrimativeValue::Type::PRI_ARRAY:
        {
            auto* array = astValue->Array;

            IntegerType* i32 = IntegerType::get(this->context, 32);
            ArrayType* arrayType = ArrayType::get(i32, array->size());
            std::vector<Value*> constantArray;

            auto itEnd = array->end();
            for (auto it = array->begin(); it != itEnd; ++it)
                constantArray.push_back(getConstant(*it));
        }
        /*return llvm::ConstantArray::get(;*/
        break;

        case PrimativeValue::Type::PRI_DICTIONARY:
            break;

        case PrimativeValue::Type::PRI_NULL:
            break;
        }
    }
};

BACKEND_ITEM_TYPE LLVMBackend::detectType(Ast* ast)
{
    if (ast == nullptr)
    return BACKEND_ITEM_TYPE::EMPTY;
    
    switch (ast->GetType())
    {
        case EASY_AST_TYPE::UNARY:
        {
            auto* unary = static_cast<UnaryAst*>(ast);
            return detectType(unary->Data);
        }
            
        case EASY_AST_TYPE::PRIMATIVE:
        {
            auto type = static_cast<PrimativeAst*>(ast)->Value->Type;
            switch (type)
            {
                    
                case PrimativeValue::Type::PRI_INTEGER:
                    return BACKEND_ITEM_TYPE::INT;
                    
                case PrimativeValue::Type::PRI_DOUBLE:
                    return BACKEND_ITEM_TYPE::DOUBLE;
                    
                case PrimativeValue::Type::PRI_STRING:
                    return BACKEND_ITEM_TYPE::STRING;
                    
                case PrimativeValue::Type::PRI_BOOL:
                    return BACKEND_ITEM_TYPE::BOOL;
                    
                case PrimativeValue::Type::PRI_ARRAY:
                    return BACKEND_ITEM_TYPE::ARRAY;
                    
                case PrimativeValue::Type::PRI_DICTIONARY:
                    return BACKEND_ITEM_TYPE::DICTIONARY;
                    
                case PrimativeValue::Type::PRI_NULL:
                    return BACKEND_ITEM_TYPE::EMPTY;
            }
        }
            
        case EASY_AST_TYPE::RETURN:
            return BACKEND_ITEM_TYPE::EMPTY;
            
        case EASY_AST_TYPE::PARENTHESES_BLOCK:
            return detectType(static_cast<ParenthesesGroupAst*>(ast)->Data);
            
        case EASY_AST_TYPE::EXPR_STATEMENT:
            return detectType(static_cast<ExprStatementAst*>(ast)->Expr);
            break;
            
        case EASY_AST_TYPE::VARIABLE:
        {
            
        }
            break;
            
        case EASY_AST_TYPE::ASSIGNMENT:
            return BACKEND_ITEM_TYPE::EMPTY;
            break;
            
        case EASY_AST_TYPE::BLOCK:
            return BACKEND_ITEM_TYPE::EMPTY;
            break;
            
        case EASY_AST_TYPE::FUNCTION_DECLARATION:
            //static_cast<FunctionDefinetionAst*>(ast)->accept(this);
            break;
            
        case EASY_AST_TYPE::FUNCTION_CALL:
        {
            
        }
            break;
            
        case EASY_AST_TYPE::IF_STATEMENT:
            return BACKEND_ITEM_TYPE::EMPTY;
            
        case EASY_AST_TYPE::FOR:
            return BACKEND_ITEM_TYPE::EMPTY;
            
        case EASY_AST_TYPE::BINARY_OPERATION:
        {
            auto binary = static_cast<BinaryAst*>(ast);
            BACKEND_ITEM_TYPE rightType =  detectType(binary->Right);
            BACKEND_ITEM_TYPE leftType =  detectType(binary->Left);
            
            return operationResultType(rightType, leftType);
        }
            break;
            
        case EASY_AST_TYPE::STRUCT_OPERATION:
            //static_cast<StructAst*>(ast)->accept(this);
            break;
            
        case EASY_AST_TYPE::CONTROL_OPERATION:
            return BACKEND_ITEM_TYPE::BOOL;
            break;
            
        case EASY_AST_TYPE::NONE:
            return BACKEND_ITEM_TYPE::EMPTY;
    }
    
    return BACKEND_ITEM_TYPE::EMPTY;
}

void LLVMBackend::Prepare(std::shared_ptr<std::vector<Ast*>> pAsts)
{
    temporaryAsts.clear();
    temporaryAsts.insert(temporaryAsts.end(), pAsts.get()->begin(), pAsts.get()->end());
}

PrimativeValue* LLVMBackend::getPrimative(Ast* ast)
{
    auto* primative = static_cast<PrimativeAst*>(ast)->Value;
    return primative;
}

BACKEND_ITEM_TYPE LLVMBackend::operationResultType(BACKEND_ITEM_TYPE from, BACKEND_ITEM_TYPE to)
{
    if (from == BACKEND_ITEM_TYPE::INT && to == BACKEND_ITEM_TYPE::DOUBLE)
        return BACKEND_ITEM_TYPE::DOUBLE;
    else if (to == BACKEND_ITEM_TYPE::INT && from == BACKEND_ITEM_TYPE::DOUBLE)
        return BACKEND_ITEM_TYPE::DOUBLE;
    else if (to == BACKEND_ITEM_TYPE::INT && from == BACKEND_ITEM_TYPE::INT)
        return BACKEND_ITEM_TYPE::INT;
    else if (from == BACKEND_ITEM_TYPE::INT && to == BACKEND_ITEM_TYPE::BOOL)
        return BACKEND_ITEM_TYPE::INT;
    else if (to == BACKEND_ITEM_TYPE::INT && from == BACKEND_ITEM_TYPE::BOOL)
        return BACKEND_ITEM_TYPE::INT;
    else if (to == BACKEND_ITEM_TYPE::BOOL && from == BACKEND_ITEM_TYPE::BOOL)
        return BACKEND_ITEM_TYPE::BOOL;
    else if (from == BACKEND_ITEM_TYPE::DOUBLE && to == BACKEND_ITEM_TYPE::BOOL)
        return BACKEND_ITEM_TYPE::DOUBLE;
    else if (to == BACKEND_ITEM_TYPE::DOUBLE && from == BACKEND_ITEM_TYPE::BOOL)
        return BACKEND_ITEM_TYPE::DOUBLE;
    else if (to == BACKEND_ITEM_TYPE::DOUBLE && from == BACKEND_ITEM_TYPE::DOUBLE)
        return BACKEND_ITEM_TYPE::DOUBLE;
}

llvm::Value* LLVMBackend::getAstItem(Ast* ast)
{
    if (ast == nullptr)
        return nullptr;

    switch (ast->GetType())
    {
    case EASY_AST_TYPE::UNARY:
    {
        auto* unary = static_cast<UnaryAst*>(ast);
        getAstItem(unary->Data);

        return nullptr;
    }
    break;

    case EASY_AST_TYPE::PRIMATIVE:
        return static_cast<JITPrimativeAst*>(ast)->generate(this);
        break;

    case EASY_AST_TYPE::RETURN:
        static_cast<ReturnAst*>(ast)->accept(this);
        break;

    case EASY_AST_TYPE::PARENTHESES_BLOCK:
        break;

    case EASY_AST_TYPE::EXPR_STATEMENT:
        return static_cast<JITExprStatementAst*>(ast)->generate(this);
        break;

    case EASY_AST_TYPE::VARIABLE:
        break;

    case EASY_AST_TYPE::ASSIGNMENT:
        break;

    case EASY_AST_TYPE::BLOCK:
        static_cast<BlockAst*>(ast)->accept(this);
        break;

    case EASY_AST_TYPE::FUNCTION_DECLARATION:
        static_cast<FunctionDefinetionAst*>(ast)->accept(this);
        break;

    case EASY_AST_TYPE::FUNCTION_CALL:
        break;

    case EASY_AST_TYPE::IF_STATEMENT:
        static_cast<IfStatementAst*>(ast)->accept(this);
        break;

    case EASY_AST_TYPE::FOR:
        static_cast<ForStatementAst*>(ast)->accept(this);
        break;

    case EASY_AST_TYPE::BINARY_OPERATION:
        return static_cast<JITBinaryAst*>(ast)->generate(this);
        break;

    case EASY_AST_TYPE::STRUCT_OPERATION:
        break;

    case EASY_AST_TYPE::CONTROL_OPERATION:
        break;

    case EASY_AST_TYPE::NONE:
        break;
    }

    return nullptr;
}


void LLVMBackend::Compile(std::vector<char_type> & pOpcode)
{
    opcodes.assign(pOpcode.begin(), pOpcode.end());

    size_t totalAst = temporaryAsts.size();
    for (int i = 0; i < totalAst; ++i) {
        getAstItem(temporaryAsts[i]);
    }
    

    temporaryAsts.clear();
    pOpcode.assign(this->opcodes.begin(), this->opcodes.end());
    this->opcodes.clear();
}

/*
 *
 * func carp(a:int):int return a * 10
 * func test(a:int, b:int):int return a + b
 *
 * */
PrimativeValue* LLVMBackend::Execute()
{
    PrimativeValue* result = nullptr;
    size_t codeStart = temporaryAsts.size();
    std::vector<char_type> opcodes;
    
    this->Compile(opcodes);

    return nullptr;
}

void LLVMBackend::Execute(std::vector<char_type> const & opcodes)
{
    int n = 32;
    std::string errStr;
    ExecutionEngine *EE = EngineBuilder(std::move(impl->module)).setErrorStr(&errStr).create();
    
    std::vector<GenericValue> Args(1);
    Args[0].IntVal = APInt(32, n);
    
    GenericValue GV = EE->runFunction(impl->functions["test"], Args);
    
    // import result of execution
    outs() << "Result: " << GV.IntVal << "\n";
}

LLVMBackend::~LLVMBackend()
{
    delete impl;
    delete Scope::GlobalScope;
}

LLVMBackend::LLVMBackend()
{
    impl = new LLVMBackendImpl;
}

llvm::Value* LLVMBackend::visit(JITAssignmentAst* ast)
{
    return nullptr;
}

void LLVMBackend::visit(BlockAst* ast)
{
    //if data == 123 then { data = 111 } else {data = 999}
    size_t totalBlock = ast->Blocks.size();
    for (size_t i = 0; i < totalBlock; ++i) {
        getAstItem(ast->Blocks.at(i));
    }
}

void LLVMBackend::visit(IfStatementAst* ast)
{
    this->getAstItem(ast->ControlOpt);
    
}

void LLVMBackend::visit(FunctionDefinetionAst* ast)
{
    
}

void LLVMBackend::visit(ForStatementAst* ast)
{
   
}

llvm::Value* LLVMBackend::visit(JITVariableAst* ast)
{
    return nullptr;
}

llvm::Value* LLVMBackend::visit(JITPrimativeAst* ast)
{
    return impl->getConstant(ast->Value);
}

llvm::Value* LLVMBackend::visit(JITControlAst* ast)
{
    return nullptr;
}

llvm::Value* LLVMBackend::visit(JITBinaryAst* ast)
{
    BACKEND_ITEM_TYPE rightType = BACKEND_ITEM_TYPE::INT;
    BACKEND_ITEM_TYPE leftType = BACKEND_ITEM_TYPE::INT;
    auto binaryResultType = BACKEND_ITEM_TYPE::INT;
    
    llvm::Value* left = getAstItem(ast->Left);
    llvm::Value* right = getAstItem(ast->Right);

    Instruction::BinaryOps instr;
    switch (ast->Op)
    {
    case PLUS:
        instr = Instruction::Add;
    break;

    case MINUS:
        instr = Instruction::Sub;
    break;

    case MULTIPLICATION:
        instr = Instruction::Mul;
    break;

    case DIVISION:
        instr = Instruction::FDiv;
    break;
    }

    return BinaryOperator::Create(instr, left, right);
}

llvm::Value* LLVMBackend::visit(JITStructAst* ast)
{
    return nullptr;
}

void LLVMBackend::visit(ReturnAst* ast)
{

}

llvm::Value* LLVMBackend::visit(JITParenthesesGroupAst* ast)
{
    return nullptr;
}

llvm::Value* LLVMBackend::visit(JITFunctionCallAst* ast)
{
    return nullptr;
}

llvm::Value* LLVMBackend::visit(JITUnaryAst* ast)
{
    return nullptr;
}

void LLVMBackend::visit(ExprStatementAst* ast)
{
    //ast->Expr->accept(this);
}

llvm::Value* LLVMBackend::visit(JITExprStatementAst* ast)
{
    return getAstItem(ast);
}

#endif
