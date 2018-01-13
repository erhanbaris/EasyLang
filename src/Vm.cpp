#include "Vm.h"
#include "Console.h"

#define BYTES_TO_INT(one,two) (one + ( 256 * two ))

#define STORE(index, obj) *(currentStore->variables + index) = obj
#define LOAD(index) *(currentStore->variables + index)

#define GSTORE(index, obj) *(globalStore.variables + index) = obj
#define GLOAD(index) *(globalStore.variables + index)

#if _DEBUG
#define PEEK() currentStack[stackIndex - 1]
#define POP() currentStack[--stackIndex]
#define PUSH(obj) currentStack[stackIndex++] = obj;
#define SET(obj) currentStack[stackIndex - 1] = obj

#define iPEEK() (( PEEK() << 8) | (PEEK() & 0x00FF))
#define iPOP() (( POP() << 8) | (POP() & 0x00FF))
#define iPUSH(obj1, obj2) *currentStack = obj1; ++currentStack; *currentStack = obj2; ++currentStack;
#define iSET(obj) (*(currentStack - 1)) = obj

#else 
#define PEEK() (*(currentStack - 1))
#define POP() (*(--currentStack))
#define PUSH(obj) *currentStack = obj; ++currentStack;
#define SET(obj) (*(currentStack - 1)) = obj

#define iPEEK() (( PEEK() << 8) | (PEEK() & 0x00FF))
#define iPOP() (( POP() << 8) | (POP() & 0x00FF))
#define iPUSH(obj1, obj2) *currentStack = obj1; ++currentStack; *currentStack = obj2; ++currentStack;
#define iSET(obj) (*(currentStack - 1)) = obj

#endif

