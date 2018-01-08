#ifndef EASYLANG_VM_H
#define EASYLANG_VM_H

#include <stdio.h>
#include <vector>
#include <stdexcept>
#include "Definitions.h"
#include "Macros.h"

class VmException : public std::runtime_error
{
public:
    VmException(string_type const & message) : std::runtime_error(message.c_str())
    {
    }
};

class vm_object;
class vm_system_impl;
template <typename T> class vm_store;
template <typename T> class vm_stack;

DECLARE_ENUM(vm_inst,
iHALT, // 0
iADD, // 1
iSUB, // 2
iMUL, // 3
iDIV, // 4
iEQ, // 5
iLT, // 6
iLTE, // 7
iGT, // 8
iGTE, // 9
iAND, // 10
iOR, // 11
iDUP, // 12
iPOP, // 13
iJMP, // 14

iIF_EQ, // 15

iJIF, // 16
iJNIF, // 17
iINC, // 18
iDINC, // 19

iLOAD, // 20
iLOAD_0, // 21
iLOAD_1, // 22
iLOAD_2, // 23
iLOAD_3, // 24
iLOAD_4, // 25

iSTORE, // 26
iSTORE_0, // 27
iSTORE_1, // 28
iSTORE_2, // 29
iSTORE_3, // 30
iSTORE_4, // 31

iGLOAD, // 32
iGLOAD_0, // 33
iGLOAD_1, // 34
iGLOAD_2, // 35
iGLOAD_3, // 36
iGLOAD_4, // 37

iGSTORE, // 38
iGSTORE_0, // 39
iGSTORE_1, // 40
iGSTORE_2, // 41
iGSTORE_3, // 42
iGSTORE_4, // 43

iCALL, // 44
iRETURN, // 45

iPUSH, // 46
iPRINT // 47
)


class vm_system
{
public:
    vm_system();
    ~vm_system();
	void execute(size_t* code, size_t len, size_t startIndex);
	void dump(size_t* code, size_t len);
	size_t getUInt();

private:
	vm_system_impl* impl;
};


#endif //EASYLANG_VM_H
