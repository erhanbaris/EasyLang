#include "Vm.h"
#define BYTES_TO_INT(one,two) (one + ( 256 * two ))

#define STORE(index, obj) *(currentStore->variables + index) = obj
#define LOAD(index) (*currentStore->variables + index)


#define PEEK() *(currentStack.Data - 1)
#define POP() (*(--currentStack.Data))
#define PUSH(obj) (*(currentStack.Data++)) = obj
#define SET(obj) (*(currentStack.Data - 1)) = obj

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
		size_t Int;
		double Double;
		void * Ptr;
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
		// delete[] variables;
	}

	size_t startAddress;
	T* variables;
};

template <typename T>
class vm_stack
{
public:
	void init(size_t max)
	{
		Max = max;
		Data = new T[max];
        StartData = Data;

        for (size_t i = 0; i < max; ++i)
            Data[i] = T();
	}

	inline T Pop()
	{
		return *Data;
	}

	~vm_stack()
	{
		// delete[] Data;
	}

    T* StartData;
	T* Data;
	size_t Max;
};

class vm_system_impl
{
public:
	vm_system_impl()
	{
		const int STORE_SIZE = 1024;
		currentStack.init(1024 * 512);
		currentStore = new vm_store<size_t>;
		stores = new vm_store<size_t>*[STORE_SIZE];
		
		for (size_t i = 0; i < STORE_SIZE; ++i)
			stores[i] = new vm_store<size_t>;

		stores[0] = currentStore;
		storesCount = 0;
	}

	~vm_system_impl()
	{
		for (int i = 0; i < 1024; ++i)
			delete stores[i];

		delete[] stores;
	}

	size_t storesCount;
	vm_store<size_t>** stores;
	vm_store<size_t>* currentStore{ nullptr };
	vm_stack<size_t> currentStack;

	void execute(size_t* code, size_t len)
	{
		size_t i = 0;
		bool isHalted = false;

		while (1) {
			switch (code[i])
			{
			case vm_inst::iADD:
				PUSH(POP() + POP());
			break;

			case vm_inst::iSUB:
				PUSH(-POP() + POP());
			break;

			case vm_inst::iMUL:
				PUSH(POP() * POP());
			break;

			case vm_inst::iDIV:
			{
				size_t a = POP();
				size_t b = POP();
				PUSH(b / a);
			}
			break;

			case vm_inst::iEQ:
				PUSH(TO_INT(POP() == POP()));
			break;

			case vm_inst::iLT:
				PUSH(POP() > POP() ? 1 : 0);
			break;

			case vm_inst::iLTE:
				PUSH(POP() >= POP() ? 1 : 0);
			break;

			case vm_inst::iGT:
				PUSH(POP() < POP() ? 1 : 0);
			break;

			case vm_inst::iGTE:
				PUSH(POP() <= POP() ? 1 : 0);
			break;

			case vm_inst::iAND:
				PUSH(TO_INT(TO_BOOL(POP()) && TO_BOOL(POP())));
			break;

			case vm_inst::iOR:
				PUSH(TO_INT(TO_BOOL(POP()) || TO_BOOL(POP())));
			break;

			case vm_inst::iDUP:
				PUSH(POP());
				break;

			case vm_inst::iJMP:
			{
				size_t popValue = code[i + 1];
				i = popValue - 1;
			}
			break;

			case vm_inst::iJIF:
			{
				if (TO_BOOL(POP()))
					++i;
				else
					i = code[i + 1] - 1;
			}
			break;

			case vm_inst::iIF_EQ:
			{
				if (TO_BOOL(POP() == POP()))
					++i;
				else
					i = code[i + 1] - 1;
			}
			break;
			
			case vm_inst::iJNIF:
			{
				if (!TO_BOOL(POP()))
					i = code[i + 1] - 1;
				else
					++i;
			}
			break;

			case vm_inst::iINC:
				SET(PEEK() + 1);
				break;

			case vm_inst::iDINC:
				SET(PEEK() - 1);
				break;

			case vm_inst::iLOAD:
				PUSH(LOAD(code[++i]));
				break;

			case vm_inst::iLOAD_0:
				PUSH(LOAD(0));
				break;

			case vm_inst::iLOAD_1:
				PUSH(LOAD(1));
				break;

			case vm_inst::iLOAD_2:
				PUSH(LOAD(2));
				break;

			case vm_inst::iLOAD_3:
				PUSH(LOAD(3));
				break;

			case vm_inst::iLOAD_4:
				PUSH(LOAD(4));
				break;

			case vm_inst::iSTORE:
				STORE(code[++i], PEEK());
				break;

			case vm_inst::iSTORE_0:
				STORE(0, PEEK());
				break;

			case vm_inst::iSTORE_1:
				STORE(1, PEEK());
				break;

			case vm_inst::iSTORE_2:
				STORE(2, PEEK());
				break;

			case vm_inst::iSTORE_3:
				STORE(3, PEEK());
				break;

			case vm_inst::iSTORE_4:
				STORE(4, PEEK());
				break;

			case vm_inst::iCALL:
			{
				currentStore = stores[++storesCount];
				currentStore->startAddress = i + 1;
				i = code[i + 1] - 1;
			}
			break;

			case vm_inst::iRETURN:
			{
				i = currentStore->startAddress;
				currentStore = stores[--storesCount];
			}
			break;

			case vm_inst::iPOP:
				POP();
				break;

			case vm_inst::iPUSH:
				PUSH(code[++i]);
				break;

			case vm_inst::iPRINT:
				console_out << POP();
				break;

			case vm_inst::iHALT:
				--currentStack.Data;
				return;
			}

			++i;
		}
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

void vm_system::execute(size_t* code, size_t len)
{
	impl->execute(code, len);
}

size_t vm_system::getUInt()
{
	return impl->currentStack.Pop();
}