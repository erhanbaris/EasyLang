#ifndef EASYLANG_VMTESTS_H
#define EASYLANG_VMTESTS_H

#define CATCH_CONFIG_RUNNER
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>
#include <ctime>
#include <ratio>
#include <chrono>

#ifdef EASYLANG_JIT_ACTIVE
#include "LLVMEasyEngine.h"
#include "../include/LLVMBackend.h"
#include "../include/LLVMEasyEngine.h"
#define ENGINE LLVMEasyEngine
#else 
#include "../include/VmBackend.h"
#include "../include/VmEasyEngine.h"
#define ENGINE VmEasyEngine
#endif


using namespace std;
using namespace std::chrono;


TEST_CASE("VM Fibonacci tests") {
	auto* engine = new ENGINE;
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	engine->Execute(_T("func fibonacci(num){     if num == 1 || num == 2 then return 1    return fibonacci(num - 1) + fibonacci(num - 2) }"));
	auto* result = engine->Execute(_T("fibonacci(25)"));

	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
	std::cout << "It took me " << time_span.count() << " seconds.";
	std::cout << std::endl;

	REQUIRE(result->Integer == 75025);
	delete engine;
	delete result;
}

TEST_CASE("number tests") {
    auto *engine = new ENGINE;

    REQUIRE(engine->Execute(_T("1 == 1.0"))->Bool == true);
    REQUIRE(engine->Execute(_T("1.0 == 1"))->Bool == true);
    REQUIRE(engine->Execute(_T("-1"))->Integer == -1);
    REQUIRE(engine->Execute(_T("-1.1"))->Double == -1.1);
    REQUIRE(engine->Execute(_T("-1.1 < 1"))->Bool == true);
    REQUIRE(engine->Execute(_T("-1.1 <= 1"))->Bool == true);
    REQUIRE(engine->Execute(_T("-1.1 > 1"))->Bool == false);
    REQUIRE(engine->Execute(_T("-1.1 >= 1"))->Bool == false);
    REQUIRE(engine->Execute(_T("60-100"))->Integer == -40);
    REQUIRE(engine->Execute(_T("(40 - (10 * 10))"))->Integer == -60);
	REQUIRE(engine->Execute(_T("((10 - 10) - (10 * 10))"))->Integer == -100);
	REQUIRE(engine->Execute(_T("(40 - (10 * 10) * 2)"))->Integer == -160);
	REQUIRE(engine->Execute(_T("((10 - 10) * 40)"))->Integer == 0);
	REQUIRE(engine->Execute(_T("7 + 3 * (10 / (12 / (3 + 1) - 1))"))->Integer == 22);
	REQUIRE(engine->Execute(_T("2 + 7 * 4"))->Integer == 30);

    delete engine;
}

TEST_CASE("bool tests") {
	auto* engine = new ENGINE;
	
	REQUIRE(engine->Execute(_T("true == true"))->Bool == true);
	REQUIRE(engine->Execute(_T("false == false"))->Bool == true);
	REQUIRE(engine->Execute(_T("true == false"))->Bool == false);
	REQUIRE(engine->Execute(_T("false == true"))->Bool == false);

	REQUIRE(engine->Execute(_T("true != true"))->Bool == false);
	REQUIRE(engine->Execute(_T("false != false"))->Bool == false);
	REQUIRE(engine->Execute(_T("true != false"))->Bool == true);
	REQUIRE(engine->Execute(_T("false != true"))->Bool == true);

	REQUIRE(engine->Execute(_T("true && true"))->Bool == true);
	REQUIRE(engine->Execute(_T("false && false"))->Bool == false);
	REQUIRE(engine->Execute(_T("true && false"))->Bool == false);
	REQUIRE(engine->Execute(_T("false && true"))->Bool == false);

	REQUIRE(engine->Execute(_T("true || true"))->Bool == true);
	REQUIRE(engine->Execute(_T("false || false"))->Bool == false);
	REQUIRE(engine->Execute(_T("true || false"))->Bool == true);
	REQUIRE(engine->Execute(_T("false || true"))->Bool == true);

	REQUIRE(engine->Execute(_T("true < true"))->Bool == false);
	REQUIRE(engine->Execute(_T("false < true"))->Bool == true);
	REQUIRE(engine->Execute(_T("true < false"))->Bool == false);
	REQUIRE(engine->Execute(_T("false < false"))->Bool == false);
	REQUIRE(engine->Execute(_T("true <= false"))->Bool == false);
	REQUIRE(engine->Execute(_T("false <= true"))->Bool == true);
	REQUIRE(engine->Execute(_T("false <= false"))->Bool == true);
	REQUIRE(engine->Execute(_T("true <= true"))->Bool == true);

	REQUIRE(engine->Execute(_T("true > true"))->Bool == false);
	REQUIRE(engine->Execute(_T("false > true"))->Bool == false);
	REQUIRE(engine->Execute(_T("true > false"))->Bool == true);
	REQUIRE(engine->Execute(_T("false > false"))->Bool == false);
	REQUIRE(engine->Execute(_T("true >= false"))->Bool == true);
	REQUIRE(engine->Execute(_T("false >= true"))->Bool == false);
	REQUIRE(engine->Execute(_T("false >= false"))->Bool == true);
	REQUIRE(engine->Execute(_T("true >= true"))->Bool == true);

	delete engine;
}


TEST_CASE("string tests") {
	auto* engine = new ENGINE;

	REQUIRE(engine->Execute(_T("\"erhan\" == \"erhan\""))->Bool == true);

	REQUIRE(engine->Execute(_T("\"1\" == 1"))->Bool == true);
	REQUIRE(engine->Execute(_T("\"1.2\" == 1.2"))->Bool == true);

	REQUIRE(engine->Execute(_T("1 == \"1\""))->Bool == true);
	REQUIRE(engine->Execute(_T("1.2 == \"1.2\""))->Bool == true);

	REQUIRE(engine->Execute(_T("true == \"true\""))->Bool == true);
	REQUIRE(engine->Execute(_T("false == \"true\""))->Bool == false);
	REQUIRE(engine->Execute(_T("false == \"false\""))->Bool == true);
	REQUIRE(engine->Execute(_T("true == \"false\""))->Bool == false);

	REQUIRE(engine->Execute(_T("\"true\" == true"))->Bool == true);
	REQUIRE(engine->Execute(_T("\"true\" == false"))->Bool == false);
	REQUIRE(engine->Execute(_T("\"false\" == false"))->Bool == true);
	REQUIRE(engine->Execute(_T("\"false\" == true"))->Bool == false);


	REQUIRE(engine->Execute(_T("data = 0 if data > 1 then true else false"))->Bool == false);
	REQUIRE(engine->Execute(_T("data = 10 if data > 1 then true else false"))->Bool == true);
	REQUIRE(engine->Execute(_T("if \"erhan\" == \"erhan\" then true else false"))->Bool == true);
	REQUIRE(engine->Execute(_T("if \"erhan\" != \"erhan\" then true else false"))->Bool == false);
	REQUIRE(engine->Execute(_T("if 1 < -0.1 then true else false"))->Bool == false);
	REQUIRE(engine->Execute(_T("if \"true\" == true then true else false"))->Bool == true);
	REQUIRE((*engine->Execute(_T("(\"erhan\" * 2)"))->String) == "erhanerhan");


	delete engine;
}

#endif //EASYLANG_VMTESTS_H
