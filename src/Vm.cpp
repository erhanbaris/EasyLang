#include "Vm.h"
#include "Console.h"

#define BYTES_TO_INT(one,two) (one + ( 256 * two ))

#define STORE(index, obj) *(currentStore->variables + index) = obj
#define LOAD(index) *(currentStore->variables + index)

#define GSTORE(index, obj) *(globalStore.variables + index) = obj
#define GLOAD(index) *(globalStore.variables + index)

#if _DEBUG
#define PEEK() (currentStack[stackIndex - 1])
#define POP() (currentStack[--stackIndex])
#define PUSH(obj) currentStack[stackIndex++] = (obj)
#define SET(obj) currentStack[stackIndex - 1] = obj

#else 
#define PEEK() (*(currentStack - 1))
#define POP() (*(--currentStack))
#define PUSH(obj) *currentStack = obj; ++currentStack
#define SET(obj) (*(currentStack - 1)) = obj
#endif

template <int N>
struct StaticAssignment {
	static void assign(vm_char_t * data, char*& code)
	{
		data[N - 1] = *++code;
		StaticAssignment<N - 1>::assign(data, code);
	}
};
template <>
struct StaticAssignment<1> {
	static void assign(vm_char_t * data, char* & code)
	{
		data[0] = *++code;
	}
};

struct Operations {
	template<typename T>
	static void Add(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 2] = (T)currentStack[stackIndex - 1] + (T)currentStack[stackIndex - 2];
		--stackIndex;
	}

	template<typename T>
	static void Mul(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 2] = (T)currentStack[stackIndex - 1] * (T)currentStack[stackIndex - 2];
		--stackIndex;
	}

	template<typename T>
	static void Sub(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 2] = (T)currentStack[stackIndex - 2] - (T)currentStack[stackIndex - 1];
		--stackIndex;
	}

	template<typename T>
	static void Div(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 2] = (T)currentStack[stackIndex - 2] / (T)currentStack[stackIndex - 1];
		--stackIndex;
	}

	static void And(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 2] = currentStack[stackIndex - 2].Bool && currentStack[stackIndex - 1].Bool;
		--stackIndex;
	}

	static void Or(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 2] = currentStack[stackIndex - 2].Bool || currentStack[stackIndex - 1].Bool;
		--stackIndex;
	}

	template<typename T>
	static void Lt(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 2] = (T)currentStack[stackIndex - 2] < (T)currentStack[stackIndex - 1];
		--stackIndex;
	}

	template<typename T>
	static void Lte(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 2] = (T)currentStack[stackIndex - 2] <= (T)currentStack[stackIndex - 1];
		--stackIndex;
	}

	template<typename T>
	static void Gt(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 2] = (T)currentStack[stackIndex - 2] > (T)currentStack[stackIndex - 1];
		--stackIndex;
	}

	template<typename T>
	static void Gte(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 2] = (T)currentStack[stackIndex - 2] >= (T)currentStack[stackIndex - 1];
		--stackIndex;
	}

	template<typename T>
	static void Eq(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 2] = (T)currentStack[stackIndex - 2] == (T)currentStack[stackIndex - 1];
		--stackIndex;
	}

	static void Dup(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex] = currentStack[stackIndex];
		++stackIndex;
	}

	template<typename T>
	static void Inc(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 1] = ((T)currentStack[stackIndex - 1]) + 1;
	}

	template<typename T>
	static void Dinc(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 1] = ((T)currentStack[stackIndex - 1]) -1;
	}

	template<typename T>
	static void Neg(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 1] = (T)currentStack[stackIndex - 1] * -1;
	}

	template<typename From, typename To>
	static void Convert(vm_object * currentStack, size_t & stackIndex)
	{
		currentStack[stackIndex - 1] = static_cast<To>((From)currentStack[stackIndex - 1]);
	}

	static vm_object& Pop(vm_object * currentStack, size_t & stackIndex)
	{
		return currentStack[--stackIndex];
	}

	template<typename  T>
	static T PopAs(vm_object * currentStack, size_t & stackIndex)
	{
		return (T)currentStack[--stackIndex];
	}

	template<typename T>
	static bool IsEqual(vm_object * currentStack, size_t & stackIndex)
	{
		return (T)currentStack[--stackIndex] == (T)currentStack[--stackIndex];
	}
};

template <typename T>
class vm_store
{
public:
	vm_store()
	{
		variables = new T[64];
	}

	~vm_store()
	{
		if (variables != nullptr)
			delete[] variables;
	}

	size_t startAddress;
	T* variables{nullptr};
};

