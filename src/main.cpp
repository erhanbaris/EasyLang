//#define LLVM_ACTIVE

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>
#define CATCH_CONFIG_RUNNER

#include "Catch.h"
#include "System.h"
#include "EasyEngine.h"
#include "VmEasyEngine.h"
#include "InterpreterEasyEngine.h"
#include "Definitions.h"
#include "FunctionDispatch.h"

#include "VmBackend.h"
#include "InterpreterBackend.h"
#include "ASTs.h"
#include "Lexer.h"

#include "../Tests/LexerTests.h"
#include "../Tests/AstTests.h"
#include "../Tests/InterpreterTests.h"
#include "../Tests/VmTests.h"

using namespace std;

#ifdef LLVM_ACTIVE
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


static Function *CreateFibFunction(Module *M, LLVMContext &Context) {
    // Create the fib function and insert it into module M. This function is said
    // to return an int and take an int parameter.
    Function *FibF =
            cast<Function>(M->getOrInsertFunction("fib", Type::getInt32Ty(Context),
                                                  Type::getInt32Ty(Context)));

    // Add a basic block to the function.
    BasicBlock *BB = BasicBlock::Create(Context, "EntryBlock", FibF);

    // Get pointers to the constants.
    Value *One = ConstantInt::get(Type::getInt32Ty(Context), 1);
    Value *Two = ConstantInt::get(Type::getInt32Ty(Context), 2);

    // Get pointer to the integer argument of the add1 function...
    Argument *ArgX = &*FibF->arg_begin(); // Get the arg.
    ArgX->setName("AnArg");            // Give it a nice symbolic name for fun.

    // Create the true_block.
    BasicBlock *RetBB = BasicBlock::Create(Context, "return", FibF);
    // Create an exit block.
    BasicBlock* RecurseBB = BasicBlock::Create(Context, "recurse", FibF);

    // Create the "if (arg <= 2) goto exitbb"
    Value *CondInst = new ICmpInst(*BB, ICmpInst::ICMP_SLE, ArgX, Two, "cond");
    BranchInst::Create(RetBB, RecurseBB, CondInst, BB);

    // Create: ret int 1
    ReturnInst::Create(Context, One, RetBB);

    // create fib(x-1)
    Value *Sub = BinaryOperator::CreateSub(ArgX, One, "arg", RecurseBB);
    CallInst *CallFibX1 = CallInst::Create(FibF, Sub, "fibx1", RecurseBB);
    CallFibX1->setTailCall();

    // create fib(x-2)
    Sub = BinaryOperator::CreateSub(ArgX, Two, "arg", RecurseBB);
    CallInst *CallFibX2 = CallInst::Create(FibF, Sub, "fibx2", RecurseBB);
    CallFibX2->setTailCall();

    // fib(x-1)+fib(x-2)
    Value *Sum = BinaryOperator::CreateAdd(CallFibX1, CallFibX2,
                                           "addresult", RecurseBB);

    // Create the return instruction and add it to the basic block
    ReturnInst::Create(Context, Sum, RecurseBB);

    return FibF;
}
#endif

int main(int argc, char* argv[]) {
#ifdef LLVM_ACTIVE
    int n = argc > 1 ? atol(argv[1]) : 27;

    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    LLVMContext Context;

    std::unique_ptr<Module> Owner(new Module("test", Context));
    Module *M = Owner.get();

    Function *FibF = CreateFibFunction(M, Context);

    std::string errStr;
    ExecutionEngine *EE =
            EngineBuilder(std::move(Owner))
                    .setErrorStr(&errStr)
                    .create();

    if (!EE) {
        errs() << argv[0] << ": Failed to construct ExecutionEngine: " << errStr
               << "\n";
        return 1;
    }

    errs() << "verifying... ";
    if (verifyModule(*M)) {
        errs() << argv[0] << ": Error constructing function!\n";
        return 1;
    }

    errs() << "OK\n";
    errs() << "We just constructed this LLVM module:\n\n---------\n" << *M;
    errs() << "---------\nstarting fibonacci(" << n << ") with JIT...\n";

    // Call the Fibonacci function with argument n:
    std::vector<GenericValue> Args(1);
    Args[0].IntVal = APInt(32, n);
    GenericValue GV = EE->runFunction(FibF, Args);

    // import result of execution
    outs() << "Result: " << GV.IntVal << "\n";

#endif

    System::WarmUp();
	// Unit tests
	Catch::Session().run(argc, argv);

	auto* engine = new VmEasyEngine;
    
	string_type line;
	console_out << _T("EasyLang Virtual Machine\n\n");
	console_out << _T("easy > ");
	while (std::getline(console_in, line))
	{
        try {
			engine->Execute(line);
        } catch (exception& e) {
			console_out << _T("#ERROR ") << e.what() << '\n';
        }

		console_out << _T("easy > ");
	}

    getchar();
    return 0;
}
