#include "Vm.h"
#include "Console.h"
#include <unordered_map>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef _WIN32
# include <windows.h>
#else
# include <sys/time.h>
# include <unistd.h>
#endif

#define PRINT_ACTIVE 0
#define PERFORMANCE_MODE 1

#if PRINT_ACTIVE == 1
#define PRINT_OPCODE() console_out << _T(" > ") << vm_instToString((vm_inst)*code) << '\n';
#define PRINT_STACK() console_out << _T("STACK: ") << stackIndex << _T(" " ) << __LINE__ << '\n';
#define PRINT_AND_CHECK_STACK() PRINT_STACK(); if (stackIndex == 0) { dumpOpcode(startPoint, len); dump(startPoint, len); __asm { int 3 } }
#else
#define PRINT_OPCODE() 
#define PRINT_STACK() 
#define PRINT_AND_CHECK_STACK()
#endif

#if PERFORMANCE_MODE == 0
#define FUNC_BEGIN() [&]{
#define FUNC_END() }();
#else 
#define FUNC_BEGIN()
#define FUNC_END()
#endif

#define STORE(index, obj) FUNC_BEGIN() *(currentStore->variables + (index)) = obj; FUNC_END()
#define LOAD(index) ((vm_object) currentStore[ index ])

#define GSTORE(index, obj) FUNC_BEGIN() *(globalStore.variables + (index)) = obj; FUNC_END()
#define GLOAD(index) ((vm_object) globalStore.variables[ index ])


#define PEEK_INDEX(index) (currentStack[stackIndex - index ])
#define PEEK_INDEX_PTR(index) (&currentStack[stackIndex - index ])

#define PEEK() (PEEK_INDEX(1))
#define PEEK_PTR() (PEEK_INDEX_PTR(1))
#define POP() (currentStack[--stackIndex])
#define POP_AS(type) (currentStack[--stackIndex]. type )
#define SET(obj) currentStack[stackIndex - 1] = obj

#define OPERATION(currentStack, stackIndex, op, type) currentStack [ stackIndex - 2]. type = currentStack [ stackIndex - 2]. type ##op currentStack [ stackIndex - 1]. type ;\
    PRINT_AND_CHECK_STACK();\
    -- stackIndex ;

#define PUSH_WITH_STACK(currentStack, stackIndex, type, obj) FUNC_BEGIN() currentStack [ stackIndex ]. type = obj . type ; PRINT_STACK(); ++ stackIndex; FUNC_END()
#define PUSH_WITH_INIT(obj) currentStack [ stackIndex ] = vm_object( obj ) ; PRINT_STACK(); ++ stackIndex;
#define PUSH_WITH_EMPTY() currentStack [ stackIndex ] = vm_object( ) ; PRINT_STACK(); ++ stackIndex;
#define PUSH(type, obj) currentStack [ stackIndex ]. type = obj ; PRINT_STACK(); ++ stackIndex;
#define PUSH_WITH_ASSIGN(obj) currentStack [ stackIndex ] = obj ; PRINT_STACK(); ++ stackIndex;
#define INC(currentStack, stackIndex, type) currentStack [ stackIndex - 1]. type = currentStack [ stackIndex - 1]. type + 1;
#define DINC(currentStack, stackIndex, type) currentStack [ stackIndex - 1]. type = currentStack [ stackIndex - 1]. type - 1;
#define NEG(currentStack, stackIndex, type) currentStack [ stackIndex - 1]. type =  currentStack [ stackIndex - 1]. type * -1;
#define LOAD_AND_PUSH(index) FUNC_BEGIN() { currentStack[stackIndex].Double = (currentStore->variables + index )->Double;\
    currentStack[stackIndex].Type = (currentStore->variables + index )->Type;\
    PRINT_STACK(); \
    ++ stackIndex; } FUNC_END()

#define STACK_DINC() PRINT_AND_CHECK_STACK(); -- stackIndex ;
#define GET_ITEM(index, type) currentStack[stackIndex - index ]. type
#define OPERATION_ADD(var, type) FUNC_BEGIN() GET_ITEM(2, var) = GET_ITEM(2, var) +  GET_ITEM(1, var); GET_ITEM(2, Type) = type; STACK_DINC() FUNC_END()
#define OPERATION_SUB(var, type) FUNC_BEGIN() GET_ITEM(2, var) = GET_ITEM(2, var) -  GET_ITEM(1, var); GET_ITEM(2, Type) = type; STACK_DINC() FUNC_END()
#define OPERATION_MUL(var, type) FUNC_BEGIN() GET_ITEM(2, var) = GET_ITEM(2, var) *  GET_ITEM(1, var); GET_ITEM(2, Type) = type; STACK_DINC() FUNC_END()
#define OPERATION_DIV(var, type) FUNC_BEGIN() GET_ITEM(2, var) = GET_ITEM(2, var) /  GET_ITEM(1, var); GET_ITEM(2, Type) = type; STACK_DINC() FUNC_END()
#define OPERATION_EQ( var, type, condType) FUNC_BEGIN() GET_ITEM(2, var) = GET_ITEM(2, condType) == GET_ITEM(1, condType); GET_ITEM(2, Type) = type; STACK_DINC() FUNC_END()
#define OPERATION_LT( var, type, condType) FUNC_BEGIN() GET_ITEM(2, var) = GET_ITEM(2, condType) <  GET_ITEM(1, condType); GET_ITEM(2, Type) = type; STACK_DINC() FUNC_END()
#define OPERATION_LTE(var, type, condType) FUNC_BEGIN() GET_ITEM(2, var) = GET_ITEM(2, condType) <= GET_ITEM(1, condType); GET_ITEM(2, Type) = type; STACK_DINC() FUNC_END()
#define OPERATION_GT( var, type, condType) FUNC_BEGIN() GET_ITEM(2, var) = GET_ITEM(2, condType) >  GET_ITEM(1, condType); GET_ITEM(2, Type) = type; STACK_DINC() FUNC_END()
#define OPERATION_GTE(var, type, condType) FUNC_BEGIN() GET_ITEM(2, var) = GET_ITEM(2, condType) >= GET_ITEM(1, condType); GET_ITEM(2, Type) = type; STACK_DINC() FUNC_END()
#define OPERATION_AND(var, type) FUNC_BEGIN() GET_ITEM(2, var) = GET_ITEM(2, var) && GET_ITEM(1, var); GET_ITEM(2, Type) = type; STACK_DINC() FUNC_END()
#define OPERATION_OR( var, type) FUNC_BEGIN() GET_ITEM(2, var) = GET_ITEM(2, var) || GET_ITEM(1, var); GET_ITEM(2, Type) = type; STACK_DINC() FUNC_END()
#define IS_EQUAL(condType) GET_ITEM(2, condType) == GET_ITEM(1, condType)
#define CONVERT(from, to, toType) currentStack[stackIndex - 1]. to = currentStack[stackIndex - 1]. from ; currentStack[stackIndex - 1].Type = toType ;

#define GLOAD_PRE(index) opt_GLOAD_##index:\
++code;\
PRINT_OPCODE();\
PUSH_WITH_ASSIGN(GLOAD( index ));\
GOTO_OPCODE();

