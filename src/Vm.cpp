#include "Vm.h"
#include "Console.h"
#include <unordered_map>


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
	data [7] = *( code + 1);\
	data [6] = *( code + 2);\
	data [5] = *( code + 3);\
	data [4] = *( code + 4);\
	data [3] = *( code + 5);\
	data [2] = *( code + 6);\
	data [1] = *( code + 7);\
	data [0] = *( code + 8);\
	code += 8;

#define ASSIGN_4(data, code)\
	data [3] = *( code + 1);\
	data [2] = *( code + 2);\
	data [1] = *( code + 3);\
	data [0] = *( code + 4);\
	code += 4;

#define ASSIGN_3(data, code)\
	data [2] = *( code + 1);\
	data [1] = *( code + 2);\
	data [0] = *( code + 3);\
	code += 3;

#define ASSIGN_2(data, code)\
	data [1] = *( code + 1);\
	data [0] = *( code + 2);\
	code += 2;

#define ASSIGN_1(data, code)\
	data [0] = *code++;

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
	vm_store<vm_object>** stores{nullptr};
	vm_store<vm_object>* currentStore{ nullptr };
    vm_store<vm_object> globalStore;
    std::unordered_map<string_type, VmMethod> nativeMethods;
    std::unordered_map<string_type, VmMethod>::iterator nativeMethodsEnd;
	std::unordered_map<string_type, size_t> methods;
	std::unordered_map<string_type, size_t>::iterator methodsEnd;

	vm_object* currentStack{nullptr};
	size_t stackIndex;
    vm_system* system {nullptr};


	void execute(char_type* code, size_t len, size_t startIndex)
	{
		char_type* startPoint = code;
		code += startIndex;
		while (1) {
			switch ((vm_inst)*code)
			{
			case vm_inst::OPT_iADD:
                PRINT_OPCODE();
                OPERATION_ADD(Int, vm_object::vm_object_type::INT);
				break;

			case vm_inst::OPT_iSUB:
                PRINT_OPCODE();
				OPERATION_SUB(Int, vm_object::vm_object_type::INT);
				break;

			case vm_inst::OPT_iMUL:
                PRINT_OPCODE();
				OPERATION_MUL(Int, vm_object::vm_object_type::INT);
				break;

			case vm_inst::OPT_iDIV:
                PRINT_OPCODE();
				OPERATION_DIV(Int, vm_object::vm_object_type::INT);
				break;

			case vm_inst::OPT_dADD:
                PRINT_OPCODE();
				OPERATION_ADD(Double, vm_object::vm_object_type::DOUBLE);
				break;

			case vm_inst::OPT_dSUB:
                PRINT_OPCODE();
				OPERATION_SUB(Double, vm_object::vm_object_type::DOUBLE);
				break;

			case vm_inst::OPT_dMUL:
                PRINT_OPCODE();
				OPERATION_MUL(Double, vm_object::vm_object_type::DOUBLE);
				break;

			case vm_inst::OPT_dDIV:
                PRINT_OPCODE();
				OPERATION_DIV(Double, vm_object::vm_object_type::DOUBLE);
				break;

			case vm_inst::OPT_EQ:
                PRINT_OPCODE();
				OPERATION_EQ(Bool, vm_object::vm_object_type::BOOL, Int);
				break;

			case vm_inst::OPT_LT:
                PRINT_OPCODE();
				OPERATION_LT(Bool, vm_object::vm_object_type::BOOL, Int);
				break;

			case vm_inst::OPT_LTE:
                PRINT_OPCODE();
				OPERATION_LTE(Bool, vm_object::vm_object_type::BOOL, Int);
				break;

			case vm_inst::OPT_GT:
                PRINT_OPCODE();
				OPERATION_GT(Bool, vm_object::vm_object_type::BOOL, Int);
				break;

			case vm_inst::OPT_GTE:
                PRINT_OPCODE();
				OPERATION_GTE(Bool, vm_object::vm_object_type::BOOL, Int);
				break;

			case vm_inst::OPT_AND:
                PRINT_OPCODE();
				OPERATION_AND(Bool, vm_object::vm_object_type::BOOL);
				break;

			case vm_inst::OPT_OR:
                PRINT_OPCODE();
				OPERATION_OR(Bool, vm_object::vm_object_type::BOOL);
				break;

			case vm_inst::OPT_DUP:
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
				
				break;

			case vm_inst::OPT_JMP:
			{
                PRINT_OPCODE();
				FUNC_BEGIN()
				vm_int_t integer;
				integer.Int = 0;

				ASSIGN_4(integer.Chars, code);

				code += integer.Int;
				FUNC_END()
			}
				break;

			case vm_inst::OPT_METHOD:
			{
                PRINT_OPCODE();
				FUNC_BEGIN()
				vm_int_t integer;
				integer.Int = 0;
				ASSIGN_4(integer.Chars, code);
				methods[integer.Chars] = startPoint - code;
				FUNC_END()
			}
				break;

			case vm_inst::OPT_INITARRAY:
                PRINT_OPCODE();
				FUNC_BEGIN()
				PUSH_WITH_ASSIGN(new vm_array);
				FUNC_END()
                break;

			case vm_inst::OPT_aPUSH:
			{
                PRINT_OPCODE();
				FUNC_BEGIN()
				vm_object& data = POP();
				vm_array* array = static_cast<vm_array*>(PEEK().Pointer);
				array->push(data);
				FUNC_END()
			}
			break;

			case vm_inst::OPT_aGET:
			{
                PRINT_OPCODE();
				FUNC_BEGIN()
				vm_object& data = POP();
				vm_array* array = static_cast<vm_array*>(PEEK().Pointer);
				PUSH_WITH_ASSIGN(&array->Array[data.Int]);
				FUNC_END()
			}
			break;
			
			case vm_inst::OPT_JIF:
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
			break;

			case vm_inst::OPT_IF_EQ:
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
			break;

			case vm_inst::OPT_JNIF:
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
			break;

			case vm_inst::OPT_INC:
                PRINT_OPCODE();
				INC(currentStack, stackIndex, Int);
				break;
			
			case vm_inst::OPT_NEG:
                PRINT_OPCODE();
				NEG(currentStack, stackIndex, Int);
				break;

			case vm_inst::OPT_DINC:
                PRINT_OPCODE();
				DINC(currentStack, stackIndex, Int);
				break;

			case vm_inst::OPT_LOAD:
			{
                PRINT_OPCODE();
				int data = *++code;
				LOAD_AND_PUSH(data);
			}
				break;

			case vm_inst::OPT_LOAD_0:
                PRINT_OPCODE();
				LOAD_AND_PUSH(0);
				break;

			case vm_inst::OPT_LOAD_1:
                PRINT_OPCODE();
				LOAD_AND_PUSH(1);
				break;

			case vm_inst::OPT_LOAD_2:
                PRINT_OPCODE();
				LOAD_AND_PUSH(2);
				break;

			case vm_inst::OPT_LOAD_3:
                PRINT_OPCODE();
				LOAD_AND_PUSH(3);
				break;

			case vm_inst::OPT_LOAD_4:
                PRINT_OPCODE();
				LOAD_AND_PUSH(4);
				break;

			case vm_inst::OPT_STORE:
				PRINT_OPCODE();
				STORE(*++code, POP());
				break;

			case vm_inst::OPT_STORE_0:
				PRINT_OPCODE();
				STORE(0, POP());
				break;

			case vm_inst::OPT_STORE_1:
				PRINT_OPCODE();
				STORE(1, POP());
				break;

			case vm_inst::OPT_STORE_2:
				PRINT_OPCODE();
				STORE(2, POP());
				break;

			case vm_inst::OPT_STORE_3:
				PRINT_OPCODE();
				STORE(3, POP());
				break;

			case vm_inst::OPT_STORE_4:
				PRINT_OPCODE();
				STORE(4, POP());
				break;

			case vm_inst::OPT_GLOAD:
                PRINT_OPCODE();
				PUSH_WITH_ASSIGN(GLOAD(*++code));
				break;

			case vm_inst::OPT_GLOAD_0:
                PRINT_OPCODE();
				PUSH_WITH_ASSIGN(GLOAD(0));
				break;

			case vm_inst::OPT_GLOAD_1:
                PRINT_OPCODE();
                PUSH_WITH_ASSIGN(GLOAD(1));
                break;

			case vm_inst::OPT_GLOAD_2:
                PRINT_OPCODE();
				PUSH_WITH_ASSIGN(GLOAD(2));
				break;

			case vm_inst::OPT_GLOAD_3:
                PRINT_OPCODE();
				PUSH_WITH_ASSIGN(GLOAD(3));
				break;

			case vm_inst::OPT_GLOAD_4:
                PRINT_OPCODE();
				PUSH_WITH_ASSIGN(GLOAD(4));
				break;

			case vm_inst::OPT_GSTORE:
                PRINT_OPCODE();
				GSTORE(*++code, POP());
				break;

			case vm_inst::OPT_GSTORE_0:
                PRINT_OPCODE();
				GSTORE(0, POP());
				break;

			case vm_inst::OPT_GSTORE_1:
                PRINT_OPCODE();
				GSTORE(1, POP());
				break;

			case vm_inst::OPT_GSTORE_2:
                PRINT_OPCODE();
				GSTORE(2, POP());
				break;

			case vm_inst::OPT_GSTORE_3:
                PRINT_OPCODE();
				GSTORE(3, POP());
				break;

			case vm_inst::OPT_GSTORE_4:
                PRINT_OPCODE();
				GSTORE(4, POP());
				break;

			case vm_inst::OPT_CALL:
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
			break;

			case vm_inst::OPT_RETURN:
			{
                PRINT_OPCODE();
				FUNC_BEGIN()
				code = startPoint + currentStore->startAddress;
				currentStore = stores[--storesCount];
				FUNC_END()
			}
			break;

			case vm_inst::OPT_POP:
                PRINT_OPCODE();
				POP();
				break;

			case vm_inst::OPT_iPUSH: {
                PRINT_OPCODE();
				FUNC_BEGIN()
				vm_int_t integer;
				integer.Int = 0;
				ASSIGN_4(integer.Chars, code);
				PUSH_WITH_ASSIGN(integer.Int);
				FUNC_END()
                // console_out << _T("PUSH : ") << Operations::Peek<int>(currentStack, stackIndex) << '\n';
			}
			break;

            case vm_inst::OPT_iPUSH_0:
                PRINT_OPCODE();
                PUSH_WITH_ASSIGN(0);
                break;

            case vm_inst::OPT_iPUSH_1:
                PRINT_OPCODE();
                PUSH_WITH_ASSIGN(1);
                break;

            case vm_inst::OPT_iPUSH_2:
                PRINT_OPCODE();
                PUSH_WITH_ASSIGN(2);
                break;

            case vm_inst::OPT_iPUSH_3:
                PRINT_OPCODE();
                PUSH_WITH_ASSIGN(3);
                break;

            case vm_inst::OPT_iPUSH_4:
                PRINT_OPCODE();
                PUSH_WITH_ASSIGN(4);
                break;

            case vm_inst::OPT_dPUSH_0:
                PRINT_OPCODE();
                PUSH_WITH_ASSIGN(0.0);
                break;

            case vm_inst::OPT_dPUSH_1:
                PRINT_OPCODE();
                PUSH_WITH_ASSIGN(1.0);
                break;

            case vm_inst::OPT_dPUSH_2:
                PRINT_OPCODE();
                PUSH_WITH_ASSIGN(2.0);
                break;

            case vm_inst::OPT_dPUSH_3:
                PRINT_OPCODE();
                PUSH_WITH_ASSIGN(3.0);
                break;

            case vm_inst::OPT_dPUSH_4:
                PRINT_OPCODE();
                PUSH_WITH_ASSIGN(4.0);
                break;

            case vm_inst::OPT_bPUSH_0:
                PRINT_OPCODE();
                PUSH_WITH_ASSIGN(false);
                break;

            case vm_inst::OPT_bPUSH_1:
                PRINT_OPCODE();
                PUSH_WITH_ASSIGN(true);
                break;

			case vm_inst::OPT_dPUSH: {
                PRINT_OPCODE();
				vm_double_t d;
				d.Double = 0.0;
				ASSIGN_8(d.Chars, code);
				PUSH_WITH_ASSIGN(d.Double);
			}
				break;


			case vm_inst::OPT_bPUSH:
                PRINT_OPCODE();
				PUSH_WITH_ASSIGN((bool)*++code);
				break;
                    
            case vm_inst::OPT_INVOKE: {
                PRINT_OPCODE();
                vm_int_t integer;
                integer.Int = 0;
				ASSIGN_1(integer.Chars, code);
                
                char_type * chars = new char_type[integer.Int + 1];
                for (int i = integer.Int - 1; i >= 0; --i)
                    chars[i] = *++code;
                    
                chars[integer.Int] = '\0';
                if (nativeMethods.find(chars) != nativeMethodsEnd)
					nativeMethods[chars](system);
                else
                    console_out << _T("ERROR : Method '") << chars << _T("' Not Found\n");
            }
                break;

			case vm_inst::OPT_sPUSH: {
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
				break;

			case vm_inst::OPT_PRINT:
                PRINT_OPCODE();
				// console_out << Operations::PopAs<int>(currentStack, stackIndex);
				break;

			case vm_inst::OPT_HALT:
                PRINT_OPCODE();
				return;

			case OPT_lADD:break;
			case OPT_lSUB:break;
			case OPT_lMUL:break;
			case OPT_lDIV:break;

			case OPT_I2D:
                PRINT_OPCODE();
				CONVERT(Int, Double, vm_object::vm_object_type::DOUBLE);
                break;

            case OPT_D2I:
                PRINT_OPCODE();
				CONVERT(Double, Int, vm_object::vm_object_type::INT);
                break;

            case OPT_I2B:
                PRINT_OPCODE();
				CONVERT(Int, Bool, vm_object::vm_object_type::BOOL);
                break;

            case OPT_B2I:
                PRINT_OPCODE();
				CONVERT(Bool, Int, vm_object::vm_object_type::INT);
                break;

            case OPT_D2B:
                PRINT_OPCODE();
				CONVERT(Double, Bool, vm_object::vm_object_type::BOOL);
                break;

            case OPT_B2D:
                PRINT_OPCODE();
				CONVERT(Bool, Double, vm_object::vm_object_type::DOUBLE);
                break;
			}

			++code;
		}
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
                case vm_inst ::OPT_iPUSH:
                {
					vm_int_t integer;
					integer.Int = 0;
					ASSIGN_4(integer.Chars, code);
                    console_out << _T(" ") << integer.Int;
                    index += 4;
                }
                    break;

                case vm_inst ::OPT_dPUSH:
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
}

vm_system::~vm_system()
{
	delete impl;
}

void vm_system::execute(char_type* code, size_t len, size_t startIndex)
{
	impl->execute(code, len, startIndex);
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
