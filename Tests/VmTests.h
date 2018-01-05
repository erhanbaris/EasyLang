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
	std::vector<size_t> codes{
			iPUSH, 35, // number
			iCALL, 5, // jump and create new stack
			iHALT,

			// Init variables
			iSTORE_0, // save number

			/* if (n == 0) return 0; */
			iPUSH, 0,
			iIF_EQ, 13,
			iPUSH, 0,
			iRETURN,

			/* if (n == 1) return 1; */
			iLOAD, 0, // get total to stack
			iPUSH, 1,
			iIF_EQ, 22,
			iPUSH, 1,
			iRETURN,

			/* return fibonacci(n-1) + fibonacci(n-2); */
			iLOAD_0, // get total to stack
			iPUSH, 1,
			iSUB,
			iCALL, 5,

			iLOAD_0, // get total to stack
			iPUSH, 2,
			iSUB,
			iCALL, 5,
			iADD,

			iRETURN 
	};
	vm.execute(&codes[0], codes.size());

	high_resolution_clock::time_point t2 = high_resolution_clock::now();
	duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
	std::cout << "It took me " << time_span.count() << " seconds.";
	std::cout << std::endl;

	size_t result = vm.getUInt();
	REQUIRE(result == 9227465);
}

#endif //EASYLANG_VMTESTS_H