#define GSTORE_PRE(index) opt_GSTORE_##index:\
++code;\
PRINT_OPCODE();\
GSTORE( index , POP());\
GOTO_OPCODE();

#define LOAD_PRE(index) opt_LOAD_##index:\
++code;\
PRINT_OPCODE();\
LOAD_AND_PUSH( index );\
GOTO_OPCODE();

#define STORE_PRE(index) opt_STORE_##index:\
++code;\
PRINT_OPCODE();\
STORE( index , POP());\
GOTO_OPCODE();



#define ASSIGN_8(data, code)\
    data [7] = *( code + 0);\
    data [6] = *( code + 1);\
    data [5] = *( code + 2);\
    data [4] = *( code + 3);\
    data [3] = *( code + 4);\
    data [2] = *( code + 5);\
    data [1] = *( code + 6);\
    data [0] = *( code + 7);\
    code += 8;

#define ASSIGN_4(data, code)\
    data [3] = *( code + 0);\
    data [2] = *( code + 1);\
    data [1] = *( code + 2);\
    data [0] = *( code + 3);\
    code += 4;

#define ASSIGN_3(data, code)\
    data [2] = *( code + 0);\
    data [1] = *( code + 1);\
    data [0] = *( code + 2);\
    code += 3;

#define ASSIGN_2(data, code)\
    data [1] = *( code + 0);\
    data [0] = *( code + 1);\
    code += 2;

#define ASSIGN_1(data, code)\
    data [0] = *code++;

#ifdef _WIN32
#  define STORE_ADDRESS(index,label) __asm lea eax, label __asm mov edx,testCodes\
     __asm mov [edx][index * TYPE testCodes],eax
#  define GOTO_OPCODE() { void* addr = gotoAddresses[*code]; __asm jmp addr }

#else
#  define STORE_ADDRESS(index,label) gotoAddresses[index] = &&label
#  define GOTO_OPCODE() goto *gotoAddresses[*code]
#endif

template <typename T>
class vm_store
{
public:
	vm_store()
	{
		variables = new T[256];
	}

	~vm_store()
	{
		if (variables != nullptr)
			delete[] variables;
	}

	size_t startAddress;
	T* variables{ nullptr };
};

//template <int N>
//struct StaticAssignment {
//	inline static void assign(vm_char_t * data, char*& code)
//	{
//		data[N - 1] = *++code;
//		StaticAssignment<N - 1>::assign(data, code);
//	}
//};
//template <>
//struct StaticAssignment<1> {
//	inline static void assign(vm_char_t * data, char* & code)
//	{
//		data[0] = *++code;
//	}
//};

typedef void (two_arg_func)(vm_object*, vm_object*);
typedef void (one_arg_func)(vm_object*);

struct str_functions {
	two_arg_func add_str;
};

class vm_system_impl
{
public:
	const int STORE_SIZE = 1024;
	vm_system_impl(vm_system* pSystem)
	{
		currentStack = new vm_object[1024 * 512];
		currentStore = new vm_store<vm_object>;
		stores = new vm_store<vm_object>*[STORE_SIZE];

		for (size_t i = 0; i < STORE_SIZE; ++i)
			stores[i] = new vm_store<vm_object>;

		stores[0] = currentStore;
		storesCount = 0;
		stackIndex = 0;
		system = pSystem;
		nativeMethodsEnd = nativeMethods.end();
	}

	~vm_system_impl()
	{
		for (int i = 0; i < STORE_SIZE; ++i)
			delete stores[i];

		delete[] currentStack;
		delete[] stores;
	}

	size_t storesCount;
	vm_store<vm_object>** stores{ nullptr };
	vm_store<vm_object>* currentStore{ nullptr };
	vm_store<vm_object> globalStore;
	std::unordered_map<string_type, VmMethod> nativeMethods;
	std::unordered_map<string_type, VmMethod>::iterator nativeMethodsEnd;
	std::unordered_map<string_type, size_t> methods;
	std::unordered_map<string_type, size_t>::iterator methodsEnd;

	vm_object* currentStack{ nullptr };
	size_t stackIndex;
	vm_system* system{ nullptr };

	void* gotoAddresses[128];

	inline bool is_number(vm_object* obj)
	{
		return obj != nullptr && (obj->Type == vm_object::vm_object_type::INT || obj->Type == vm_object::vm_object_type::DOUBLE);
	}

	inline bool is_number(char_type* chr)
	{
		char* end = 0;
		double val = strtod(chr, &end);
		return end != chr && val != HUGE_VAL;
	}

	inline void add_str(vm_object* left, vm_object* right)
	{
		vm_object* newLeft = get_as_string(left);
		vm_object* newRight = get_as_string(right);

		size_t leftLen = strlen(static_cast<char_type*>(newLeft->Pointer));
		size_t rightLen = strlen(static_cast<char_type*>(newRight->Pointer));

		char_type* newStr = new char_type[leftLen + rightLen + 1];
		std::memcpy(newStr, newLeft->Pointer, leftLen);
		std::memcpy(newStr + leftLen, newRight->Pointer, rightLen);
		newStr[leftLen + rightLen] = '\0';

		GET_ITEM(2, Pointer) = newStr;
		GET_ITEM(2, Type) = vm_object::vm_object_type::STR;

		delete newLeft;
		delete newRight;
	}

	inline void add_double(vm_object* left, vm_object* right)
	{
		vm_object* newLeft = get_as_double(left);
		vm_object* newRight = get_as_double(right);

		GET_ITEM(2, Double) = newLeft->Double + newRight->Double;
		GET_ITEM(2, Type) = vm_object::vm_object_type::DOUBLE;

		delete newLeft;
		delete newRight;
	}

	inline void add_int(vm_object* left, vm_object* right)
	{
		vm_object* newLeft = get_as_int(left);
		vm_object* newRight = get_as_int(right);

		GET_ITEM(2, Int) = newLeft->Int + newRight->Int;
		GET_ITEM(2, Type) = vm_object::vm_object_type::INT;

		delete newLeft;
		delete newRight;
	}

	inline void add_bool(vm_object* left, vm_object* right)
	{
		vm_object* newLeft = get_as_bool(left);
		vm_object* newRight = get_as_bool(right);

		GET_ITEM(2, Bool) = newLeft->Bool && newRight->Bool;
		GET_ITEM(2, Type) = vm_object::vm_object_type::BOOL;

		delete newLeft;
		delete newRight;
	}

	inline void sub_double(vm_object* left, vm_object* right)
	{
		vm_object* newLeft = get_as_double(left);
		vm_object* newRight = get_as_double(right);

		GET_ITEM(2, Double) = newLeft->Double - newRight->Double;
		GET_ITEM(2, Type) = vm_object::vm_object_type::DOUBLE;

		delete newLeft;
		delete newRight;
	}

	inline void sub_int(vm_object* left, vm_object* right)
	{
		vm_object* newLeft = get_as_int(left);
		vm_object* newRight = get_as_int(right);

		GET_ITEM(2, Int) = newLeft->Int - newRight->Int;
		GET_ITEM(2, Type) = vm_object::vm_object_type::INT;

		delete newLeft;
		delete newRight;
	}

