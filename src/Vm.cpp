#include "Vm.h"
#include "Console.h"
#include <unordered_map>
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

#define PEEK() (currentStack[stackIndex - 1])
#define POP() (currentStack[--stackIndex])
#define POP_AS(type) (currentStack[--stackIndex]. type )
#define SET(obj) currentStack[stackIndex - 1] = obj

#define OPERATION(currentStack, stackIndex, op, type) currentStack [ stackIndex - 2]. type = currentStack [ stackIndex - 2]. type ##op currentStack [ stackIndex - 1]. type ;\
    PRINT_AND_CHECK_STACK();\
    -- stackIndex ;

#define PUSH_WITH_STACK(currentStack, stackIndex, type, obj) FUNC_BEGIN() currentStack [ stackIndex ]. type = obj . type ; PRINT_STACK(); ++ stackIndex; FUNC_END()
#define PUSH_WITH_INIT(obj) currentStack [ stackIndex ] = vm_object( obj ) ; PRINT_STACK(); ++ stackIndex;
//#define PUSH(type, obj) currentStack [ stackIndex ]. type = obj ; PRINT_STACK(); ++ stackIndex;
#define PUSH_WITH_ASSIGN(obj) currentStack [ stackIndex ] = obj ; PRINT_STACK(); ++ stackIndex;
#define INC(currentStack, stackIndex, type) currentStack [ stackIndex - 1]. type = currentStack [ stackIndex - 1]. type + 1;
#define DINC(currentStack, stackIndex, type) currentStack [ stackIndex - 1]. type = currentStack [ stackIndex - 1]. type - 1;
#define NEG(currentStack, stackIndex, type) currentStack [ stackIndex - 1]. type =  currentStack [ stackIndex - 1]. type * -1;
#define LOAD_AND_PUSH(index) FUNC_BEGIN() { currentStack[stackIndex].Double = (currentStore->variables + index )->Double;\
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

