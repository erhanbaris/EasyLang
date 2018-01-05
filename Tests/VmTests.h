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

TEST_CASE("VM If statement tests") {
	vm_system vm;

	/*
	int status;
	if (2018 - 1985 == 33)
	status = 1;
	else
	status = 0;
	*/

	std::vector<size_t> codes{
		iPUSH, 2018,
		iPUSH, 1985,
		iSUB,
		iPUSH, 33,
		iEQ,
		iJIF, 13,
		iPUSH, 1,
		iHALT,
		iPUSH, 0,
		iHALT
	};
	vm.execute(&codes[0], codes.size());

	REQUIRE(vm.getUInt() == 1);
}

TEST_CASE("VM For statement tests") {
    vm_system vm;
    
    // int total = 100;
    // for (int i = 0; i < total; ++i) { }
    
    std::vector<size_t> codes{
        iPUSH, 100, // total
        iCALL, 5, // jump and create new stack
        iHALT,
        
        // Init variables
        iSTORE, 0, // save total
        iLOAD, 0, // get total to stack
        iPUSH, 0, // i = 0
        iSTORE, 1, // save i
        iLOAD, 1,
        iGT,
        iJIF, 29,
        iLOAD, 0,
        iLOAD, 1,
        iINC, // ++i
        iSTORE, 1,
        iLOAD, 1,
        iJMP, 15, // loop
        iLOAD, 1, // return i value
        iRETURN // clear stack
    };
    vm.execute(&codes[0], codes.size());
    
    REQUIRE(vm.getUInt() == 100);
}

TEST_CASE("VM Fibonacci tests") {
    vm_system vm;
    std::vector<size_t> codes{
        iPUSH, 10, // number
        iCALL, 5, // jump and create new stack
        iHALT,
        
        // Init variables
        iSTORE, 0, // save number

        /* if (n == 0) return 0; */
        iLOAD, 0, // get total to stack
        iPUSH, 0,
        iEQ,
        iJIF, 17,
        iPUSH, 0,
        iRETURN,

        /* if (n == 1) return 1; */
        iLOAD, 0, // get total to stack
        iPUSH, 1,
        iEQ,
        iJIF, 27,
        iPUSH, 1,
        iRETURN,

        /* return fibonacci(n-1) + fibonacci(n-2); */
        iLOAD, 0, // get total to stack
        iPUSH, 1,
        iSUB,
        iCALL, 5,

        iLOAD, 0, // get total to stack
        iPUSH, 2,
        iSUB,
        iCALL, 5,
        iADD,

        iRETURN };
    vm.execute(&codes[0], codes.size());
    size_t result = vm.getUInt();
    REQUIRE(result == 55);
}


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
			iLOAD_0, // get total to stack
			iPUSH, 0,
			iIF_EQ, 14,
			iPUSH, 0,
			iRETURN,

			/* if (n == 1) return 1; */
			iLOAD, 0, // get total to stack
			iPUSH, 1,
			iIF_EQ, 23,
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
