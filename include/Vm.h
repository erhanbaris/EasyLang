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
    VmException(string_type const & message) : std::runtime_error(AS_CHAR(message.c_str()))
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
typedef char_type vm_char_t;
typedef union vm_double_u { vm_char_t Chars[8];  double Double; } vm_double_t;
typedef union vm_long_u { vm_char_t Chars[8];  long Long; } vm_long_t;
typedef union vm_int_u { vm_char_t Chars[4];  int Int; } vm_int_t;
typedef bool vm_bool_t;

class vm_object
{
public:
	enum class vm_object_type : char {
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

	~vm_object()
	{
		if (Type == vm_object_type::STR)
			delete (char_type*)Pointer;
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

    vm_object(char_type* b)
    {
        Pointer = b;
        Type = vm_object_type::STR;
    }

	vm_object(string_type const & b)
	{
		Pointer = new char_type[b.size()];
		memcpy(Pointer, b.c_str(), b.size());
		Type = vm_object_type::STR;
	}

	vm_object(vm_array const * array)
	{
		Pointer = const_cast<void*>((void*)array);
		Type = vm_object_type::ARRAY;
	}
    
    vm_object& operator=(vm_object const & other)
    {
        Type = other.Type;
        Double = other.Double;
        return *this;
    }

	static vm_object* CreateFromString(char_type* b)
	{
		vm_object* obj = new vm_object;
		obj->Type = vm_object::vm_object_type::STR;
		obj->Pointer = b;
		return obj;
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
OPT_ADD, // 1
OPT_SUB, // 2
OPT_MUL, // 3
OPT_DIV, // 4

OPT_EQ, // 5
OPT_LT, // 6
OPT_LTE, // 7
OPT_GT, // 8
OPT_GTE, // 9

OPT_AND, // 10
OPT_OR, // 11
OPT_DUP, // 12
OPT_POP, // 13
OPT_JMP, // 14

OPT_CONST_STR, // 15
OPT_CONST_INT, // 16
OPT_CONST_INT_0, // 16
OPT_CONST_INT_1, // 16
OPT_CONST_BOOL_TRUE, // 17
OPT_CONST_BOOL_FALSE, // 17
OPT_CONST_DOUBLE, // 18
OPT_CONST_DOUBLE_0, // 18
OPT_CONST_DOUBLE_1, // 18

OPT_DELETE, // 19

OPT_IF_EQ, // 20
OPT_JIF, // 21
OPT_JNIF, // 22
OPT_INC, // 23
OPT_DINC, // 24

OPT_LOAD, // 25
OPT_LOAD_0, // 27
OPT_LOAD_1, // 27
OPT_LOAD_2, // 27
OPT_LOAD_3, // 27
OPT_LOAD_4, // 27
OPT_LOAD_5, // 27
OPT_LOAD_6, // 27
OPT_LOAD_7, // 27
OPT_LOAD_8, // 27
OPT_LOAD_9, // 27
OPT_LOAD_10, // 27
OPT_LOAD_11, // 27
OPT_LOAD_12, // 27
OPT_LOAD_13, // 27
OPT_LOAD_14, // 27
OPT_LOAD_15, // 27
OPT_LOAD_16, // 27

OPT_STORE, // 26
OPT_STORE_0, // 26
OPT_STORE_1, // 26
OPT_STORE_2, // 26
OPT_STORE_3, // 26
OPT_STORE_4, // 26
OPT_STORE_5, // 26
OPT_STORE_6, // 26
OPT_STORE_7, // 26
OPT_STORE_8, // 26
OPT_STORE_9, // 26
OPT_STORE_10, // 26
OPT_STORE_11, // 27
OPT_STORE_12, // 27
OPT_STORE_13, // 27
OPT_STORE_14, // 27
OPT_STORE_15, // 27
OPT_STORE_16, // 27

OPT_GLOAD, // 27
OPT_GLOAD_0, // 27
OPT_GLOAD_1, // 27
OPT_GLOAD_2, // 27
OPT_GLOAD_3, // 27
OPT_GLOAD_4, // 27
OPT_GLOAD_5, // 27
OPT_GLOAD_6, // 27
OPT_GLOAD_7, // 27
OPT_GLOAD_8, // 27
OPT_GLOAD_9, // 27
OPT_GLOAD_10, // 27
OPT_GLOAD_11, // 27
OPT_GLOAD_12, // 27
OPT_GLOAD_13, // 27
OPT_GLOAD_14, // 27
OPT_GLOAD_15, // 27
OPT_GLOAD_16, // 27

OPT_GSTORE, // 28
OPT_GSTORE_0, // 26
OPT_GSTORE_1, // 26
OPT_GSTORE_2, // 26
OPT_GSTORE_3, // 26
OPT_GSTORE_4, // 26
OPT_GSTORE_5, // 26
OPT_GSTORE_6, // 26
OPT_GSTORE_7, // 26
OPT_GSTORE_8, // 26
OPT_GSTORE_9, // 26
OPT_GSTORE_10, // 26
OPT_GSTORE_11, // 27
OPT_GSTORE_12, // 27
OPT_GSTORE_13, // 27
OPT_GSTORE_14, // 27
OPT_GSTORE_15, // 27
OPT_GSTORE_16, // 27

OPT_CALL, // 29
OPT_RETURN, // 30

OPT_PUSH, // 31
OPT_PRINT, // 32
OPT_NEG, // 33

OPT_CALL_NATIVE, // 34
OPT_METHOD_DEF, // 35
OPT_INITARRAY, // 36
OPT_INITDICT // 37
)


class vm_system
{
public:
    vm_system();
    ~vm_system();
    void execute(char_type* code, size_t len, size_t startIndex);
    void addMethod(string_type const & name, VmMethod method);
	void dumpOpcode(char_type* code, size_t len);
	void dumpStack();
    void dump(char_type* code, size_t len);
	size_t getUInt();
	vm_object const* getObject();

private:
	vm_system_impl* impl;
};


#endif //EASYLANG_VM_H
