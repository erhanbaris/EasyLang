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
#include "Vm.h"
#include "../Tests/LexerTests.h"
#include "../Tests/AstTests.h"
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


//#include "../Tests/InterpreterTests.h"

using namespace std;

int main(int argc, char_type* argv[]) {

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
			{
				console_out << result->Describe() << '\n';
				delete result;
			}

        } catch (exception& e) {
			console_out << _T("#ERROR ") << e.what() << '\n';
        }

		COLOR_GREY("easy > ");
	}

    getchar();
    return 0;
}
