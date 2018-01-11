#include "Vm.h"
#include "Console.h"

#define BYTES_TO_INT(one,two) (one + ( 256 * two ))

#define STORE(index, obj) *(currentStore->variables + index) = obj
#define LOAD(index) *(currentStore->variables + index)

#define GSTORE(index, obj) *(globalStore.variables + index) = obj
#define GLOAD(index) *(globalStore.variables + index)

#if _DEBUG
#define PEEK() (int)currentStack[stackIndex - 1]
#define POP() (int)currentStack[--stackIndex]
#define PUSH(obj) currentStack[stackIndex++] = obj;
#define SET(obj) currentStack[stackIndex - 1] = obj

#define iPEEK() (( PEEK() << 8) | (PEEK() & 0x00FF))
#define iPOP() (( POP() << 8) | (POP() & 0x00FF))
#define iPUSH(obj1, obj2) *currentStack = obj1; ++currentStack; *currentStack = obj2; ++currentStack;
#define iSET(obj) (*(currentStack - 1)) = obj

#else 
#define PEEK() *(currentStack - 1)
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

class vm_object
{
	enum class vm_object_type {
		INT,
		DOUBLE,
		STR,
		NATIVE_CALL,
		CALL
	};

	vm_object_type Type;

	union {
		bool Bool;
		size_t Int;
		double Double;
		string_type* String;
		VmMethodCallback Method;
	} Data;
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
		currentStack = new size_t[1024 * 512];
		currentStore = new vm_store<size_t>;
		stores = new vm_store<size_t>*[STORE_SIZE];

		for (size_t i = 0; i < STORE_SIZE; ++i)
			stores[i] = new vm_store<size_t>;

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
	vm_store<size_t>** stores;
	vm_store<size_t>* currentStore{ nullptr };
	vm_store<size_t> globalStore;

	size_t* currentStack;
#if _DEBUG
	size_t stackIndex;
#endif

	void execute(char* code, size_t len, size_t startIndex)
	{
		char* startPoint = code;
		code += startIndex;
		while (1) {
			switch (*code)
			{
			case vm_inst::OPT_iADD:
				PUSH(POP() + POP());
				break;

			case vm_inst::OPT_iSUB:
				PUSH(-POP() + POP());
				break;

			case vm_inst::OPT_iMUL:
				PUSH(POP() * POP());
				break;

			case vm_inst::OPT_iDIV:
			{
				size_t a = POP();
				size_t b = POP();
				PUSH(b / a);
			}
			break;

			case vm_inst::OPT_EQ:
				PUSH(TO_INT(POP() == POP()));
				break;

			case vm_inst::OPT_LT:
				PUSH(POP() > POP() ? 1 : 0);
				break;

			case vm_inst::OPT_LTE:
				PUSH(POP() >= POP() ? 1 : 0);
				break;

			case vm_inst::OPT_GT:
				PUSH(POP() < POP() ? 1 : 0);
				break;

			case vm_inst::OPT_GTE:
				PUSH(POP() <= POP() ? 1 : 0);
				break;

			case vm_inst::OPT_AND:
				PUSH(TO_INT(TO_BOOL(POP()) && TO_BOOL(POP())));
				break;

			case vm_inst::OPT_OR:
				PUSH(TO_INT(TO_BOOL(POP()) || TO_BOOL(POP())));
				break;

			case vm_inst::OPT_DUP:
				PUSH(POP());
				break;

			case vm_inst::OPT_JMP:
				code = startPoint + (*++code - 1);
				break;

			case vm_inst::OPT_JIF:
			{
				if (TO_BOOL(POP()))
					++code;
				else
					code = startPoint + (*++code - 1);
			}
			break;

			case vm_inst::OPT_IF_EQ:
			{
				if (TO_BOOL(POP() == POP()))
					++code;
				else
					code = startPoint + (*++code - 1);
			}
			break;

			case vm_inst::OPT_JNIF:
			{
				if (!TO_BOOL(POP()))
					code = startPoint + (*++code - 1);
				else
					++code;
			}
			break;

			case vm_inst::OPT_INC:
				SET(PEEK() + 1);
				break;
			
			case vm_inst::OPT_NEG:
				SET(PEEK() * -1);
				break;

			case vm_inst::OPT_DINC:
				SET(PEEK() - 1);
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
				code = startPoint + (*++code - 1);
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

			case vm_inst::OPT_PUSH: {
				PUSH(*++code);
			}
								 break;

			case vm_inst::OPT_PRINT:
				console_out << POP();
				break;

			case vm_inst::OPT_HALT:
				return;
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

			switch (*code)
			{
				case vm_inst::OPT_JMP:
				case vm_inst::OPT_JIF:
				case vm_inst::OPT_IF_EQ:
				case vm_inst::OPT_JNIF:
				case vm_inst::OPT_LOAD:
				case vm_inst::OPT_STORE:
				case vm_inst::OPT_GLOAD:
				case vm_inst::OPT_GSTORE:
				case vm_inst::OPT_CALL:
				case vm_inst::OPT_PUSH: 
					console_out << _T(" ") << (int)*++code;
					++index;
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
	return impl->currentStack[impl->stackIndex - 1];
#else
	return *impl->currentStack;
#endif
}
