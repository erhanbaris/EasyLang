#include "Vm.h"
#include "Console.h"
#include <unordered_map>


#define STORE(index, obj) *(currentStore->variables + index) = obj
#define LOAD(index) *(currentStore->variables + index)

#define GSTORE(index, obj) *(globalStore.variables + index) = obj
#define GLOAD(index) *(globalStore.variables + index)

#define PEEK() (currentStack[stackIndex - 1])
#define POP() (currentStack[--stackIndex])
#define SET(obj) currentStack[stackIndex - 1] = obj



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

template <int N>
struct StaticAssignment {
	inline static void assign(vm_char_t * data, char*& code)
	{
		data[N - 1] = *++code;
		StaticAssignment<N - 1>::assign(data, code);
	}
};
template <>
struct StaticAssignment<1> {
	inline static void assign(vm_char_t * data, char* & code)
	{
		data[0] = *++code;
	}
};

struct Operations {
	template<typename T>
	inline static void Add(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 2] = (T)currentStack[stackIndex - 1] + (T)currentStack[stackIndex - 2];
		--stackIndex;
	}

	template<typename T>
	inline static void Mul(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 2] = (T)currentStack[stackIndex - 1] * (T)currentStack[stackIndex - 2];
		--stackIndex;
	}

	template<typename T>
	inline static void Sub(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 2] = (T)currentStack[stackIndex - 2] - (T)currentStack[stackIndex - 1];
		--stackIndex;
	}

	template<typename T>
	inline static void Div(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 2] = (T)currentStack[stackIndex - 2] / (T)currentStack[stackIndex - 1];
		--stackIndex;
	}

	inline static void And(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 2] = currentStack[stackIndex - 2].Bool && currentStack[stackIndex - 1].Bool;
		--stackIndex;
	}

	inline static void Or(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 2] = currentStack[stackIndex - 2].Bool || currentStack[stackIndex - 1].Bool;
		--stackIndex;
	}

	template<typename T>
	inline static void Lt(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 2] = (T)currentStack[stackIndex - 2] < (T)currentStack[stackIndex - 1];
		--stackIndex;
	}

	template<typename T>
	inline static void Lte(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 2] = (T)currentStack[stackIndex - 2] <= (T)currentStack[stackIndex - 1];
		--stackIndex;
	}

	template<typename T>
	inline static void Gt(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 2] = (T)currentStack[stackIndex - 2] > (T)currentStack[stackIndex - 1];
		--stackIndex;
	}

	template<typename T>
	inline static void Gte(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 2] = (T)currentStack[stackIndex - 2] >= (T)currentStack[stackIndex - 1];
		--stackIndex;
	}

	template<typename T>
	inline static void Eq(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 2] = (T)currentStack[stackIndex - 2] == (T)currentStack[stackIndex - 1];
		--stackIndex;
	}

	inline static void Dup(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex] = currentStack[stackIndex];
		++stackIndex;
	}

	template<typename T>
	inline static void Inc(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 1] = ((T)currentStack[stackIndex - 1]) + 1;
	}

	template<typename T>
	inline static void Dinc(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 1] = ((T)currentStack[stackIndex - 1]) -1;
	}

	template<typename T>
	inline static void Neg(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 1] = (T)currentStack[stackIndex - 1] * -1;
	}

	template<typename From, typename To>
	inline static void Convert(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 1] = static_cast<To>((From)currentStack[stackIndex - 1]);
	}

	inline static vm_object& Pop(vm_object * currentStack, size_t & stackIndex)
	{
		return currentStack[--stackIndex];
	}

	template<typename  T>
	inline static T PopAs(vm_object * currentStack, size_t & stackIndex)
	{
		return (T)currentStack[--stackIndex];
	}

	template<typename T>
	inline static bool IsEqual(vm_object * currentStack, size_t & stackIndex)
	{
		return (T)currentStack[--stackIndex] == (T)currentStack[--stackIndex];
	}
    
    inline static void LoadAndPush(vm_object * currentStack, size_t & stackIndex, vm_store<vm_object>* currentStore, size_t index)
    {
        currentStack[stackIndex] = *(currentStore->variables + index);
        stackIndex++;
    }
    //*(currentStore->variables + index) = obj
    inline static void PopAndStore(vm_store<vm_object>* currentStore, size_t index, vm_object * currentStack, size_t & stackIndex)
    {
        --stackIndex;
        *(currentStore->variables + index) = currentStack[stackIndex];
    }
    
    inline static void Push(vm_object * currentStack, size_t & stackIndex, vm_object & obj)
	{
		currentStack[stackIndex] = obj;
		stackIndex++;
	}

	inline static void Push(vm_object * currentStack, size_t & stackIndex, vm_object && obj)
	{
		currentStack[stackIndex] = obj;
		stackIndex++;
    }
    
    template<typename T>
    inline static T Pop(vm_object * currentStack, size_t & stackIndex)
    {
        return (T)currentStack[--stackIndex];
    }
    
    template<typename T>
    inline static T Peek(vm_object * currentStack, size_t & stackIndex)
    {
        return (T)currentStack[stackIndex - 1];
    }

	inline static vm_object* Load(vm_store<vm_object>* currentStore, size_t index)
	{
		return (currentStore->variables + index);
	}

	/*inline static vm_object Store(vm_store<vm_object>* currentStore, size_t index)
	{
		return *(currentStore->variables + index);
	}*/
};

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


	void execute(char* code, size_t len, size_t startIndex)
	{
		char* startPoint = code;
		code += startIndex;
		while (1) {
			switch ((vm_inst)*code)
			{
			case vm_inst::OPT_iADD:
				Operations::Add<int>(currentStack, stackIndex);
                // console_out << _T("ADD : ") << Operations::Peek<int>(currentStack, stackIndex) << '\n';
				break;

			case vm_inst::OPT_iSUB:
				Operations::Sub<int>(currentStack, stackIndex);
                // console_out << _T("SUB : ") << Operations::Peek<int>(currentStack, stackIndex) << '\n';
				break;

			case vm_inst::OPT_iMUL:
				Operations::Mul<int>(currentStack, stackIndex);
				break;

			case vm_inst::OPT_iDIV:
				Operations::Div<int>(currentStack, stackIndex);
				break;

			case vm_inst::OPT_dADD:
				Operations::Add<double>(currentStack, stackIndex);
				break;

			case vm_inst::OPT_dSUB:
				Operations::Sub<double>(currentStack, stackIndex);
				break;

			case vm_inst::OPT_dMUL:
				Operations::Mul<double>(currentStack, stackIndex);
				break;

			case vm_inst::OPT_dDIV:
				Operations::Div<double>(currentStack, stackIndex);
				break;

			case vm_inst::OPT_EQ:
				Operations::Eq<int>(currentStack, stackIndex);
				break;

			case vm_inst::OPT_LT:
				Operations::Lt<int>(currentStack, stackIndex);
				break;

			case vm_inst::OPT_LTE:
				Operations::Lte<int>(currentStack, stackIndex);
                // console_out << _T("LTE : ") << Operations::Peek<int>(currentStack, stackIndex) << '\n';
				break;

			case vm_inst::OPT_GT:
				Operations::Gt<int>(currentStack, stackIndex);
				break;

			case vm_inst::OPT_GTE:
				Operations::Gte<int>(currentStack, stackIndex);
				break;

			case vm_inst::OPT_AND:
				Operations::And(currentStack, stackIndex);
				break;

			case vm_inst::OPT_OR:
				Operations::Or(currentStack, stackIndex);
				break;

			case vm_inst::OPT_DUP:
				Operations::Dup(currentStack, stackIndex);
				break;

			case vm_inst::OPT_JMP:
			{
				vm_int_t integer;
				integer.Int = 0;
				StaticAssignment<4>::assign(integer.Chars, code);

				code += integer.Int;
			}
				break;

			case vm_inst::OPT_METHOD:
			{
				vm_int_t integer;
				integer.Int = 0;
				StaticAssignment<4>::assign(integer.Chars, code);
				methods[integer.Chars] = startPoint - code;
			}
				break;

			case vm_inst::OPT_JIF:
			{
				if (Operations::PopAs<bool>(currentStack, stackIndex))
					code += 4;
				else
				{
					vm_int_t integer;
					StaticAssignment<4>::assign(integer.Chars, code);
					code += integer.Int;
				}
			}
			break;

			case vm_inst::OPT_IF_EQ:
			{
				if (Operations::IsEqual<bool>(currentStack, stackIndex))
					code += 4;
				else
				{
					vm_int_t integer;
					StaticAssignment<4>::assign(integer.Chars, code);
					code += integer.Int;
				}
			}
			break;

			case vm_inst::OPT_JNIF:
			{
				if (!Operations::IsEqual<bool>(currentStack, stackIndex))
				{
					vm_int_t integer;
					integer.Int = 0;
					StaticAssignment<4>::assign(integer.Chars, code);
					code += integer.Int;
				}
				else
					code += 4;
			}
			break;

			case vm_inst::OPT_INC:
				Operations::Inc<int>(currentStack, stackIndex);
				break;
			
			case vm_inst::OPT_NEG:
				Operations::Neg<int>(currentStack, stackIndex);
				break;

			case vm_inst::OPT_DINC:
				Operations::Dinc<int>(currentStack, stackIndex);
				break;

			case vm_inst::OPT_LOAD:
                Operations::LoadAndPush(currentStack, stackIndex, currentStore, *++code);
				break;

			case vm_inst::OPT_LOAD_0:
				Operations::LoadAndPush(currentStack, stackIndex, currentStore, 0);
                // console_out << _T("LOAD_0 : ") << Operations::Peek<int>(currentStack, stackIndex) << '\n';
				break;

			case vm_inst::OPT_LOAD_1:
                Operations::LoadAndPush(currentStack, stackIndex, currentStore, 1);
                // console_out << _T("LOAD_1 : ") << Operations::Peek<int>(currentStack, stackIndex) << '\n';
				break;

			case vm_inst::OPT_LOAD_2:
				Operations::LoadAndPush(currentStack, stackIndex, currentStore, 2);
                // console_out << _T("LOAD_2 : ") << Operations::Peek<int>(currentStack, stackIndex) << '\n';
				break;

			case vm_inst::OPT_LOAD_3:
				Operations::LoadAndPush(currentStack, stackIndex, currentStore, 3);
				break;

			case vm_inst::OPT_LOAD_4:
				Operations::LoadAndPush(currentStack, stackIndex, currentStore, 4);
				break;

			case vm_inst::OPT_STORE:
                Operations::PopAndStore(currentStore, *++code, currentStack, stackIndex);
				break;

			case vm_inst::OPT_STORE_0:
				Operations::PopAndStore(currentStore, 0, currentStack, stackIndex);
				break;

			case vm_inst::OPT_STORE_1:
				Operations::PopAndStore(currentStore, 1, currentStack, stackIndex);
				break;

			case vm_inst::OPT_STORE_2:
				Operations::PopAndStore(currentStore, 2, currentStack, stackIndex);
				break;

			case vm_inst::OPT_STORE_3:
				Operations::PopAndStore(currentStore, 3, currentStack, stackIndex);
				break;

			case vm_inst::OPT_STORE_4:
				Operations::PopAndStore(currentStore, 4, currentStack, stackIndex);
				break;


			case vm_inst::OPT_GLOAD:
				Operations::Push(currentStack, stackIndex, GLOAD(*++code));
				break;

			case vm_inst::OPT_GLOAD_0:
				Operations::Push(currentStack, stackIndex, GLOAD(0));
				break;

			case vm_inst::OPT_GLOAD_1:
				Operations::Push(currentStack, stackIndex, GLOAD(1));
				break;

			case vm_inst::OPT_GLOAD_2:
				Operations::Push(currentStack, stackIndex, GLOAD(2));
				break;

			case vm_inst::OPT_GLOAD_3:
				Operations::Push(currentStack, stackIndex, GLOAD(3));
				break;

			case vm_inst::OPT_GLOAD_4:
				Operations::Push(currentStack, stackIndex, GLOAD(4));
				break;

			case vm_inst::OPT_GSTORE:
				GSTORE(*++code, POP());
				break;

			case vm_inst::OPT_GSTORE_0:
				GSTORE(0, POP());
				break;

			case vm_inst::OPT_GSTORE_1:
				GSTORE(1, POP());
				break;

			case vm_inst::OPT_GSTORE_2:
				GSTORE(2, POP());
				break;

			case vm_inst::OPT_GSTORE_3:
				GSTORE(3, POP());
				break;

			case vm_inst::OPT_GSTORE_4:
				GSTORE(4, POP());
				break;

			case vm_inst::OPT_CALL:
			{
				currentStore = stores[++storesCount];
				vm_int_t integer;
				integer.Int = 0;
				StaticAssignment<4>::assign(integer.Chars, code);
                currentStore->startAddress = (code - startPoint);
				code += integer.Int;
			}
			break;

			case vm_inst::OPT_RETURN:
			{
				code = startPoint + currentStore->startAddress;
				currentStore = stores[--storesCount];
			}
			break;

			case vm_inst::OPT_POP:
				POP();
				break;

			case vm_inst::OPT_iPUSH: {
				vm_int_t integer;
				integer.Int = 0;
				StaticAssignment<4>::assign(integer.Chars, code);
				Operations::Push(currentStack, stackIndex, integer.Int);
                // console_out << _T("PUSH : ") << Operations::Peek<int>(currentStack, stackIndex) << '\n';
			}
			break;

            case vm_inst::OPT_iPUSH_0:
                Operations::Push(currentStack, stackIndex, 0);
                break;

            case vm_inst::OPT_iPUSH_1:
                Operations::Push(currentStack, stackIndex, 1);
                break;

            case vm_inst::OPT_iPUSH_2:
                Operations::Push(currentStack, stackIndex, 2);
                break;

            case vm_inst::OPT_iPUSH_3:
                Operations::Push(currentStack, stackIndex, 3);
                break;

            case vm_inst::OPT_iPUSH_4:
                Operations::Push(currentStack, stackIndex, 4);
                break;

            case vm_inst::OPT_dPUSH_0:
                Operations::Push(currentStack, stackIndex, 0.0);
                break;

            case vm_inst::OPT_dPUSH_1:
                Operations::Push(currentStack, stackIndex, 1.0);
                break;

            case vm_inst::OPT_dPUSH_2:
                Operations::Push(currentStack, stackIndex, 2.0);
                break;

            case vm_inst::OPT_dPUSH_3:
                Operations::Push(currentStack, stackIndex, 3.0);
                break;

            case vm_inst::OPT_dPUSH_4:
                Operations::Push(currentStack, stackIndex, 4.0);
                break;

            case vm_inst::OPT_bPUSH_0:
                Operations::Push(currentStack, stackIndex, false);
                break;

            case vm_inst::OPT_bPUSH_1:
                Operations::Push(currentStack, stackIndex, true);
                break;

			case vm_inst::OPT_dPUSH: {
				vm_double_t d;
				d.Double = 0.0;
				StaticAssignment<8>::assign(d.Chars, code);
				Operations::Push(currentStack, stackIndex, d.Double);
			}
				break;


			case vm_inst::OPT_bPUSH:
				Operations::Push(currentStack, stackIndex, (bool)*++code);
				break;
                    
            case vm_inst::OPT_INVOKE: {
                vm_int_t integer;
                integer.Int = 0;
                StaticAssignment<1>::assign(integer.Chars, code);
                
                char * chars = new char[integer.Int + 1];
                for (int i = integer.Int - 1; i >= 0; i--)
                    chars[i] = *++code;
                    
                chars[integer.Int] = '\0';
                if (nativeMethods.find(chars) != nativeMethodsEnd)
					nativeMethods[chars](system);
                else
                    console_out << _T("ERROR : Method '") << chars << _T("' Not Found\n");
            }
                break;

			case vm_inst::OPT_sPUSH: {
				vm_int_t integer;
				integer.Int = 0;
				StaticAssignment<4>::assign(integer.Chars, code);

				char * chars = new char[integer.Int + 1];
				for (int i = integer.Int - 1; i >= 0; i--)
					chars[i] = *++code;
				
				chars[integer.Int] = '\0';
				Operations::Push(currentStack, stackIndex, chars);
			}
				break;

			case vm_inst::OPT_PRINT:
				// console_out << Operations::PopAs<int>(currentStack, stackIndex);
				break;

			case vm_inst::OPT_HALT:
				return;

			case OPT_lADD:break;
			case OPT_lSUB:break;
			case OPT_lMUL:break;
			case OPT_lDIV:break;

			case OPT_I2D:
				Operations::Convert<int, double>(currentStack, stackIndex);
                break;

            case OPT_D2I:
				Operations::Convert<double, int>(currentStack, stackIndex);
                break;

            case OPT_I2B:
				Operations::Convert<int, bool>(currentStack, stackIndex);
                break;

            case OPT_B2I:
				Operations::Convert<bool, int>(currentStack, stackIndex);
                break;

            case OPT_D2B:
				Operations::Convert<double, bool>(currentStack, stackIndex);
                break;

            case OPT_B2D:
				Operations::Convert<bool, double>(currentStack, stackIndex);
                break;
			}

			++code;
		}
	}

	void dumpOpcode(char* code, size_t len)
	{
		size_t index = 0;
		while (index < len) {
			console_out << _T(">>> ") << index++ << _T(". ");
			COLOR_RED(vm_instToString((vm_inst)*code));

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
					StaticAssignment<4>::assign(integer.Chars, code);
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
					StaticAssignment<4>::assign(integer.Chars, code);
                    console_out << _T(" ") << integer.Int;
                    index += 4;
                }
                    break;

                case vm_inst ::OPT_dPUSH:
                {
					vm_double_t d;
					d.Double = 0;
					StaticAssignment<8>::assign(d.Chars, code);

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
                    StaticAssignment<1>::assign(integer.Chars, code);
                    index += 1;
                    index += integer.Int;
                    
                    char * chars = new char[integer.Int + 1];
                    for (int i = integer.Int - 1; i >= 0; i--)
                        chars[i] = *++code;
                    
                    chars[integer.Int] = '\0';
                    
                    console_out << _T(" \"") << chars << _T("\"");
                }
                    break;
                    
				case vm_inst::OPT_METHOD:
				{
					vm_int_t integer;
					integer.Int = 0;
					StaticAssignment<4>::assign(integer.Chars, code);
					index += 4;
					index += integer.Int;

					char * chars = new char[integer.Int + 1];
					for (int i = integer.Int - 1; i >= 0; i--)
						chars[i] = *++code;

					chars[integer.Int] = '\0';

					console_out << _T(" \"") << chars << _T("\"");
				}
					break;

				case vm_inst::OPT_sPUSH:
				{
					vm_int_t integer;
					integer.Int = 0;
					StaticAssignment<4>::assign(integer.Chars, code);
					index += 4;
					index += integer.Int;

					char * chars = new char[integer.Int + 1];
					for (int i = integer.Int - 1; i >= 0; i--)
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
				console_out << string_type(static_cast<char*>(item.Pointer));
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
	void dump(char* code, size_t len)
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

void vm_system::execute(char* code, size_t len, size_t startIndex)
{
	impl->execute(code, len, startIndex);
}

void vm_system::dump(char* code, size_t len)
{
    impl->dump(code, len);
}

void vm_system::dumpOpcode(char* code, size_t len)
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

vm_object* vm_system::getObject()
{
	if (impl->stackIndex > 0)
		return &impl->currentStack[--impl->stackIndex];

	return nullptr;
}

void vm_system::addMethod(string_type const & name, VmMethod method)
{
    this->impl->nativeMethods[name] = method;
    this->impl->nativeMethodsEnd = this->impl->nativeMethods.end();
}