class vm_system_impl
{
public:
	vm_system_impl(vm_system* pSystem)
	{
		const int STORE_SIZE = 1024;
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
		for (int i = 0; i < 1024; ++i)
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
	void execute(char_type* code, size_t len, size_t startIndex, bool firstInit)
	{
		char_type* startPoint = code;
		code += startIndex;

		if (firstInit)
			goto initSystem;

		PRINT_OPCODE();
		GOTO_OPCODE();

	opt_iADD:
		++code;
		PRINT_OPCODE();
		OPERATION_ADD(Int, vm_object::vm_object_type::INT);
		GOTO_OPCODE();

	opt_iSUB:
		++code;
		PRINT_OPCODE();
		OPERATION_SUB(Int, vm_object::vm_object_type::INT);
		GOTO_OPCODE();

	opt_iMUL:
		++code;
		PRINT_OPCODE();
		OPERATION_MUL(Int, vm_object::vm_object_type::INT);
		GOTO_OPCODE();

	opt_iDIV:
		++code;
		PRINT_OPCODE();
		OPERATION_DIV(Int, vm_object::vm_object_type::INT);
		GOTO_OPCODE();

	opt_dADD:
		++code;
		PRINT_OPCODE();
		OPERATION_ADD(Double, vm_object::vm_object_type::DOUBLE);
		GOTO_OPCODE();

	opt_dSUB:
		++code;
		PRINT_OPCODE();
		OPERATION_SUB(Double, vm_object::vm_object_type::DOUBLE);
		GOTO_OPCODE();

	opt_dMUL:
		++code;
		PRINT_OPCODE();
		OPERATION_MUL(Double, vm_object::vm_object_type::DOUBLE);
		GOTO_OPCODE();

	opt_dDIV:
		++code;
		PRINT_OPCODE();
		OPERATION_DIV(Double, vm_object::vm_object_type::DOUBLE);
		GOTO_OPCODE();

	opt_EQ:
		++code;
		PRINT_OPCODE();
		OPERATION_EQ(Bool, vm_object::vm_object_type::BOOL, Int);
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

	opt_METHOD:
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

	opt_INITARRAY:
		++code;
		PRINT_OPCODE();
		FUNC_BEGIN()
			PUSH_WITH_ASSIGN(new vm_array);
		FUNC_END()
			GOTO_OPCODE();

	opt_aPUSH:
		++code;
		{
			PRINT_OPCODE();
			FUNC_BEGIN()
				vm_object& data = POP();
			vm_array* array = static_cast<vm_array*>(PEEK().Pointer);
			array->push(data);
			FUNC_END()
		}
		GOTO_OPCODE();

	opt_aGET:
		++code;
		{
			PRINT_OPCODE();
			FUNC_BEGIN()
				vm_object& data = POP();
			vm_array* array = static_cast<vm_array*>(PEEK().Pointer);
			PUSH_WITH_ASSIGN(&array->Array[data.Int]);
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

	opt_LOAD_0:
		++code;
		PRINT_OPCODE();
		LOAD_AND_PUSH(0);
		GOTO_OPCODE();

	opt_LOAD_1:
		++code;
		PRINT_OPCODE();
		LOAD_AND_PUSH(1);
		GOTO_OPCODE();

	opt_LOAD_2:
		++code;
		PRINT_OPCODE();
		LOAD_AND_PUSH(2);
		GOTO_OPCODE();

	opt_LOAD_3:
		++code;
		PRINT_OPCODE();
		LOAD_AND_PUSH(3);
		GOTO_OPCODE();

	opt_LOAD_4:
		++code;
		PRINT_OPCODE();
		LOAD_AND_PUSH(4);
		GOTO_OPCODE();

	opt_STORE:
		++code;
		PRINT_OPCODE();
		STORE(*++code, POP());
		GOTO_OPCODE();

	opt_STORE_0:
		++code;
		PRINT_OPCODE();
		STORE(0, POP());
		GOTO_OPCODE();

	opt_STORE_1:
		++code;
		PRINT_OPCODE();
		STORE(1, POP());
		GOTO_OPCODE();

	opt_STORE_2:
		++code;
		PRINT_OPCODE();
		STORE(2, POP());
		GOTO_OPCODE();

	opt_STORE_3:
		++code;
		PRINT_OPCODE();
		STORE(3, POP());
		GOTO_OPCODE();

	opt_STORE_4:
		++code;
		PRINT_OPCODE();
		STORE(4, POP());
		GOTO_OPCODE();

	opt_GLOAD:
		++code;
		PRINT_OPCODE();
		PUSH_WITH_ASSIGN(GLOAD(*++code));
		GOTO_OPCODE();

	opt_GLOAD_0:
		++code;
		PRINT_OPCODE();
		PUSH_WITH_ASSIGN(GLOAD(0));
		GOTO_OPCODE();

	opt_GLOAD_1:
		++code;
		PRINT_OPCODE();
		PUSH_WITH_ASSIGN(GLOAD(1));
		GOTO_OPCODE();

	opt_GLOAD_2:
		++code;
		PRINT_OPCODE();
		PUSH_WITH_ASSIGN(GLOAD(2));
		GOTO_OPCODE();

	opt_GLOAD_3:
		++code;
		PRINT_OPCODE();
		PUSH_WITH_ASSIGN(GLOAD(3));
		GOTO_OPCODE();

	opt_GLOAD_4:
		++code;
		PRINT_OPCODE();
		PUSH_WITH_ASSIGN(GLOAD(4));
		GOTO_OPCODE();

	opt_GSTORE:
		++code;
		PRINT_OPCODE();
		GSTORE(*++code, POP());
		GOTO_OPCODE();

	opt_GSTORE_0:
		++code;
		PRINT_OPCODE();
		GSTORE(0, POP());
		GOTO_OPCODE();

	opt_GSTORE_1:
		++code;
		PRINT_OPCODE();
		GSTORE(1, POP());
		GOTO_OPCODE();

	opt_GSTORE_2:
		++code;
		PRINT_OPCODE();
		GSTORE(2, POP());
		GOTO_OPCODE();

	opt_GSTORE_3:
		++code;
		PRINT_OPCODE();
		GSTORE(3, POP());
		GOTO_OPCODE();

	opt_GSTORE_4:
		++code;
		PRINT_OPCODE();
		GSTORE(4, POP());
		GOTO_OPCODE();

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

	opt_iPUSH:
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

	opt_iPUSH_0:
		++code;
		PRINT_OPCODE();
		PUSH_WITH_ASSIGN(0);
		GOTO_OPCODE();

	opt_iPUSH_1:
		++code;
		PRINT_OPCODE();
		PUSH_WITH_ASSIGN(1);
		GOTO_OPCODE();

	opt_iPUSH_2:
		++code;
		PRINT_OPCODE();
		PUSH_WITH_ASSIGN(2);
		GOTO_OPCODE();

	opt_iPUSH_3:
		++code;
		PRINT_OPCODE();
		PUSH_WITH_ASSIGN(3);
		GOTO_OPCODE();

	opt_iPUSH_4:
		++code;
		PRINT_OPCODE();
		PUSH_WITH_ASSIGN(4);
		GOTO_OPCODE();

	opt_dPUSH_0:
		++code;
		PRINT_OPCODE();
		PUSH_WITH_ASSIGN(0.0);
		GOTO_OPCODE();

	opt_dPUSH_1:
		++code;
		PRINT_OPCODE();
		PUSH_WITH_ASSIGN(1.0);
		GOTO_OPCODE();

	opt_dPUSH_2:
		++code;
		PRINT_OPCODE();
		PUSH_WITH_ASSIGN(2.0);
		GOTO_OPCODE();

	opt_dPUSH_3:
		++code;
		PRINT_OPCODE();
		PUSH_WITH_ASSIGN(3.0);
		GOTO_OPCODE();

	opt_dPUSH_4:
		++code;
		PRINT_OPCODE();
		PUSH_WITH_ASSIGN(4.0);
		GOTO_OPCODE();

	opt_bPUSH_0:
		++code;
		PRINT_OPCODE();
		PUSH_WITH_ASSIGN(false);
		GOTO_OPCODE();

	opt_bPUSH_1:
		++code;
		PRINT_OPCODE();
		PUSH_WITH_ASSIGN(true);
		GOTO_OPCODE();

	opt_dPUSH:
		++code; {
			PRINT_OPCODE();
			vm_double_t d;
			d.Double = 0.0;
			ASSIGN_8(d.Chars, code);
			PUSH_WITH_ASSIGN(d.Double);
		}
		GOTO_OPCODE();


	opt_bPUSH:
		++code;
		PRINT_OPCODE();
		PUSH_WITH_ASSIGN((bool)*++code);
		GOTO_OPCODE();

	opt_INVOKE:
		++code; {
			PRINT_OPCODE();
			vm_int_t integer;
			integer.Int = 0;
			ASSIGN_4(integer.Chars, code);

			char_type * chars = new char_type[integer.Int + 1];
			for (int i = integer.Int - 1; i >= 0; --i)
				chars[i] = *++code;

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
						PUSH_WITH_INIT(string_type(*static_cast<string_type*>(result->Pointer)));
						break;
					}
				}
			}
			else
				console_out << _T("ERROR : Method '") << chars << _T("' Not Found\n");
		}
		GOTO_OPCODE();

