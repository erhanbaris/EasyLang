//https://github.com/catchorg/Catch2
//https://github.com/DeadMG/Wide/blob/master/Wide/Lexer/Lexer.cpp

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>
#define CATCH_CONFIG_RUNNER

#include "Catch.h"
#include "System.h"
#include "EasyEngine.h"
#include "Definitions.h"
#include "FunctionDispatch.h"

#include "../Tests/LexerTests.h"
#include "../Tests/AstTests.h"
#include "../Tests/InterpreterTests.h"


using namespace std;


static int test(int a, int b, float c)
{
    return a + b * (int)c;
}

static bool ErhanTest(std::string mesaj)
{
    std::cout << mesaj << '\n';
	return true;
}

std::unordered_map<string_type, Caller*> asd;

template <typename Ret, typename... Args>
static void AddFunc(string_type const& funcName, Ret(*func)(Args...))
{
    auto funcCaller = Func_Caller<Ret, Args...>(func);
    asd[funcName] = new CallerImpl<decltype(func)>(func);
}

int main(int argc, char* argv[]) {
    
    AddFunc("erhantest", &ErhanTest);
	AddFunc("test", &test);
    
    int a = 3, b = 4;
    float c = 5;
	Any sa = a;
	Any sb = b;
	Any sc = c;

	std::vector<Any> vec;
	string_type mesaj = _T("merhaba dünya");
	vec.push_back(mesaj);


	Any result2 = asd[_T("erhantest")]->Call(&vec[0]);

	vec[0].clear();
	vec.clear();

	vec.push_back(1);
	vec.push_back(999);
	vec.push_back(5.0001f);
	Any result1 = asd[_T("test")]->Call(&vec[0]);
	
	if (result1.is<int>())
		console_out << result1.cast<int>();

	System::WarmUp();
	// Unit tests
	Catch::Session().run(argc, argv);

    auto* engine = EasyEngine::Interpreter();

	string_type line;
	console_out << _T("EasyLang Interpreter\n\n");
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