	inline void sub_bool(vm_object* left, vm_object* right)
	{
		vm_object* newLeft = get_as_bool(left);
		vm_object* newRight = get_as_bool(right);

		GET_ITEM(2, Bool) = newLeft->Bool && newRight->Bool;
		GET_ITEM(2, Type) = vm_object::vm_object_type::BOOL;

		delete newLeft;
		delete newRight;
	}

	inline void seq_str(vm_object* left, vm_object* right)
	{
		if (left->Type == right->Type) // string cannot multiply with another string
		{
			GET_ITEM(2, Type) = vm_object::vm_object_type::EMPTY;
			return;
		}

		if ((left->Type == vm_object::vm_object_type::STR && !is_number(right)) ||
			(right->Type == vm_object::vm_object_type::STR && !is_number(left)))
		{
			GET_ITEM(2, Type) = vm_object::vm_object_type::EMPTY;
			return;
		}

		vm_object* strObj = nullptr;
		vm_object* numberObj = nullptr;

		if (left->Type == vm_object::vm_object_type::STR && is_number(right))
		{
			strObj = left;
			numberObj = get_as_int(right);
		}
		else {
			strObj = right;
			numberObj = get_as_int(left);
		}

		size_t strLen = strlen(static_cast<char_type*>(strObj->Pointer));
		size_t totalLen = strLen * numberObj->Int;

		char_type* newStr = new char_type[totalLen + 1];
		for(size_t i = 0; i < totalLen; ++i)
			std::memcpy(newStr + (i * strLen), strObj->Pointer, strLen);

		newStr[totalLen] = '\0';

		GET_ITEM(2, Pointer) = newStr;
		GET_ITEM(2, Type) = vm_object::vm_object_type::STR;
	}

	inline void mul_double(vm_object* left, vm_object* right)
	{
		vm_object* newLeft = get_as_double(left);
		vm_object* newRight = get_as_double(right);

		GET_ITEM(2, Double) = newLeft->Double * newRight->Double;
		GET_ITEM(2, Type) = vm_object::vm_object_type::DOUBLE;

		delete newLeft;
		delete newRight;
	}

	inline void mul_int(vm_object* left, vm_object* right)
	{
		vm_object* newLeft = get_as_int(left);
		vm_object* newRight = get_as_int(right);

		GET_ITEM(2, Int) = newLeft->Int * newRight->Int;
		GET_ITEM(2, Type) = vm_object::vm_object_type::INT;

		delete newLeft;
		delete newRight;
	}

	inline void mul_bool(vm_object* left, vm_object* right)
	{
		vm_object* newLeft = get_as_bool(left);
		vm_object* newRight = get_as_bool(right);

		GET_ITEM(2, Bool) = newLeft->Bool || newRight->Bool;
		GET_ITEM(2, Type) = vm_object::vm_object_type::BOOL;

		delete newLeft;
		delete newRight;
	}

	inline void div_double(vm_object* left, vm_object* right)
	{
		vm_object* newLeft = get_as_double(left);
		vm_object* newRight = get_as_double(right);

		GET_ITEM(2, Double) = newLeft->Double / newRight->Double;
		GET_ITEM(2, Type) = vm_object::vm_object_type::DOUBLE;

		delete newLeft;
		delete newRight;
	}

	inline void div_int(vm_object* left, vm_object* right)
	{
		vm_object* newLeft = get_as_double(left);
		vm_object* newRight = get_as_double(right);

		GET_ITEM(2, Double) = newLeft->Double / newRight->Double;
		GET_ITEM(2, Type) = vm_object::vm_object_type::DOUBLE;

		delete newLeft;
		delete newRight;
	}

	inline void div_bool(vm_object* left, vm_object* right)
	{
		vm_object* newLeft = get_as_bool(left);
		vm_object* newRight = get_as_bool(right);

		GET_ITEM(2, Bool) = newLeft->Bool && newRight->Bool;
		GET_ITEM(2, Type) = vm_object::vm_object_type::BOOL;

		delete newLeft;
		delete newRight;
	}

	inline void eq_str(vm_object* left, vm_object* right)
	{
		vm_object* newLeft = get_as_string(left);
		vm_object* newRight = get_as_string(right);

		GET_ITEM(2, Bool) = strcmp((char_type*)newLeft->Pointer, (char_type*)newRight->Pointer) == 0;
		GET_ITEM(2, Type) = vm_object::vm_object_type::BOOL;

		delete newLeft;
		delete newRight;
	}

	inline void eq_double(vm_object* left, vm_object* right)
	{
		vm_object* newLeft = get_as_double(left);
		vm_object* newRight = get_as_double(right);

		GET_ITEM(2, Bool) = newLeft->Double == newRight->Double;
		GET_ITEM(2, Type) = vm_object::vm_object_type::BOOL;

		delete newLeft;
		delete newRight;
	}

	inline void eq_int(vm_object* left, vm_object* right)
	{
		vm_object* newLeft = get_as_int(left);
		vm_object* newRight = get_as_int(right);

		GET_ITEM(2, Bool) = newLeft->Int == newRight->Int;
		GET_ITEM(2, Type) = vm_object::vm_object_type::BOOL;

		delete newLeft;
		delete newRight;
	}

	inline void eq_bool(vm_object* left, vm_object* right)
	{
		vm_object* newLeft = get_as_bool(left);
		vm_object* newRight = get_as_bool(right);

		GET_ITEM(2, Bool) = newLeft->Bool == newRight->Bool;
		GET_ITEM(2, Type) = vm_object::vm_object_type::BOOL;

		delete newLeft;
		delete newRight;
	}

	inline vm_object* get_as_string(vm_object* obj)
	{
		switch (obj->Type)
		{
			case vm_object::vm_object_type::DOUBLE:
				return new vm_object(AS_STRING(obj->Double));

			case vm_object::vm_object_type::INT:
				return new vm_object(AS_STRING(obj->Int));

			case vm_object::vm_object_type::BOOL:
				return new vm_object(AS_STRING(obj->Bool));

			case vm_object::vm_object_type::STR:
				return new vm_object((char_type*)obj->Pointer);

			default:
				return new vm_object(_T(""));
		}
	}

	inline vm_object* get_as_int(vm_object* obj)
	{
		switch (obj->Type)
		{
			case vm_object::vm_object_type::DOUBLE:
				return new vm_object((int)obj->Double);

			case vm_object::vm_object_type::INT:
				return new vm_object(obj->Int);

			case vm_object::vm_object_type::BOOL:
				return new vm_object(obj->Bool ? 1 : 0);

			case vm_object::vm_object_type::STR:
			{
				char_type* chr = (char_type*)obj->Pointer;
				if (is_number(chr))
					return new vm_object();

				new vm_object(std::stoi(chr));
			}

			default:
				return new vm_object(0);
		}
	}

	inline vm_object* get_as_bool(vm_object* obj)
	{
		switch (obj->Type)
		{
			case vm_object::vm_object_type::DOUBLE:
				return new vm_object(obj->Double > 0.0);

			case vm_object::vm_object_type::INT:
				return new vm_object(obj->Int > 0);

			case vm_object::vm_object_type::BOOL:
				return new vm_object(obj->Bool);

			case vm_object::vm_object_type::STR:
				return new vm_object(strlen((char_type*)obj->Pointer) > 0);

			default:
				return new vm_object(false);
		}
	}

