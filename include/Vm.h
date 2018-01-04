#ifndef EASYLANG_VM_H
#define EASYLANG_VM_H

#include <stdio.h>
#include <vector>
#include <unordered_map>
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
             iADD = 1,
             iSUB,
             iMUL,
             iDIV,
			 iEQ,
             iLT,
             iLTE,
             iGT,
             iGTE,
             iAND,
             iOR,
             iDUP, // dublicate
             iPOP,
             iJMP, // jump
             iJIF, // if jump
             iJNIF, // if not jump
             iINC,
             iDINC,
             iLOAD, // Load variable
             iSTORE, // Store variable

             iCALL,
             iRETURN,

             iPUSH,
             iPRINT,
             iHALT)


class vm_system
{
public:
	vm_system();
	void execute(size_t* code, size_t len);
	size_t getUInt();

private:
	vm_system_impl* impl;
};


#endif //EASYLANG_VM_H
