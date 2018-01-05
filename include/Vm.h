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

	     iIF_EQ, // if jump

         iJIF, // if jump
         iJNIF, // if not jump
         iINC,
         iDINC,
         
         iLOAD, // Load variable
         iLOAD_0, // Load variable 0
         iLOAD_1, // Load variable 1
         iLOAD_2, // Load variable 2
         iLOAD_3, // Load variable 3
         iLOAD_4, // Load variable 4
         
         iSTORE, // Store variable 
         iSTORE_0, // Store variable 0
         iSTORE_1, // Store variable 1
         iSTORE_2, // Store variable 2
         iSTORE_3, // Store variable 3
         iSTORE_4, // Store variable 4
         
         iCALL,
         iRETURN,
         
         iPUSH,
         iPRINT,
         iHALT)


class vm_system
{
public:
    vm_system();
    ~vm_system();
	void execute(size_t* code, size_t len);
	size_t getUInt();

private:
	vm_system_impl* impl;
};


#endif //EASYLANG_VM_H
