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
typedef union vm_long_u { vm_char_t Chars[8];  long Long; } vm_long_t;
typedef union vm_int_u { vm_char_t Chars[4];  int Int; } vm_int_t;
typedef bool vm_bool_t;

class vm_object
{
public:
	enum class vm_object_type {
		EMPTY,
		INT,
		DOUBLE,
		BOOL,
		STR,
		NATIVE_CALL,
		CALL
	};

	vm_object()
	{
		Type = vm_object_type::EMPTY;
	}

	vm_object(int i)
	{
		Int = i;
		Type = vm_object_type::INT;
	}

	vm_object(double d)
	{
		Double = d;
		Type = vm_object_type::DOUBLE;
	}

	vm_object(bool b)
	{
		Bool = b;
		Type = vm_object_type::BOOL;
	}

	vm_object(char* b)
	{
		String = b;
		Type = vm_object_type::STR;
	}

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

	vm_object& operator=(char* right) {
		String = right;
		Type = vm_object_type::STR;
		return *this;
	}

    operator int()
    {
        return Int;
    }

    operator double()
    {
        return Double;
    }

    operator bool()
    {
        return Bool;
    }

	vm_object_type Type;

	union {
		bool Bool;
		int Int;
		double Double;
		char* String{nullptr};
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
OPT_OR, // 19
OPT_DUP, // 20
OPT_POP, // 21
OPT_JMP, // 22

OPT_IF_EQ, // 23

OPT_JIF, // 24
OPT_JNIF, // 25
OPT_INC, // 26
OPT_DINC, // 27

OPT_LOAD, // 28
OPT_LOAD_0, // 29
OPT_LOAD_1, // 30
OPT_LOAD_2, // 31
OPT_LOAD_3, // 32
OPT_LOAD_4, // 33

OPT_STORE, // 34
OPT_STORE_0, // 35
OPT_STORE_1, // 36
OPT_STORE_2, // 37
OPT_STORE_3, // 38
OPT_STORE_4, // 39

OPT_GLOAD, // 40
OPT_GLOAD_0, // 41
OPT_GLOAD_1, // 42
OPT_GLOAD_2, // 43
OPT_GLOAD_3, // 44
OPT_GLOAD_4, // 45

OPT_GSTORE, // 46
OPT_GSTORE_0, // 47
OPT_GSTORE_1, // 48
OPT_GSTORE_2, // 49
OPT_GSTORE_3, // 50
OPT_GSTORE_4, // 51

OPT_CALL, // 52
OPT_RETURN, // 53

OPT_iPUSH, // 54
OPT_dPUSH, // 55
OPT_bPUSH, // 56
OPT_sPUSH, // 57
OPT_PRINT, // 58
OPT_NEG, // 59

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
	void dumpOpcode(char* code, size_t len);
    void dump(char* code, size_t len);
	size_t getUInt();
	vm_object getObject();

private:
	vm_system_impl* impl;
};


#endif //EASYLANG_VM_H