#define TO_INT(code) code ? 1 : 0;
#define TO_BOOL(code) code != 0

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
			{
				int a = POP().Int;
				int b = POP().Int;
				PUSH(b + a);
			}
				break;

			case vm_inst::OPT_iSUB:
			{
				int a = POP().Int;
				int b = POP().Int;
				PUSH(b - a);
			}
				break;

			case vm_inst::OPT_iMUL:
            {
                int a = POP().Int;
                int b = POP().Int;
                int c = a * b;
                PUSH(c);
            }
				break;

			case vm_inst::OPT_iDIV:
			{
				int a = POP().Int;
				int b = POP().Int;
				PUSH(b / a);
			}
			break;


			case vm_inst::OPT_dADD:
			{
				double a = POP().Double;
				double b = POP().Double;
				PUSH(b + a);
			}
				break;

			case vm_inst::OPT_dSUB:
			{
				double a = POP().Double;
				double b = POP().Double;
				PUSH(b - a);
			}
				break;

			case vm_inst::OPT_dMUL:
			{
				double a = POP().Double;
				double b = POP().Double;
				double c = a * b;
				PUSH(c);
			}
				break;

			case vm_inst::OPT_dDIV:
			{
				double a = POP().Double;
				double b = POP().Double;
				PUSH(b / a);
			}
				break;

			case vm_inst::OPT_EQ:
				PUSH(TO_INT(POP().Bool == POP().Bool));
				break;

			case vm_inst::OPT_LT:
				PUSH(POP().Int > POP().Int ? 1 : 0);
				break;

			case vm_inst::OPT_LTE:
				PUSH(POP().Int >= POP().Int ? 1 : 0);
				break;

			case vm_inst::OPT_GT:
				PUSH(POP().Int < POP().Int ? 1 : 0);
				break;

			case vm_inst::OPT_GTE:
				PUSH(POP().Int <= POP().Int ? 1 : 0);
				break;

			case vm_inst::OPT_AND:
				PUSH(TO_INT(TO_BOOL(POP().Int) && TO_BOOL(POP().Int)));
				break;

			case vm_inst::OPT_OR:
				PUSH(TO_INT(TO_BOOL(POP().Int) || TO_BOOL(POP().Int)));
				break;

			case vm_inst::OPT_DUP:
				PUSH(POP().Int);
				break;

			case vm_inst::OPT_JMP:
			{
				vm_int_t integer = {.Int = 0};
				integer.Chars[1] = *++code;
				integer.Chars[0] = *++code;

				code = startPoint + (integer.Int + 1);
			}
				break;

			case vm_inst::OPT_JIF:
			{
				if (TO_BOOL(POP().Int))
					++code;
				else
				{
					vm_int_t integer = {.Int = 0};
					integer.Chars[1] = *++code;
					integer.Chars[0] = *++code;

					code = startPoint + (integer.Int + 1);
				}
			}
			break;

			case vm_inst::OPT_IF_EQ:
			{
				if (TO_BOOL(POP().Int == POP().Int))
					++code;
				else
				{
					vm_int_t integer = {.Int = 0};
					integer.Chars[1] = *++code;
					integer.Chars[0] = *++code;

					code = startPoint + (integer.Int + 1);
				}
			}
			break;

			case vm_inst::OPT_JNIF:
			{
				if (!TO_BOOL(POP().Int))
				{
					vm_int_t integer = {.Int = 0};
					integer.Chars[1] = *++code;
					integer.Chars[0] = *++code;

					code = startPoint + (integer.Int + 1);
				}
				else
					++code;
			}
			break;

			case vm_inst::OPT_INC:
				SET(PEEK().Int + 1);
				break;
			
			case vm_inst::OPT_NEG:
				SET(PEEK().Int * -1);
				break;

			case vm_inst::OPT_DINC:
				SET(PEEK().Int - 1);
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
				STORE(*++code, POP().Int);
				break;

			case vm_inst::OPT_STORE_0:
				STORE(0, POP().Int);
				break;

			case vm_inst::OPT_STORE_1:
				STORE(1, POP().Int);
				break;

			case vm_inst::OPT_STORE_2:
				STORE(2, POP().Int);
				break;

			case vm_inst::OPT_STORE_3:
				STORE(3, POP().Int);
				break;

			case vm_inst::OPT_STORE_4:
				STORE(4, POP().Int);
				break;


			case vm_inst::OPT_GLOAD:
				PUSH(GLOAD(*++code));
				break;

			case vm_inst::OPT_GLOAD_0:
				PUSH(GLOAD(0));
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
				GSTORE(*++code, POP().Int);
				break;

			case vm_inst::OPT_GSTORE_0:
				GSTORE(0, POP().Int);
				break;

			case vm_inst::OPT_GSTORE_1:
				GSTORE(1, POP().Int);
				break;

			case vm_inst::OPT_GSTORE_2:
				GSTORE(2, POP().Int);
				break;

			case vm_inst::OPT_GSTORE_3:
				GSTORE(3, POP().Int);
				break;

			case vm_inst::OPT_GSTORE_4:
				GSTORE(4, POP().Int);
				break;

			case vm_inst::OPT_CALL:
			{
				currentStore = stores[++storesCount];
				currentStore->startAddress = (code - startPoint) + 1;
                vm_int_t integer = {.Int = 0};
                integer.Chars[1] = *++code;
                integer.Chars[0] = *++code;

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
				POP().Int;
				break;

			case vm_inst::OPT_iPUSH: {
				vm_int_t integer = {.Int = 0};
				integer.Chars[1] = *++code;
				integer.Chars[0] = *++code;
				PUSH(integer.Int);
			}
			break;

			case vm_inst::OPT_dPUSH: {
				vm_double_t d = {.Double = 0};
				d.Chars[7] = *++code;
				d.Chars[6] = *++code;
				d.Chars[5] = *++code;
				d.Chars[4] = *++code;
				d.Chars[3] = *++code;
				d.Chars[2] = *++code;
				d.Chars[1] = *++code;
				d.Chars[0] = *++code;
				PUSH(d.Double);
			}
				break;


			case vm_inst::OPT_bPUSH:
				PUSH((bool)*++code);
				break;

			case vm_inst::OPT_PRINT:
				console_out << POP().Int;
				break;

			case vm_inst::OPT_HALT:
				return;

			case OPT_lADD:break;
			case OPT_lSUB:break;
			case OPT_lMUL:break;
			case OPT_lDIV:break;


			case OPT_I2D:
			{
				int a = POP().Int;
				double b = static_cast<double>(a);
				PUSH(b);
			}
                break;

            case OPT_D2I:
                PUSH(static_cast<int>(POP().Double));
                break;

            case OPT_I2B:
                PUSH(POP().Int == 0.0 ? false : true);
                break;

            case OPT_B2I:
                PUSH(POP().Bool ? 1 : 0);
                break;

            case OPT_D2B:
                PUSH(POP().Double == 0.0 ? false : true);
                break;

            case OPT_B2D:
                PUSH(POP().Bool ? 1.0 : 0.0);
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
                    vm_int_t integer = {.Int = 0};
                    integer.Chars[1] = *++code;
                    integer.Chars[0] = *++code;
                    console_out << _T(" ") << integer.Int;
                    index += 2;
                }
                    break;

                case vm_inst ::OPT_dPUSH:
                {
                    vm_double_t d = {.Double = 0};
                    d.Chars[7] = *++code;
                    d.Chars[6] = *++code;
                    d.Chars[5] = *++code;
                    d.Chars[4] = *++code;
                    d.Chars[3] = *++code;
                    d.Chars[2] = *++code;
                    d.Chars[1] = *++code;
                    d.Chars[0] = *++code;

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