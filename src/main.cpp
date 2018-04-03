//#define LLVM_ACTIVE
#define NOMINMAX
#include <fstream>

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <limits>
#define CATCH_CONFIG_RUNNER

#include "Catch.h"
#include "System.h"
#include "EasyEngine.h"

#ifdef EASYLANG_JIT_ACTIVE
#include "LLVMEasyEngine.h"
#define ENGINE LLVMEasyEngine
#else 
#include "VmEasyEngine.h"
#include "VmBackend.h"
#include "../Tests/VmTests.h"
//#include "InterpreterBackend.h"
//#include "InterpreterEasyEngine.h"
#define ENGINE VmEasyEngine
#endif

#include "Definitions.h"
#include "FunctionDispatch.h"


#include "ASTs.h"
#include "Lexer.h"
#include "Console.h"

#include "../Tests/LexerTests.h"
#include "../Tests/AstTests.h"
//#include "../Tests/InterpreterTests.h"

using namespace std;

#ifdef EASYLANG_JIT_ACTIVE
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
            cast<Function>(M->getOrInsertFunction("fib", llvm::Type::getInt32Ty(Context), llvm::Type::getInt32Ty(Context)));

    // Add a basic block to the function.
    BasicBlock *BB = BasicBlock::Create(Context, "EntryBlock", FibF);

    // Get pointers to the constants.
    Value *One = ConstantInt::get(llvm::Type::getInt32Ty(Context), 1);
    Value *Two = ConstantInt::get(llvm::Type::getInt32Ty(Context), 2);

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

int main(int argc, char_type* argv[]) {
#ifdef EASYLANG_JIT_ACTIVE
    int n = 32;

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

	auto* engine = new ENGINE;
	if (argc == 2)
	{
		string_type file(argv[1]);
		file_stream codeFile(file);
		if (!codeFile.is_open())
		{
			console_out << _T("Unable to open file");
			return 0;
		}

		std::vector<char_type> opcodes;
		if (file.substr(file.find_last_of(_T(".")) + 1) == _T("ea")) {
			string_type line;
			string_stream stream;
			while (getline(codeFile, line))
				stream << line;

			codeFile.close(); 
			
			engine->Compile(stream.str(), opcodes);
			fstream opcodeFile(file + _T("c"), ios::out | ios::binary | ios::trunc);

			if (opcodeFile.is_open())
			{
				opcodeFile.seekp(0);
				opcodeFile.write(AS_CHAR(&opcodes[0]), opcodes.size());
				opcodeFile.close();
			}
			else
			{
				console_out << _T("Unable to open file");
				return 0;
			}

			engine->Execute(opcodes);
		}
		else if (file.substr(file.find_last_of(_T(".")) + 1) == _T("eac")) {
			std::vector<char_type> codes;

			while (!codeFile.eof()) {
				codes.push_back(codeFile.get());
			}

			engine->Execute(codes);
		}

		return 0;
	}

    
	string_type line;
	console_out << _T("EasyLang Virtual Machine\n\n");
	COLOR_GREY("easy > ");
	while (std::getline(console_in, line))
	{
        try {
			auto* result = engine->Execute(line);
			if (result != nullptr)
				delete result;

        } catch (exception& e) {
			console_out << _T("#ERROR ") << e.what() << '\n';
        }

		COLOR_GREY("easy > ");
	}

    getchar();
    return 0;
}