	inline vm_object* get_as_double(vm_object* obj)
	{
		switch (obj->Type)
		{
			case vm_object::vm_object_type::DOUBLE:
				return new vm_object(obj->Double);

			case vm_object::vm_object_type::INT:
				return new vm_object((double)obj->Int);

			case vm_object::vm_object_type::BOOL:
				return new vm_object(obj->Bool ? 1.0 : 0.0);

			case vm_object::vm_object_type::STR:
			{
				char_type* chr = (char_type*)obj->Pointer;
				if (is_number(chr))
					return new vm_object();

				new vm_object(std::stod(chr));
			}

			default:
				return new vm_object(0.0);
		}
	}

	void execute(char_type* code, size_t len, size_t startIndex, bool firstInit)
	{
		char_type* startPoint = code;
		code += startIndex;

		if (firstInit)
			goto initSystem;

		PRINT_OPCODE();
		GOTO_OPCODE();

	opt_ADD:
		++code;
		PRINT_OPCODE();
        FUNC_BEGIN()
        {
            auto* left = PEEK_INDEX_PTR(2);
            auto* right = PEEK_INDEX_PTR(1);

			if (left->Type == vm_object::vm_object_type::STR ||
				right->Type == vm_object::vm_object_type::STR)
			{
				add_str(left, right);
			}
			else if (left->Type == vm_object::vm_object_type::DOUBLE ||
					 right->Type == vm_object::vm_object_type::DOUBLE)
			{
				add_double(left, right);
			}
			else if (left->Type == vm_object::vm_object_type::INT ||
					 right->Type == vm_object::vm_object_type::INT)
			{
				add_int(left, right);
			}
			else if (left->Type == vm_object::vm_object_type::BOOL ||
					 right->Type == vm_object::vm_object_type::BOOL)
			{
				add_bool(left, right);
			}
			else if (left->Type == vm_object::vm_object_type::ARRAY ||
					right->Type == vm_object::vm_object_type::ARRAY)
			{
				// todo: implement
				GET_ITEM(2, Type) = vm_object::vm_object_type::EMPTY;
			}
			else if (left->Type == vm_object::vm_object_type::DICT ||
					 right->Type == vm_object::vm_object_type::DICT)
			{
				// todo: implement
				GET_ITEM(2, Type) = vm_object::vm_object_type::EMPTY;
			}
			else
				GET_ITEM(2, Type) = vm_object::vm_object_type::EMPTY;

            STACK_DINC();
        }
        FUNC_END();
		GOTO_OPCODE();

	opt_SUB:
		++code;
		PRINT_OPCODE();
		FUNC_BEGIN()
		{
			auto* left = PEEK_INDEX_PTR(2);
			auto* right = PEEK_INDEX_PTR(1);

			if (left->Type == vm_object::vm_object_type::DOUBLE ||
					 right->Type == vm_object::vm_object_type::DOUBLE)
			{
				sub_double(left, right);
			}
			else if (left->Type == vm_object::vm_object_type::INT ||
					 right->Type == vm_object::vm_object_type::INT)
			{
				sub_int(left, right);
			}
			else if (left->Type == vm_object::vm_object_type::BOOL ||
					 right->Type == vm_object::vm_object_type::BOOL)
			{
				sub_bool(left, right);
			}
			else if (left->Type == vm_object::vm_object_type::ARRAY ||
					 right->Type == vm_object::vm_object_type::ARRAY)
			{
				// todo: implement
				GET_ITEM(2, Type) = vm_object::vm_object_type::EMPTY;
			}
			else if (left->Type == vm_object::vm_object_type::DICT ||
					 right->Type == vm_object::vm_object_type::DICT)
			{
				// todo: implement
				GET_ITEM(2, Type) = vm_object::vm_object_type::EMPTY;
			}
			else
				GET_ITEM(2, Type) = vm_object::vm_object_type::EMPTY;

			STACK_DINC();
		}
		FUNC_END();
		GOTO_OPCODE();

	opt_MUL:
		++code;
		PRINT_OPCODE();
		FUNC_BEGIN()
		{
			auto* left = PEEK_INDEX_PTR(2);
			auto* right = PEEK_INDEX_PTR(1);

			if (left->Type == vm_object::vm_object_type::STR ||
				right->Type == vm_object::vm_object_type::STR)
			{
				seq_str(left, right);
			}
			else if (left->Type == vm_object::vm_object_type::DOUBLE ||
					 right->Type == vm_object::vm_object_type::DOUBLE)
			{
				mul_double(left, right);
			}
			else if (left->Type == vm_object::vm_object_type::INT ||
					 right->Type == vm_object::vm_object_type::INT)
			{
				mul_int(left, right);
			}
			else if (left->Type == vm_object::vm_object_type::BOOL ||
					 right->Type == vm_object::vm_object_type::BOOL)
			{
				mul_bool(left, right);
			}
			else
				GET_ITEM(2, Type) = vm_object::vm_object_type::EMPTY;

			STACK_DINC();
		}
		FUNC_END();
		GOTO_OPCODE();

	opt_DIV:
		++code;
		PRINT_OPCODE();
		FUNC_BEGIN()
		{
			auto* left = PEEK_INDEX_PTR(2);
			auto* right = PEEK_INDEX_PTR(1);

			if (left->Type == vm_object::vm_object_type::DOUBLE ||
					 right->Type == vm_object::vm_object_type::DOUBLE)
			{
				div_double(left, right);
			}
			else if (left->Type == vm_object::vm_object_type::INT ||
					 right->Type == vm_object::vm_object_type::INT)
			{
				div_int(left, right);
			}
			else if (left->Type == vm_object::vm_object_type::BOOL ||
					 right->Type == vm_object::vm_object_type::BOOL)
			{
				div_bool(left, right);
			}
			else
				GET_ITEM(2, Type) = vm_object::vm_object_type::EMPTY;

			STACK_DINC();
		}
		FUNC_END();
		GOTO_OPCODE();

	opt_EQ:
		++code;
		PRINT_OPCODE();
		FUNC_BEGIN()
		{
			auto* left = PEEK_INDEX_PTR(2);
			auto* right = PEEK_INDEX_PTR(1);

			if (left->Type == vm_object::vm_object_type::STR ||
				right->Type == vm_object::vm_object_type::STR)
			{
				eq_str(left, right);
			}
			else if (left->Type == vm_object::vm_object_type::DOUBLE ||
					 right->Type == vm_object::vm_object_type::DOUBLE)
			{
				eq_double(left, right);
			}
			else if (left->Type == vm_object::vm_object_type::INT ||
					 right->Type == vm_object::vm_object_type::INT)
			{
				eq_int(left, right);
			}
			else if (left->Type == vm_object::vm_object_type::BOOL ||
					 right->Type == vm_object::vm_object_type::BOOL)
			{
				eq_bool(left, right);
			}
			else
				GET_ITEM(2, Type) = vm_object::vm_object_type::EMPTY;

			STACK_DINC();
		}
		FUNC_END();
		GOTO_OPCODE();

	opt_LT:
		++code;
		PRINT_OPCODE();
		OPERATION_LT(Bool, vm_object::vm_object_type::BOOL, Int);
		GOTO_OPCODE();

	opt_LTE:
		++code;
		PRINT_OPCODE();
		OPERATION_LTE(Bool, vm_object::vm_object_type::BOOL, Int);
		GOTO_OPCODE();

	opt_GT:
		++code;
		PRINT_OPCODE();
		OPERATION_GT(Bool, vm_object::vm_object_type::BOOL, Int);
		GOTO_OPCODE();

	opt_GTE:
		++code;
		PRINT_OPCODE();
		OPERATION_GTE(Bool, vm_object::vm_object_type::BOOL, Int);
		GOTO_OPCODE();

	opt_AND:
		++code;
		PRINT_OPCODE();
		OPERATION_AND(Bool, vm_object::vm_object_type::BOOL);
		GOTO_OPCODE();

	opt_OR:
		++code;
		PRINT_OPCODE();
		OPERATION_OR(Bool, vm_object::vm_object_type::BOOL);
		GOTO_OPCODE();

	opt_DUP:
		++code;
		PRINT_OPCODE();
		switch (currentStack[stackIndex - 1].Type)
		{
		case vm_object::vm_object_type::ARRAY:
			// todo : implement
			break;

		case vm_object::vm_object_type::STR:
		{
			string_type* newStr = new string_type(*static_cast<string_type*>(currentStack[stackIndex - 1].Pointer));
			PUSH_WITH_INIT(newStr);
		}
		break;

		case vm_object::vm_object_type::DICT:
			break;

		case vm_object::vm_object_type::INT:
			PUSH_WITH_STACK(currentStack, stackIndex, Int, currentStack[stackIndex - 1]);
			break;

		case vm_object::vm_object_type::DOUBLE:
			PUSH_WITH_STACK(currentStack, stackIndex, Double, currentStack[stackIndex - 1]);
			break;

		case vm_object::vm_object_type::BOOL:
			PUSH_WITH_STACK(currentStack, stackIndex, Bool, currentStack[stackIndex - 1]);
			break;
		}

		GOTO_OPCODE();

	opt_JMP:
		++code;
		{
			PRINT_OPCODE();
			FUNC_BEGIN()
				vm_int_t integer;
			integer.Int = 0;

			ASSIGN_4(integer.Chars, code);

			code += integer.Int;
			FUNC_END()
		}
		GOTO_OPCODE();

	OPT_METHOD_DEF:
		++code;
		{
			PRINT_OPCODE();
			FUNC_BEGIN()
				vm_int_t integer;
			integer.Int = 0;
			ASSIGN_4(integer.Chars, code);
			methods[integer.Chars] = startPoint - code;
			FUNC_END()
		}
		GOTO_OPCODE();

	opt_JIF:
		++code;
		{
			PRINT_OPCODE();
			FUNC_BEGIN()
				if (POP_AS(Bool))
					code += 4;
				else
				{
					vm_int_t integer;
					ASSIGN_4(integer.Chars, code);
					code += integer.Int;
				}
			FUNC_END()
		}
		GOTO_OPCODE();

	opt_IF_EQ:
		++code;
		{
			PRINT_OPCODE();
			FUNC_BEGIN()
				if (IS_EQUAL(Int))
					code += 4;
				else
				{
					vm_int_t integer;
					ASSIGN_4(integer.Chars, code);
					code += integer.Int;
				}

			stackIndex -= 2;
			FUNC_END()
		}
		GOTO_OPCODE();

	opt_JNIF:
		++code;
		{
			PRINT_OPCODE();
			FUNC_BEGIN()
				if (!IS_EQUAL(Int))
				{
					vm_int_t integer;
					integer.Int = 0;
					ASSIGN_4(integer.Chars, code);
					code += integer.Int;
				}
				else
					code += 4;

			stackIndex -= 2;
			FUNC_END()
		}
		GOTO_OPCODE();

	opt_INC:
		++code;
		PRINT_OPCODE();
		INC(currentStack, stackIndex, Int);
		GOTO_OPCODE();

	opt_NEG:
		++code;
		PRINT_OPCODE();
		NEG(currentStack, stackIndex, Int);
		GOTO_OPCODE();

	opt_DINC:
		++code;
		PRINT_OPCODE();
		DINC(currentStack, stackIndex, Int);
		GOTO_OPCODE();

	opt_LOAD:
		++code;
		{
			PRINT_OPCODE();
			int data = *++code;
			LOAD_AND_PUSH(data);
		}
		GOTO_OPCODE();

	LOAD_PRE(0);
    LOAD_PRE(1);
    LOAD_PRE(2);
    LOAD_PRE(3);
    LOAD_PRE(4);
    LOAD_PRE(5);
    LOAD_PRE(6);
    LOAD_PRE(7);
    LOAD_PRE(8);
    LOAD_PRE(9);
    LOAD_PRE(10);
    LOAD_PRE(11);
    LOAD_PRE(12);
    LOAD_PRE(13);
    LOAD_PRE(14);
    LOAD_PRE(15);
    LOAD_PRE(16);

	opt_STORE:
		++code;
		PRINT_OPCODE();
		STORE(*++code, POP());
		GOTO_OPCODE();

    STORE_PRE(0);
    STORE_PRE(1);
    STORE_PRE(2);
    STORE_PRE(3);
    STORE_PRE(4);
    STORE_PRE(5);
    STORE_PRE(6);
    STORE_PRE(7);
    STORE_PRE(8);
    STORE_PRE(9);
    STORE_PRE(10);
    STORE_PRE(11);
    STORE_PRE(12);
    STORE_PRE(13);
    STORE_PRE(14);
    STORE_PRE(15);
    STORE_PRE(16);

	opt_GLOAD:
		++code;
		PRINT_OPCODE();
		PUSH_WITH_ASSIGN(GLOAD(*++code));
		GOTO_OPCODE();

    GLOAD_PRE(0);
    GLOAD_PRE(1);
    GLOAD_PRE(2);
    GLOAD_PRE(3);
    GLOAD_PRE(4);
    GLOAD_PRE(5);
    GLOAD_PRE(6);
    GLOAD_PRE(7);
    GLOAD_PRE(8);
    GLOAD_PRE(9);
    GLOAD_PRE(10);
    GLOAD_PRE(11);
    GLOAD_PRE(12);
    GLOAD_PRE(13);
    GLOAD_PRE(14);
    GLOAD_PRE(15);
    GLOAD_PRE(16);

	opt_GSTORE:
		++code;
		PRINT_OPCODE();
		GSTORE(*++code, POP());
		GOTO_OPCODE();

    GSTORE_PRE(0);
    GSTORE_PRE(1);
    GSTORE_PRE(2);
    GSTORE_PRE(3);
    GSTORE_PRE(4);
    GSTORE_PRE(5);
    GSTORE_PRE(6);
    GSTORE_PRE(7);
    GSTORE_PRE(8);
    GSTORE_PRE(9);
    GSTORE_PRE(10);
    GSTORE_PRE(11);
    GSTORE_PRE(12);
    GSTORE_PRE(13);
    GSTORE_PRE(14);
    GSTORE_PRE(15);
    GSTORE_PRE(16);

	opt_CALL:
		++code;
		{
			PRINT_OPCODE();
			FUNC_BEGIN()
				currentStore = stores[++storesCount];
			vm_int_t integer;
			integer.Int = 0;
			ASSIGN_4(integer.Chars, code);
			currentStore->startAddress = (code - startPoint);
			code += integer.Int;
			FUNC_END()
		}
		GOTO_OPCODE();

	opt_RETURN:
		++code;
		{
			PRINT_OPCODE();
			FUNC_BEGIN()
				code = startPoint + currentStore->startAddress;
			currentStore = stores[--storesCount];
			FUNC_END()
		}
		GOTO_OPCODE();

	opt_POP:
		++code;
		PRINT_OPCODE();
		POP();
		GOTO_OPCODE();

    opt_CONST_INT:
		++code; {
			PRINT_OPCODE();
			FUNC_BEGIN()
				vm_int_t integer;
			integer.Int = 0;
			ASSIGN_4(integer.Chars, code);
			PUSH_WITH_ASSIGN(integer.Int);
			FUNC_END()
				// console_out << _T("PUSH : ") << Operations::Peek<int>(currentStack, stackIndex) << '\n';
		}
		GOTO_OPCODE();

    opt_CONST_INT_0:
		++code;
		PRINT_OPCODE();
		PUSH_WITH_ASSIGN(0);
		GOTO_OPCODE();

    opt_CONST_INT_1:
		++code;
		PRINT_OPCODE();
		PUSH_WITH_ASSIGN(1);
		GOTO_OPCODE();

    opt_CONST_DOUBLE:
        ++code; {
            PRINT_OPCODE();
            vm_double_t d;
            d.Double = 0.0;
            ASSIGN_8(d.Chars, code);
            PUSH_WITH_ASSIGN(d.Double);
        }
        GOTO_OPCODE();

    opt_CONST_DOUBLE_0:
        ++code; {
            PRINT_OPCODE();
            vm_double_t d;
            d.Double = 0.0;
            ASSIGN_8(d.Chars, code);
            PUSH_WITH_ASSIGN(d.Double);
        }
        GOTO_OPCODE();

    opt_CONST_DOUBLE_1:
        ++code; {
            PRINT_OPCODE();
            vm_double_t d;
            d.Double = 1.0;
            PUSH_WITH_ASSIGN(d.Double);
        }
        GOTO_OPCODE();


    opt_CONST_BOOL_TRUE:
        ++code;
        PRINT_OPCODE();
        PUSH_WITH_ASSIGN(true);
        GOTO_OPCODE();

    opt_CONST_BOOL_FALSE:
        ++code;
        PRINT_OPCODE();
        PUSH_WITH_ASSIGN(false);
        GOTO_OPCODE();

	opt_CALL_NATIVE:
		++code; {
			PRINT_OPCODE();
			vm_int_t integer;
			integer.Int = 0;
			ASSIGN_4(integer.Chars, code);

			char_type * chars = new char_type[integer.Int + 1];
			for (int i = 0; i < integer.Int; ++i)
			{
				chars[i] = *code;
				++code;
			}

			chars[integer.Int] = '\0';
			if (nativeMethods.find(chars) != nativeMethodsEnd)
			{
				vm_object* result = nativeMethods[chars](system);
				if (result != nullptr)
				{
					switch (result->Type)
					{
					case vm_object::vm_object_type::BOOL:
						PUSH_WITH_INIT(result->Bool);
						break;

					case vm_object::vm_object_type::DOUBLE:
						PUSH_WITH_INIT(result->Double);
						GOTO_OPCODE();

					case vm_object::vm_object_type::INT:
						PUSH_WITH_INIT(result->Int);
						break;

					case vm_object::vm_object_type::STR:
						PUSH_WITH_INIT(static_cast<char_type*>(result->Pointer));
						break;
					}
				}
			}
			else
			{
				console_out << _T("ERROR : Method '") << chars << _T("' Not Found\n");
				PUSH_WITH_EMPTY();
				goto opt_HALT;
			}
		}
		GOTO_OPCODE();

    opt_CONST_STR:
		++code; {
			PRINT_OPCODE();
			vm_int_t integer;
			integer.Int = 0;
			ASSIGN_4(integer.Chars, code);

			char_type * chars = new char_type[integer.Int + 1];
			for (int i = 0; i < integer.Int; ++i)
			{
				chars[i] = *code;
				++code;
			}

			chars[integer.Int] = '\0';
			PUSH_WITH_INIT(chars);
		}
		GOTO_OPCODE();

	opt_PRINT:
		++code;
		PRINT_OPCODE();
		// console_out << Operations::PopAs<int>(currentStack, stackIndex);
		GOTO_OPCODE();

	opt_HALT:
		++code;
		PRINT_OPCODE();
		return;

	initSystem:
		void** testCodes = gotoAddresses; // this dummy codes used by vc++ 
        STORE_ADDRESS(vm_inst::OPT_HALT, opt_HALT);
        STORE_ADDRESS(vm_inst::OPT_ADD, opt_ADD);
        STORE_ADDRESS(vm_inst::OPT_SUB, opt_SUB);
        STORE_ADDRESS(vm_inst::OPT_MUL, opt_MUL);
        STORE_ADDRESS(vm_inst::OPT_DIV, opt_DIV);

        STORE_ADDRESS(vm_inst::OPT_EQ, opt_EQ);
        STORE_ADDRESS(vm_inst::OPT_LT, opt_LT);
        STORE_ADDRESS(vm_inst::OPT_LTE, opt_LTE);
        STORE_ADDRESS(vm_inst::OPT_GT, opt_GT);
        STORE_ADDRESS(vm_inst::OPT_GTE, opt_GTE);

        STORE_ADDRESS(vm_inst::OPT_AND, opt_AND);
        STORE_ADDRESS(vm_inst::OPT_OR, opt_OR);
        STORE_ADDRESS(vm_inst::OPT_DUP, opt_DUP);
        STORE_ADDRESS(vm_inst::OPT_POP, opt_POP);
        STORE_ADDRESS(vm_inst::OPT_JMP, opt_JMP);

        STORE_ADDRESS(vm_inst::OPT_CONST_STR, opt_CONST_STR);
        STORE_ADDRESS(vm_inst::OPT_CONST_INT, opt_CONST_INT);
        STORE_ADDRESS(vm_inst::OPT_CONST_INT_0, opt_CONST_INT_0);
        STORE_ADDRESS(vm_inst::OPT_CONST_INT_1, opt_CONST_INT_1);
        STORE_ADDRESS(vm_inst::OPT_CONST_BOOL_TRUE, opt_CONST_BOOL_TRUE);
        STORE_ADDRESS(vm_inst::OPT_CONST_BOOL_FALSE, opt_CONST_BOOL_FALSE);
        STORE_ADDRESS(vm_inst::OPT_CONST_DOUBLE, opt_CONST_DOUBLE);
        STORE_ADDRESS(vm_inst::OPT_CONST_DOUBLE_0, opt_CONST_DOUBLE_0);
        STORE_ADDRESS(vm_inst::OPT_CONST_DOUBLE_1, opt_CONST_DOUBLE_1);

//        STORE_ADDRESS(vm_inst::OPT_DELETE, opt_DELETE);

        STORE_ADDRESS(vm_inst::OPT_IF_EQ, opt_IF_EQ);
        STORE_ADDRESS(vm_inst::OPT_JIF, opt_JIF);
        STORE_ADDRESS(vm_inst::OPT_JNIF, opt_JNIF);
        STORE_ADDRESS(vm_inst::OPT_INC, opt_INC);
        STORE_ADDRESS(vm_inst::OPT_DINC, opt_DINC);

        STORE_ADDRESS(vm_inst::OPT_LOAD, opt_LOAD);
        STORE_ADDRESS(vm_inst::OPT_LOAD_0, opt_LOAD_0);
        STORE_ADDRESS(vm_inst::OPT_LOAD_1, opt_LOAD_1);
        STORE_ADDRESS(vm_inst::OPT_LOAD_2, opt_LOAD_2);
        STORE_ADDRESS(vm_inst::OPT_LOAD_3, opt_LOAD_3);
        STORE_ADDRESS(vm_inst::OPT_LOAD_4, opt_LOAD_4);
        STORE_ADDRESS(vm_inst::OPT_LOAD_5, opt_LOAD_5);
        STORE_ADDRESS(vm_inst::OPT_LOAD_6, opt_LOAD_6);
        STORE_ADDRESS(vm_inst::OPT_LOAD_7, opt_LOAD_7);
        STORE_ADDRESS(vm_inst::OPT_LOAD_8, opt_LOAD_8);
        STORE_ADDRESS(vm_inst::OPT_LOAD_9, opt_LOAD_9);
        STORE_ADDRESS(vm_inst::OPT_LOAD_10, opt_LOAD_10);
        STORE_ADDRESS(vm_inst::OPT_LOAD_11, opt_LOAD_11);
        STORE_ADDRESS(vm_inst::OPT_LOAD_12, opt_LOAD_12);
        STORE_ADDRESS(vm_inst::OPT_LOAD_13, opt_LOAD_13);
        STORE_ADDRESS(vm_inst::OPT_LOAD_14, opt_LOAD_14);
        STORE_ADDRESS(vm_inst::OPT_LOAD_15, opt_LOAD_15);
        STORE_ADDRESS(vm_inst::OPT_LOAD_16, opt_LOAD_16);

        STORE_ADDRESS(vm_inst::OPT_STORE, opt_STORE);
        STORE_ADDRESS(vm_inst::OPT_STORE_0, opt_STORE_0);
        STORE_ADDRESS(vm_inst::OPT_STORE_1, opt_STORE_1);
        STORE_ADDRESS(vm_inst::OPT_STORE_2, opt_STORE_2);
        STORE_ADDRESS(vm_inst::OPT_STORE_3, opt_STORE_3);
        STORE_ADDRESS(vm_inst::OPT_STORE_4, opt_STORE_4);
        STORE_ADDRESS(vm_inst::OPT_STORE_5, opt_STORE_5);
        STORE_ADDRESS(vm_inst::OPT_STORE_6, opt_STORE_6);
        STORE_ADDRESS(vm_inst::OPT_STORE_7, opt_STORE_7);
        STORE_ADDRESS(vm_inst::OPT_STORE_8, opt_STORE_8);
        STORE_ADDRESS(vm_inst::OPT_STORE_9, opt_STORE_9);
        STORE_ADDRESS(vm_inst::OPT_STORE_10, opt_STORE_10);
        STORE_ADDRESS(vm_inst::OPT_STORE_11, opt_STORE_11);
        STORE_ADDRESS(vm_inst::OPT_STORE_12, opt_STORE_12);
        STORE_ADDRESS(vm_inst::OPT_STORE_13, opt_STORE_13);
        STORE_ADDRESS(vm_inst::OPT_STORE_14, opt_STORE_14);
        STORE_ADDRESS(vm_inst::OPT_STORE_15, opt_STORE_15);
        STORE_ADDRESS(vm_inst::OPT_STORE_16, opt_STORE_16);

        STORE_ADDRESS(vm_inst::OPT_GLOAD, opt_GLOAD);
        STORE_ADDRESS(vm_inst::OPT_GLOAD_0, opt_GLOAD_0);
        STORE_ADDRESS(vm_inst::OPT_GLOAD_1, opt_GLOAD_1);
        STORE_ADDRESS(vm_inst::OPT_GLOAD_2, opt_GLOAD_2);
        STORE_ADDRESS(vm_inst::OPT_GLOAD_3, opt_GLOAD_3);
        STORE_ADDRESS(vm_inst::OPT_GLOAD_4, opt_GLOAD_4);
        STORE_ADDRESS(vm_inst::OPT_GLOAD_5, opt_GLOAD_5);
        STORE_ADDRESS(vm_inst::OPT_GLOAD_6, opt_GLOAD_6);
        STORE_ADDRESS(vm_inst::OPT_GLOAD_7, opt_GLOAD_7);
        STORE_ADDRESS(vm_inst::OPT_GLOAD_8, opt_GLOAD_8);
        STORE_ADDRESS(vm_inst::OPT_GLOAD_9, opt_GLOAD_9);
        STORE_ADDRESS(vm_inst::OPT_GLOAD_10, opt_GLOAD_10);
        STORE_ADDRESS(vm_inst::OPT_GLOAD_11, opt_GLOAD_11);
        STORE_ADDRESS(vm_inst::OPT_GLOAD_12, opt_GLOAD_12);
        STORE_ADDRESS(vm_inst::OPT_GLOAD_13, opt_GLOAD_13);
        STORE_ADDRESS(vm_inst::OPT_GLOAD_14, opt_GLOAD_14);
        STORE_ADDRESS(vm_inst::OPT_GLOAD_15, opt_GLOAD_15);
        STORE_ADDRESS(vm_inst::OPT_GLOAD_16, opt_GLOAD_16);

        STORE_ADDRESS(vm_inst::OPT_GSTORE, opt_GSTORE);
        STORE_ADDRESS(vm_inst::OPT_GSTORE_0, opt_GSTORE_0);
        STORE_ADDRESS(vm_inst::OPT_GSTORE_1, opt_GSTORE_1);
        STORE_ADDRESS(vm_inst::OPT_GSTORE_2, opt_GSTORE_2);
        STORE_ADDRESS(vm_inst::OPT_GSTORE_3, opt_GSTORE_3);
        STORE_ADDRESS(vm_inst::OPT_GSTORE_4, opt_GSTORE_4);
        STORE_ADDRESS(vm_inst::OPT_GSTORE_5, opt_GSTORE_5);
        STORE_ADDRESS(vm_inst::OPT_GSTORE_6, opt_GSTORE_6);
        STORE_ADDRESS(vm_inst::OPT_GSTORE_7, opt_GSTORE_7);
        STORE_ADDRESS(vm_inst::OPT_GSTORE_8, opt_GSTORE_8);
        STORE_ADDRESS(vm_inst::OPT_GSTORE_9, opt_GSTORE_9);
        STORE_ADDRESS(vm_inst::OPT_GSTORE_10, opt_GSTORE_10);
        STORE_ADDRESS(vm_inst::OPT_GSTORE_11, opt_GSTORE_11);
        STORE_ADDRESS(vm_inst::OPT_GSTORE_12, opt_GSTORE_12);
        STORE_ADDRESS(vm_inst::OPT_GSTORE_13, opt_GSTORE_13);
        STORE_ADDRESS(vm_inst::OPT_GSTORE_14, opt_GSTORE_14);
        STORE_ADDRESS(vm_inst::OPT_GSTORE_15, opt_GSTORE_15);
        STORE_ADDRESS(vm_inst::OPT_GSTORE_16, opt_GSTORE_16);

        STORE_ADDRESS(vm_inst::OPT_CALL, opt_CALL);
        STORE_ADDRESS(vm_inst::OPT_RETURN, opt_RETURN);

//        STORE_ADDRESS(vm_inst::OPT_PUSH, opt_PUSH);
        STORE_ADDRESS(vm_inst::OPT_PRINT, opt_PRINT);
        STORE_ADDRESS(vm_inst::OPT_NEG, opt_NEG);

        STORE_ADDRESS(vm_inst::OPT_CALL_NATIVE, opt_CALL_NATIVE);
//        STORE_ADDRESS(vm_inst::OPT_METHOD_DEF, opt_METHOD_DEF);
//        STORE_ADDRESS(vm_inst::OPT_INITARRAY, opt_INITARRAY);
//        STORE_ADDRESS(vm_inst::OPT_INITDICT, opt_INITDICT);
	}