class vm_system_impl
{
public:
	vm_system_impl()
	{
		const int STORE_SIZE = 1024;
		currentStack = new vm_object[1024 * 512];
		currentStore = new vm_store<vm_object>;
		stores = new vm_store<vm_object>*[STORE_SIZE];

		for (size_t i = 0; i < STORE_SIZE; ++i)
			stores[i] = new vm_store<vm_object>;

		stores[0] = currentStore;
		storesCount = 0;
#if _DEBUG
		stackIndex = 0;
#endif
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

	vm_object* currentStack{nullptr};
#if _DEBUG
	size_t stackIndex;
#endif

	void execute(char* code, size_t len, size_t startIndex)
	{
		char* startPoint = code;
		code += startIndex;
		while (1) {
			switch ((vm_inst)*code)
			{
			case vm_inst::OPT_iADD:
				Operations::Add<int>(currentStack, stackIndex);
				break;

			case vm_inst::OPT_iSUB:
				Operations::Sub<int>(currentStack, stackIndex);
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

				code = startPoint + (integer.Int + 1);
			}
				break;

			case vm_inst::OPT_JIF:
			{
				if (Operations::PopAs<bool>(currentStack, stackIndex))
					++code;
				else
				{
					vm_int_t integer;
					StaticAssignment<4>::assign(integer.Chars, code);
					code = startPoint + (integer.Int + 1);
				}
			}
			break;

			case vm_inst::OPT_IF_EQ:
			{
				if (Operations::IsEqual<bool>(currentStack, stackIndex))
					++code;
				else
				{
					vm_int_t integer;
					StaticAssignment<4>::assign(integer.Chars, code);
					code = startPoint + (integer.Int + 1);
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
					code = startPoint + (integer.Int + 1);
				}
				else
					++code;
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
				PUSH(LOAD(*++code));
				break;

			case vm_inst::OPT_LOAD_0:
				PUSH(LOAD(0));
				break;

			case vm_inst::OPT_LOAD_1:
				PUSH(LOAD(1));
				break;

			case vm_inst::OPT_LOAD_2:
				PUSH(LOAD(2));
				break;

			case vm_inst::OPT_LOAD_3:
				PUSH(LOAD(3));
				break;

			case vm_inst::OPT_LOAD_4:
				PUSH(LOAD(4));
				break;

			case vm_inst::OPT_STORE:
				STORE(*++code, POP());
				break;

			case vm_inst::OPT_STORE_0:
				STORE(0, POP());
				break;

			case vm_inst::OPT_STORE_1:
				STORE(1, POP());
				break;

			case vm_inst::OPT_STORE_2:
				STORE(2, POP());
				break;

			case vm_inst::OPT_STORE_3:
				STORE(3, POP());
				break;

			case vm_inst::OPT_STORE_4:
				STORE(4, POP());
				break;


			case vm_inst::OPT_GLOAD:
				PUSH(GLOAD(*++code));
				break;

			case vm_inst::OPT_GLOAD_0:
			{
				auto item = GLOAD(0);
				PUSH(GLOAD(0));
			}
				break;

			case vm_inst::OPT_GLOAD_1:
				PUSH(GLOAD(1));
				break;

			case vm_inst::OPT_GLOAD_2:
				PUSH(GLOAD(2));
				break;

			case vm_inst::OPT_GLOAD_3:
				PUSH(GLOAD(3));
				break;

			case vm_inst::OPT_GLOAD_4:
				PUSH(GLOAD(4));
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
				currentStore->startAddress = (code - startPoint) + 1;
				vm_int_t integer;
				integer.Int = 0;
				StaticAssignment<4>::assign(integer.Chars, code);
				code = startPoint + (integer.Int - 1);
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
				PUSH(integer.Int);
			}
			break;

			case vm_inst::OPT_dPUSH: {
				vm_double_t d;
				d.Double = 0.0;
				StaticAssignment<8>::assign(d.Chars, code);
				PUSH(d.Double);
			}
				break;


			case vm_inst::OPT_bPUSH:
				PUSH((bool)*++code);
				break;

			case vm_inst::OPT_sPUSH: {
				vm_int_t integer;
				integer.Int = 0;
				StaticAssignment<4>::assign(integer.Chars, code);
				integer.Int;

				char * chars = new char[integer.Int + 1];
				for (int i = integer.Int - 1; i >= 0; i--)
					chars[i] = *++code;
				
				chars[integer.Int] = '\0';
				PUSH(chars);
			}
				break;

			case vm_inst::OPT_PRINT:
				console_out << Operations::PopAs<int>(currentStack, stackIndex);
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

	void dump(char* code, size_t len)
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
					console_out << _T(" ") << (int)*++code;
					++index;
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
                    index += 2;
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
                    console_out << _T(" ") << (bool)*++code;
                    break;

                case OPT_sPUSH:
                    break;
            }

			console_out << '\n';
			++code;
		}

		console_out << '\n';
	}
};

vm_system::vm_system()
{
	this->impl = new vm_system_impl();
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

size_t vm_system::getUInt()
{
#if _DEBUG
	return impl->currentStack[impl->stackIndex - 1].Int;
#else
	return *(impl->currentStack - 1);
#endif
}

vm_object vm_system::getObject()
{
#if _DEBUG
    return impl->currentStack[impl->stackIndex - 1];
#else
    return *(impl->currentStack - 1);
#endif
}