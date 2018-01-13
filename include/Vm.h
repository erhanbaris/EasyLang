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

class vm_system;
class vm_system_impl;
template <typename T> class vm_store;
template <typename T> class vm_stack;

typedef void(*VmMethodCallback)(vm_system* vm, size_t totalArgs);
typedef char vm_char_t;
typedef union vm_double_u { vm_char_t Chars[8];  double Double; } vm_double_t;
typedef union vm_long_u { vm_char_t Chars[4];  long Long; } vm_long_t;
typedef union vm_int_u { vm_char_t Chars[2];  int Int; } vm_int_t;
typedef bool vm_bool_t;

class vm_object
{
public:
	enum class vm_object_type {
		INT,
		DOUBLE,
		BOOL,
		STR,
		NATIVE_CALL,
		CALL
	};

	vm_object& operator=(int right) {
		Int = right;
		Type = vm_object_type::INT;
		return *this;
	}

	vm_object& operator=(double right) {
		Double = right;
		Type = vm_object_type::DOUBLE;
		return *this;
	}

	vm_object& operator=(bool right) {
		Bool = right;
		Type = vm_object_type::BOOL;
		return *this;
	}

	operator int()
	{
		return Int;
	}

	vm_object_type Type;

	union {
		bool Bool;
		int Int;
		double Double;
		string_type* String;
		VmMethodCallback Method;
	};
};

DECLARE_ENUM(vm_inst,
OPT_HALT, // 0
OPT_iADD, // 1
OPT_dADD, // 2
OPT_lADD, // 3

OPT_iSUB, // 4
OPT_dSUB, // 5
OPT_lSUB, // 6

OPT_iMUL, // 7
OPT_dMUL, // 8
OPT_lMUL, // 9

OPT_iDIV, // 10
OPT_dDIV, // 11
OPT_lDIV, // 12

OPT_EQ, // 13
OPT_LT, // 14
OPT_LTE, // 15
OPT_GT, // 16
OPT_GTE, // 17

OPT_AND, // 18
OPT_OR, // 11
OPT_DUP, // 12
OPT_POP, // 13
OPT_JMP, // 14

OPT_IF_EQ, // 15

OPT_JIF, // 16
OPT_JNIF, // 17
OPT_INC, // 18
OPT_DINC, // 19

OPT_LOAD, // 20
OPT_LOAD_0, // 21
OPT_LOAD_1, // 22
OPT_LOAD_2, // 23
OPT_LOAD_3, // 24
OPT_LOAD_4, // 25

OPT_STORE, // 26
OPT_STORE_0, // 27
OPT_STORE_1, // 28
OPT_STORE_2, // 29
OPT_STORE_3, // 30
OPT_STORE_4, // 31

OPT_GLOAD, // 32
OPT_GLOAD_0, // 33
OPT_GLOAD_1, // 34
OPT_GLOAD_2, // 35
OPT_GLOAD_3, // 36
OPT_GLOAD_4, // 37

OPT_GSTORE, // 38
OPT_GSTORE_0, // 39
OPT_GSTORE_1, // 40
OPT_GSTORE_2, // 41
OPT_GSTORE_3, // 42
OPT_GSTORE_4, // 43

OPT_CALL, // 44
OPT_RETURN, // 45

OPT_PUSH, // 46
OPT_PRINT, // 47
OPT_NEG, // 48

OPT_I2D,
OPT_D2I,
OPT_I2B,
OPT_B2I,
OPT_D2B,
OPT_B2D
)


class vm_system
{
public:
    vm_system();
    ~vm_system();
	void execute(char* code, size_t len, size_t startIndex);
	void dump(char* code, size_t len);
	size_t getUInt();

private:
	vm_system_impl* impl;
};


#endif //EASYLANG_VM_H