	void dumpOpcode(char_type* code, size_t len)
	{
        size_t index = 0;
		while (index < len) {
			console_out << _T(">>> ") << index++ << _T(". ");
			console_out << vm_instToString((vm_inst)*code);

			switch ((vm_inst)*code)
			{
			case vm_inst::OPT_LOAD:
			case vm_inst::OPT_STORE:
			case vm_inst::OPT_GLOAD:
			case vm_inst::OPT_GSTORE:
			case vm_inst::OPT_CALL:
			{
				++code;
				vm_int_t integer;
				integer.Int = 0;
				ASSIGN_4(integer.Chars, code);
				console_out << _T(" ") << integer.Int;
				index += 4;
				--code;
			}
			break;

			case vm_inst::OPT_JMP:
			case vm_inst::OPT_JIF:
			case vm_inst::OPT_IF_EQ:
			case vm_inst::OPT_JNIF:
            case vm_inst::OPT_CONST_INT:
			{
			    ++code;
				vm_int_t integer;
				integer.Int = 0;
				ASSIGN_4(integer.Chars, code);
				console_out << _T(" ") << integer.Int;
				index += 4;
				--code;
			}
			break;

            case vm_inst::OPT_CONST_DOUBLE:
			{
				++code;
				vm_double_t d;
				d.Double = 0;
				ASSIGN_8(d.Chars, code);

				console_out << _T(" ") << d.Double;
				index += 8;
				--code;
			}
			break;

                case vm_inst::OPT_CONST_BOOL_TRUE:
                case vm_inst::OPT_CONST_BOOL_FALSE:
				// console_out << _T(" ") << (bool)*++code;
				++index;
				break;

            case vm_inst::OPT_CALL_NATIVE:
			{
				vm_int_t integer;
				integer.Int = 0;
				ASSIGN_1(integer.Chars, code);
				index += 1;
				index += integer.Int;

				char_type * chars = new char_type[integer.Int + 1];
				for (int i = integer.Int - 1; i >= 0; --i)
					chars[i] = *++code;

				chars[integer.Int] = '\0';

				console_out << _T(" \"") << chars << _T("\"");
			}
			break;

			case vm_inst::OPT_METHOD_DEF:
			{
				++code;
				vm_int_t integer;
				integer.Int = 0;
				ASSIGN_4(integer.Chars, code);
				index += 4;
				index += integer.Int;

				char_type * chars = new char_type[integer.Int + 1];
				for (int i = 0; i < integer.Int; ++i)
				{
					chars[i] = *code;
					++code;
				}

				chars[integer.Int] = '\0';

				console_out << _T(" \"") << chars << _T("\"");
				--code;
			}
			break;

            case vm_inst::OPT_CONST_STR:
			{
				vm_int_t integer;
				integer.Int = 0;
				ASSIGN_4(integer.Chars, code);
				index += 4;
				index += integer.Int;

				char_type * chars = new char_type[integer.Int + 1];
				for (int i = integer.Int - 1; i >= 0; --i)
					chars[i] = *++code;

				chars[integer.Int] = '\0';

				console_out << _T(" \"") << chars << _T("\"");
			}
			break;
			}

            ++code;
			console_out << '\n';
		}

		console_out << '\n';
	}

