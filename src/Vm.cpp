#include "Vm.h"


#define BYTES_TO_INT(one,two) (one + ( 256 * two ))

#define STORE(index, obj) currentStore->variables[index] = obj
#define LOAD(index) currentStore->variables[index]


#define PEEK() currentStack.Data[currentStack.Current - 1]
#define POP() currentStack.Data[--currentStack.Current]
#define PUSH(obj)\
if (currentStack.Current + 1 >= currentStack.Max)\
{\
	/*increase stack*/\
}\
currentStack.Data[currentStack.Current++] = obj;
#define SET(obj) currentStack.Data[currentStack.Current - 1] = obj

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

	inline T GetVariable(size_t const & index)
	{
		return variables[index];
	}

	inline void SetVariable(size_t const & index, T const & obj)
	{
		variables[index] = obj;
	}

	inline void SetStartAddress(size_t const & StartAddress)
	{
		startAddress = StartAddress;
	}

	inline size_t GetStartAddress()
	{
		return startAddress;
	}

	inline void Clear()
	{
		//delete[] variables;
		//variables = new T[64];
	}

	~vm_store()
	{
		delete[] variables;
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
		Current = 0;
		Data = new T[max];
	}

	inline void Push(T const & obj)
	{
		if (Current + 1 >= Max)
		{
			// increase stack
		}

		Data[Current++] = obj;
	}

	inline void Set(T const & obj)
	{
		Data[Current - 1] = obj;
	}

	inline T Pop()
	{
		return Data[--Current];
	}

	inline T Peek()
	{
		return Data[Current - 1];
	}

	~vm_stack()
	{
		delete[] Data;
	}

	size_t Current;
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
				STORE(code[++i], POP());
				break;

			case vm_inst::iSTORE_0:
				STORE(0, POP());
				break;

			case vm_inst::iSTORE_1:
				STORE(1, POP());
				break;

			case vm_inst::iSTORE_2:
				STORE(2, POP());
				break;

			case vm_inst::iSTORE_3:
				STORE(3, POP());
				break;

			case vm_inst::iSTORE_4:
				STORE(4, POP());
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
				return;
				break;
			}

			++i;
		}
	}
};

vm_system::vm_system()
{
	this->impl = new vm_system_impl();
}

void vm_system::execute(size_t* code, size_t len)
{
	impl->execute(code, len);
}

size_t vm_system::getUInt()
{
	return impl->currentStack.Pop();
}