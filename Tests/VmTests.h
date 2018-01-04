#ifndef EASYLANG_VMTESTS_H
#define EASYLANG_VMTESTS_H

#define CATCH_CONFIG_RUNNER
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cmath>

#include "../include/Catch.h"
#include "../include/Vm.h"

using namespace std;

TEST_CASE("VM If statement tests") {
	vm_system vm;

	/*
	int status;
	if (2018 - 1985 == 33)
	status = 1;
	else
	status = 0;
	*/

	vm.execute({
		vm_inst::iPUSH, 2018,
		vm_inst::iPUSH, 1985,
		vm_inst::iSUB,
		vm_inst::iPUSH, 33,
		vm_inst::iEQ,
		vm_inst::iJIF, 13,
		vm_inst::iPUSH, 1,
		vm_inst::iHALT,
		vm_inst::iPUSH, 0,
		vm_inst::iHALT
	});

	REQUIRE(vm.getUInt() == 1);
}

TEST_CASE("VM For statement tests") {
	vm_system vm;

	// int total = 100;
	// for (int i = 0; i < total; ++i) { }
	vm.execute({
		vm_inst::iPUSH, 100, // total
		vm_inst::iCALL, 5, // jump and create new stack
		vm_inst::iHALT,

		// Init variables
		vm_inst::iSTORE, 0, // save total
		vm_inst::iLOAD, 0, // get total to stack
		vm_inst::iPUSH, 0, // i = 0
		vm_inst::iSTORE, 1, // save i
		vm_inst::iLOAD, 1,
		vm_inst::iGT,
		vm_inst::iJIF, 29,
		vm_inst::iLOAD, 0,
		vm_inst::iLOAD, 1,
		vm_inst::iINC, // ++i
		vm_inst::iSTORE, 1,
		vm_inst::iLOAD, 1,
		vm_inst::iJMP, 15, // loop
		vm_inst::iLOAD, 1, // return i value
		vm_inst::iRETURN // clear stack
	});

	REQUIRE(vm.getUInt() == 100);
}

#endif //EASYLANG_VMTESTS_H