#include "Vm.h"
#define BYTES_TO_INT(one,two) (one + ( 256 * two ))

#define STORE(index, obj) *(currentStore->variables + index) = obj
#define LOAD(index) (*currentStore->variables + index)

#define GSTORE(index, obj) *(globalStore.variables + index) = obj
#define GLOAD(index) (*globalStore.variables + index)


#define PEEK() *(currentStack - 1)
#define POP() (*(--currentStack))
#define PUSH(obj) (*(currentStack++)) = obj
#define SET(obj) (*(currentStack - 1)) = obj

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
		delete[] variables;
	}

	size_t startAddress;
	T* variables;
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
    vm_store<size_t> globalStore;

    size_t* currentStack;

	void execute(size_t* code, size_t len)
	{
        size_t* startPoint = code;
		while (1) {
			switch (*code)
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
					code = startPoint + (*++code - 1);
				break;

				case vm_inst::iJIF:
				{
					if (TO_BOOL(POP()))
						++code;
					else
						code = startPoint + (*++code - 1);
				}
				break;

				case vm_inst::iIF_EQ:
				{
					if (TO_BOOL(POP() == POP()))
						++code;
					else
						code = startPoint + (*++code - 1);
				}
				break;

				case vm_inst::iJNIF:
				{
					if (!TO_BOOL(POP()))
						code = startPoint + (*++code - 1);
					else
						++code;
				}
				break;

				case vm_inst::iINC:
					SET(PEEK() + 1);
					break;

				case vm_inst::iDINC:
					SET(PEEK() - 1);
					break;

				case vm_inst::iLOAD:
					PUSH(LOAD(*++code));
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
					STORE(*++code, PEEK());
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
                    
                    
                case vm_inst::iGLOAD:
                    PUSH(LOAD(*++code));
                    break;
                    
                case vm_inst::iGLOAD_0:
                    PUSH(LOAD(0));
                    break;
                    
                case vm_inst::iGLOAD_1:
                    PUSH(LOAD(1));
                    break;
                    
                case vm_inst::iGLOAD_2:
                    PUSH(LOAD(2));
                    break;
                    
                case vm_inst::iGLOAD_3:
                    PUSH(LOAD(3));
                    break;
                    
                case vm_inst::iGLOAD_4:
                    PUSH(LOAD(4));
                    break;
                    
                case vm_inst::iGSTORE:
                    GSTORE(*++code, PEEK());
                    break;
                    
                case vm_inst::iGSTORE_0:
                    GSTORE(0, PEEK());
                    break;
                    
                case vm_inst::iGSTORE_1:
                    GSTORE(1, PEEK());
                    break;
                    
                case vm_inst::iGSTORE_2:
                    GSTORE(2, PEEK());
                    break;
                    
                case vm_inst::iGSTORE_3:
                    GSTORE(3, PEEK());
                    break;
                    
                case vm_inst::iGSTORE_4:
                    GSTORE(4, PEEK());
                    break;

				case vm_inst::iCALL:
				{
					currentStore = stores[++storesCount];
					currentStore->startAddress = (code - startPoint) + 1;
					code = startPoint + (*++code - 1);
				}
				break;

				case vm_inst::iRETURN:
				{
					code = startPoint + currentStore->startAddress;
					currentStore = stores[--storesCount];
				}
				break;

				case vm_inst::iPOP:
					POP();
					break;

				case vm_inst::iPUSH:
					PUSH(*++code);
					break;

				case vm_inst::iPRINT:
					console_out << POP();
					break;

				case vm_inst::iHALT:
					--currentStack;
					return;
			}

			++code;
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
	return *impl->currentStack;
}
