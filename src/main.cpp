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

#include "../Tests/LexerTests.h"
#include "../Tests/AstTests.h"
#include "../Tests/InterpreterTests.h"


using namespace std;


template <unsigned N>
struct apply
{
    template <typename Ret, typename... Args, typename... ArgsT>
    static Ret call(Ret(func)(Args...), void* v, ArgsT... args)
    {
        return apply<N-1>::call(func, v, args...,
                                static_cast<typename std::tuple_element<sizeof...(args),
                                std::tuple<Args...>>::type>(((int*)v)[sizeof...(ArgsT)]));
    }
};

template<>
struct apply<0>
{
    template <typename Ret, typename... Args, typename... ArgsT>
    static Ret call(Ret(func)(Args...), void* v, ArgsT... args)
    {
        return func(args...);
    }
};

template <typename Ret, typename... Args>
static Ret call(Ret(func)(Args...), void* v)
{
    return apply<sizeof...(Args)>::call(func, v);
}

static int test(int a, int b, float c)
{
    return a + b * (int)c;
}

static int ErhanTest(std::string mesaj)
{
    std::cout << mesaj << '\n';
}


template <typename Ret, typename... Args>
void Func(Ret(*func)(Args...))
{
    
}

class TestClass
{
public:
    virtual void Do(int* v) = 0;
};

template<typename Callable>
class TestClassExt : public TestClass
{
public:
    TestClassExt(Callable f)
    :
    m_f(std::move(f))
    {
    }
    
    void Do(int* v) override
    {
        call(m_f, v);
    }
    
private:
    Callable m_f;
};

template<typename Ret, typename ... Param>
struct Fun_Caller
{
    explicit Fun_Caller(Ret( * t_func)(Param...) ) : m_func(t_func) {}
    
    template<typename ... Inner>
    Ret operator()(Inner&& ... inner) const {
        return (m_func)(std::forward<Inner>(inner)...);
    }
    
    Ret(*m_func)(Param...);
};

std::unordered_map<string_type, TestClass*> asd;

template <typename Ret, typename... Args>
static void AddFunc(Ret(*func)(Args...))
{
    auto funcCaller = Fun_Caller<Ret, Args...>(func);
    asd[_T("test")] = new TestClassExt<decltype(func)>(func);
}

int main(int argc, char* argv[]) {
    
    AddFunc(&ErhanTest);
    
    int a = 3, b = 4;
    float c = 5;
    int* v = new int[3];
    v[0] = *(int*)&a;
    v[1] = *(int*)&b;
    v[2] = *(float*)&c;

    //asd[_T("test")]

    std::cout << call(test, v) << std::endl;
    return 0;
    
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