	opt_sPUSH:
		++code; {
			PRINT_OPCODE();
			vm_int_t integer;
			integer.Int = 0;
			ASSIGN_4(integer.Chars, code);

			char_type * chars = new char_type[integer.Int + 1];
			for (int i = integer.Int - 1; i >= 0; --i)
				chars[i] = *++code;

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

	opt_lADD:
	opt_lSUB:
	opt_lMUL:
	opt_lDIV:
	opt_I2D:
		++code;
		PRINT_OPCODE();
		CONVERT(Int, Double, vm_object::vm_object_type::DOUBLE);
		GOTO_OPCODE();

	opt_D2I:
		++code;
		PRINT_OPCODE();
		CONVERT(Double, Int, vm_object::vm_object_type::INT);
		GOTO_OPCODE();

	opt_I2B:
		++code;
		PRINT_OPCODE();
		CONVERT(Int, Bool, vm_object::vm_object_type::BOOL);
		GOTO_OPCODE();

	opt_B2I:
		++code;
		PRINT_OPCODE();
		CONVERT(Bool, Int, vm_object::vm_object_type::INT);
		GOTO_OPCODE();

	opt_D2B:
		++code;
		PRINT_OPCODE();
		CONVERT(Double, Bool, vm_object::vm_object_type::BOOL);
		GOTO_OPCODE();

	opt_B2D:
		++code;
		PRINT_OPCODE();
		CONVERT(Bool, Double, vm_object::vm_object_type::DOUBLE);
		GOTO_OPCODE();

	initSystem:
		void** testCodes = gotoAddresses; // this dummy codes used by vc++ 
		STORE_ADDRESS(0 /*OPT_HALT*/, opt_HALT);
		STORE_ADDRESS(1 /*OPT_iADD*/, opt_iADD);
		STORE_ADDRESS(2 /*OPT_dADD*/, opt_dADD);
		STORE_ADDRESS(3 /*OPT_lADD*/, opt_lADD);

		STORE_ADDRESS(4 /*OPT_iSUB*/, opt_iSUB);
		STORE_ADDRESS(5 /*OPT_dSUB*/, opt_dSUB);
		STORE_ADDRESS(6 /*OPT_lSUB*/, opt_lSUB);

		STORE_ADDRESS(7 /*OPT_iMUL*/, opt_iMUL);
		STORE_ADDRESS(8 /*OPT_dMUL*/, opt_dMUL);
		STORE_ADDRESS(9 /*OPT_lMUL*/, opt_lMUL);

		STORE_ADDRESS(10 /*OPT_iDIV*/, opt_iDIV);
		STORE_ADDRESS(11 /*OPT_dDIV*/, opt_dDIV);
		STORE_ADDRESS(12 /*OPT_lDIV*/, opt_lDIV);

		STORE_ADDRESS(13 /*OPT_EQ*/, opt_EQ);
		STORE_ADDRESS(14 /*OPT_LT*/, opt_LT);
		STORE_ADDRESS(15 /*OPT_LTE*/, opt_LTE);
		STORE_ADDRESS(16 /*OPT_GT*/, opt_GT);
		STORE_ADDRESS(17 /*OPT_GTE*/, opt_GTE);

		STORE_ADDRESS(18 /*OPT_AND*/, opt_AND);
		STORE_ADDRESS(19 /*OPT_OR*/, opt_OR);
		STORE_ADDRESS(20 /*OPT_DUP*/, opt_DUP);
		STORE_ADDRESS(21 /*OPT_POP*/, opt_POP);
		STORE_ADDRESS(22 /*OPT_JMP*/, opt_JMP);

		STORE_ADDRESS(23 /*OPT_IF_EQ*/, opt_IF_EQ);

		STORE_ADDRESS(24 /*OPT_JIF*/, opt_JIF);
		STORE_ADDRESS(25 /*OPT_JNIF*/, opt_JNIF);
		STORE_ADDRESS(26 /*OPT_INC*/, opt_INC);
		STORE_ADDRESS(27 /*OPT_DINC*/, opt_DINC);

		STORE_ADDRESS(28 /*OPT_LOAD*/, opt_LOAD);
		STORE_ADDRESS(29 /*OPT_LOAD_0*/, opt_LOAD_0);
		STORE_ADDRESS(30 /*OPT_LOAD_1*/, opt_LOAD_1);
		STORE_ADDRESS(31 /*OPT_LOAD_2*/, opt_LOAD_2);
		STORE_ADDRESS(32 /*OPT_LOAD_3*/, opt_LOAD_3);
		STORE_ADDRESS(33 /*OPT_LOAD_4*/, opt_LOAD_4);

		STORE_ADDRESS(34 /*OPT_STORE*/, opt_STORE);
		STORE_ADDRESS(35 /*OPT_STORE_0*/, opt_STORE_0);
		STORE_ADDRESS(36 /*OPT_STORE_1*/, opt_STORE_1);
		STORE_ADDRESS(37 /*OPT_STORE_2*/, opt_STORE_2);
		STORE_ADDRESS(38 /*OPT_STORE_3*/, opt_STORE_3);
		STORE_ADDRESS(39 /*OPT_STORE_4*/, opt_STORE_4);

		STORE_ADDRESS(40 /*OPT_GLOAD*/, opt_GLOAD);
		STORE_ADDRESS(41 /*OPT_GLOAD_0*/, opt_GLOAD_0);
		STORE_ADDRESS(42 /*OPT_GLOAD_1*/, opt_GLOAD_1);
		STORE_ADDRESS(43 /*OPT_GLOAD_2*/, opt_GLOAD_2);
		STORE_ADDRESS(44 /*OPT_GLOAD_3*/, opt_GLOAD_3);
		STORE_ADDRESS(45 /*OPT_GLOAD_4*/, opt_GLOAD_4);

		STORE_ADDRESS(46 /*OPT_GSTORE*/, opt_GSTORE);
		STORE_ADDRESS(47 /*OPT_GSTORE_0*/, opt_GSTORE_0);
		STORE_ADDRESS(48 /*OPT_GSTORE_1*/, opt_GSTORE_1);
		STORE_ADDRESS(49 /*OPT_GSTORE_2*/, opt_GSTORE_2);
		STORE_ADDRESS(50 /*OPT_GSTORE_3*/, opt_GSTORE_3);
		STORE_ADDRESS(51 /*OPT_GSTORE_4*/, opt_GSTORE_4);

		STORE_ADDRESS(52 /*OPT_CALL*/, opt_CALL);
		STORE_ADDRESS(53 /*OPT_RETURN*/, opt_RETURN);

		STORE_ADDRESS(54 /*OPT_iPUSH*/, opt_iPUSH);
		STORE_ADDRESS(55 /*OPT_dPUSH*/, opt_dPUSH);
		STORE_ADDRESS(56 /*OPT_bPUSH*/, opt_bPUSH);
		STORE_ADDRESS(57 /*OPT_sPUSH*/, opt_sPUSH);
		STORE_ADDRESS(58 /*OPT_PRINT*/, opt_PRINT);
		STORE_ADDRESS(59 /*OPT_NEG*/, opt_NEG);

		STORE_ADDRESS(60 /*OPT_I2D*/, opt_I2D);
		STORE_ADDRESS(61 /*OPT_D2I*/, opt_D2I);
		STORE_ADDRESS(62 /*OPT_I2B*/, opt_I2B);
		STORE_ADDRESS(63 /*OPT_B2I*/, opt_B2I);
		STORE_ADDRESS(64 /*OPT_D2B*/, opt_D2B);
		STORE_ADDRESS(65 /*OPT_B2D*/, opt_B2D);
		STORE_ADDRESS(66 /*OPT_iPUSH_0*/, opt_iPUSH_0);
		STORE_ADDRESS(67 /*OPT_iPUSH_1*/, opt_iPUSH_1);
		STORE_ADDRESS(68 /*OPT_iPUSH_2*/, opt_iPUSH_2);
		STORE_ADDRESS(69 /*OPT_iPUSH_3*/, opt_iPUSH_3);
		STORE_ADDRESS(70 /*OPT_iPUSH_4*/, opt_iPUSH_4);
		STORE_ADDRESS(71 /*OPT_dPUSH_0*/, opt_dPUSH_0);
		STORE_ADDRESS(72 /*OPT_dPUSH_1*/, opt_dPUSH_1);
		STORE_ADDRESS(73 /*OPT_dPUSH_2*/, opt_dPUSH_2);
		STORE_ADDRESS(74 /*OPT_dPUSH_3*/, opt_dPUSH_3);
		STORE_ADDRESS(75 /*OPT_dPUSH_4*/, opt_dPUSH_4);
		STORE_ADDRESS(76 /*OPT_bPUSH_0*/, opt_bPUSH_0);
		STORE_ADDRESS(77 /*OPT_bPUSH_1*/, opt_bPUSH_1);
		STORE_ADDRESS(78 /*OPT_INVOKE*/, opt_INVOKE);
		STORE_ADDRESS(79 /*OPT_METHOD*/, opt_METHOD);
		STORE_ADDRESS(80 /*OPT_INITARRAY*/, opt_INITARRAY);
		STORE_ADDRESS(82 /*OPT_aPUSH*/, opt_aPUSH);
		STORE_ADDRESS(88 /*OPT_aGET*/, opt_aGET);
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
				vm_int_t integer;
				integer.Int = 0;
				ASSIGN_4(integer.Chars, code);
				console_out << _T(" ") << integer.Int;
				index += 4;
			}
			break;

			case vm_inst::OPT_JMP:
			case vm_inst::OPT_JIF:
			case vm_inst::OPT_IF_EQ:
			case vm_inst::OPT_JNIF:
			case vm_inst::OPT_iPUSH:
			{
				vm_int_t integer;
				integer.Int = 0;
				ASSIGN_4(integer.Chars, code);
				console_out << _T(" ") << integer.Int;
				index += 4;
			}
			break;

			case vm_inst::OPT_dPUSH:
			{
				vm_double_t d;
				d.Double = 0;
				ASSIGN_8(d.Chars, code);

				console_out << _T(" ") << d.Double;
				index += 8;
			}
			break;

			case vm_inst::OPT_bPUSH:
				// console_out << _T(" ") << (bool)*++code;
				++index;
				break;

			case vm_inst::OPT_INVOKE:
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

			case vm_inst::OPT_METHOD:
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

			case vm_inst::OPT_sPUSH:
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

			console_out << '\n';
			++code;
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
