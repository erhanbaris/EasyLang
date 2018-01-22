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

#include "../include/Catch.h"
#include "../include/Vm.h"
#include "../include/VmBackend.h"
#include "../include/VmEasyEngine.h"

using namespace std;
using namespace std::chrono;


TEST_CASE("VM Fibonacci tests 2") {
	auto* engine = new VmEasyEngine;
	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	auto* result = engine->Execute(_T("func fibonacci(num:int):int { if num <= 1 then return num left = fibonacci(num - 1) right = fibonacci(num - 2) return left + right } fibonacci(25)"));

	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
	std::cout << "It took me " << time_span.count() << " seconds.";
	std::cout << std::endl;

	REQUIRE(result->Integer == 75025);
	delete engine;
	delete result;
}

#endif //EASYLANG_VMTESTS_H
