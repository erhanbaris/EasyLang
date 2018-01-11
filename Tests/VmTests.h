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

using namespace std;
using namespace std::chrono;


TEST_CASE("VM Fibonacci tests 2") {

	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	vm_system vm;
	std::vector<char> codes{
			OPT_PUSH, 25, // number
			OPT_CALL, 5, // jump and create new stack
			OPT_HALT,

			// Init variables
			OPT_STORE_0, // save number
			OPT_LOAD_0,

			/* if (n == 0) return 0; */
			OPT_PUSH, 0,
			OPT_IF_EQ, 14,
			OPT_PUSH, 0,
			OPT_RETURN,

			/* if (n == 1) return 1; */
			OPT_LOAD, 0, // get total to stack
			OPT_PUSH, 1,
			OPT_IF_EQ, 23,
			OPT_PUSH, 1,
			OPT_RETURN,

			/* return fibonacci(n-1) + fibonacci(n-2); */
			OPT_LOAD_0, // get total to stack
			OPT_PUSH, 1,
			OPT_iSUB,
			OPT_CALL, 5,

			OPT_LOAD_0, // get total to stack
			OPT_PUSH, 2,
			OPT_iSUB,
			OPT_CALL, 5,
			OPT_iADD,

			OPT_RETURN 
	};
	vm.execute(&codes[0], codes.size(), 0);

	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
	std::cout << "It took me " << time_span.count() << " seconds.";
	std::cout << std::endl;

	size_t result = vm.getUInt();
	REQUIRE(result == 75025);
}

#endif //EASYLANG_VMTESTS_H