	void dumpStack()
	{
		int index = stackIndex;
		while (index > 0) {
			console_out << _T(">>> ") << index << _T(". ");
			vm_object item = currentStack[index];
			switch (item.Type)
			{
			case vm_object::vm_object_type::INT:
				console_out << _T(" INT: ");
				console_out << item.Int;
				break;

			case vm_object::vm_object_type::DOUBLE:
				console_out << _T(" DOUBLE: ");
				console_out << item.Double;
				break;

			case vm_object::vm_object_type::BOOL:
				console_out << _T(" BOOL: ");
				console_out << item.Bool;
				break;

			case vm_object::vm_object_type::STR:
				console_out << _T(" STR: ");
				console_out << string_type(*static_cast<string_type*>(item.Pointer));
				break;

			default:
				console_out << _T(" EMPTY: ");
				break;
			}

			console_out << '\n';

			--index;
		}

		console_out << '\n';

	}
	void dump(char_type* code, size_t len)
	{
		size_t index = 0;
		while (index < len) {
			console_out << _T(">>> ") << index++ << _T(". ");
			console_out << int(*code);
			console_out << '\n';
			++code;
		}

		console_out << '\n';
	}
};

vm_system::vm_system()
{
	this->impl = new vm_system_impl(this);
	this->impl->execute(nullptr, 0, 0, true);
}

vm_system::~vm_system()
{
	delete impl;
}

void vm_system::execute(char_type* code, size_t len, size_t startIndex)
{
	impl->execute(code, len, startIndex, false);
}

void vm_system::dump(char_type* code, size_t len)
{
	impl->dump(code, len);
}

void vm_system::dumpOpcode(char_type* code, size_t len)
{
	impl->dumpOpcode(code, len);
}

void vm_system::dumpStack()
{
	impl->dumpStack();
}

size_t vm_system::getUInt()
{
	return impl->currentStack[impl->stackIndex - 1].Int;
}

vm_object const * vm_system::getObject()
{
	if (impl->stackIndex > 0)
	{
		//console_out << impl->stackIndex << '\n';
		return &impl->currentStack[--impl->stackIndex];
	}

	return nullptr;
}

void vm_system::addMethod(string_type const & name, VmMethod method)
{
	this->impl->nativeMethods[name] = method;
	this->impl->nativeMethodsEnd = this->impl->nativeMethods.end();
}
