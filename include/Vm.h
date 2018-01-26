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

class vm_array;
class vm_object;
class vm_system;
class vm_system_impl;
template <typename T> class vm_store;
template <typename T> class vm_stack;

typedef vm_object*(*VmMethod)(vm_system* vm);
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
        ARRAY,
        DICT,
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
        Pointer = b;
        Type = vm_object_type::STR;
    }

	vm_object(string_type const & b)
	{
		Pointer = new char[b.size()];
		memcpy(Pointer, b.c_str(), b.size());
		Type = vm_object_type::STR;
	}

	vm_object(vm_array const * array)
	{
		Pointer = const_cast<void*>((void*)array);
		Type = vm_object_type::ARRAY;
	}

	vm_object_type Type;

	union {
		bool Bool;
		int Int;
		double Double;
		void* Pointer{nullptr};
		VmMethodCallback Method;
	};
};

class vm_array{
public:
	mutable size_t Length{16};
	mutable size_t Indicator{0};
	mutable vm_object* Array {nullptr};

	void push(vm_object && obj)
	{
		Array[Indicator++] = std::move(obj);
		if (Indicator == Length)
			resizeTo(Length * 2);
	}

	void push(vm_object & obj)
	{
		Array[Indicator++] = obj;
		if (Indicator == Length)
			resizeTo(Length * 2);
	}

	vm_object* get(size_t index) const
	{
		return &Array[index];
	}

	void resizeTo(size_t newSize) const
	{
		auto* tmpNewArray = new vm_object[newSize];
		memcpy(tmpNewArray, Array, Length);
		Length = newSize;
	}

	vm_array()
	{
		Array = new vm_object[Length];
	}
	
	~vm_array()
	{
		/*if (Array != nullptr)
			delete[] Array;*/
	}
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

OPT_I2D, // 60
OPT_D2I, // 61
OPT_I2B, // 62
OPT_B2I, // 63
OPT_D2B, // 64
OPT_B2D, // 65
OPT_iPUSH_0, // 66
OPT_iPUSH_1, // 67
OPT_iPUSH_2, // 68
OPT_iPUSH_3, // 69
OPT_iPUSH_4, // 70
OPT_dPUSH_0, // 71
OPT_dPUSH_1, // 72
OPT_dPUSH_2, // 73
OPT_dPUSH_3, // 74
OPT_dPUSH_4, // 75
OPT_bPUSH_0, // 76
OPT_bPUSH_1, // 77
OPT_INVOKE, // 78
OPT_METHOD, // 79
OPT_INITARRAY, // 80
OPT_INITDICT, // 81
OPT_aPUSH, // 82
OPT_diPUSH, // 83
OPT_bADD, // 84
OPT_bSUB, // 85
OPT_bDIV, // 86
OPT_bMUL, // 87
OPT_aGET, // 88
OPT_diGET // 89
)


class vm_system
{
public:
    vm_system();
    ~vm_system();
    void execute(char* code, size_t len, size_t startIndex);
    void addMethod(string_type const & name, VmMethod method);
	void dumpOpcode(char* code, size_t len);
	void dumpStack();
    void dump(char* code, size_t len);
	size_t getUInt();
	vm_object* getObject();

private:
	vm_system_impl* impl;
};


#endif //EASYLANG_VM_H
