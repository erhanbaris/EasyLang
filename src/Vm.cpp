#include "Vm.h"


#define BYTES_TO_INT(one,two) (one + ( 256 * two ))

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
	vm_store(size_t startAddress)
	{
		StartAddress = startAddress;
		Variables = new T[64];
	}

	inline T GetVariable(size_t index)
	{
		return Variables[index];
	}

	inline void SetVariable(size_t index, T obj)
	{
		Variables[index] = obj;
	}

	inline size_t GetStartAddress()
	{
		return StartAddress;
	}

	~vm_store()
	{
		delete Variables;
	}

private:
	size_t StartAddress;
	
	T* Variables;
};

template <typename T>
class vm_stack
{
public:
	vm_stack(size_t max)
	{
		Max = max;
		Current = 0;
		Data = new T[max];
	}

	inline void Push(T obj)
	{
		if (Current + 1 >= Max)
			throw VmException(_T("Stack overflow"));

		Data[Current++] = obj;
	}

	inline void Set(T obj)
	{
		Data[Current - 1] = obj;
	}

	inline T Pop()
	{
		if (Current - 1 < 0)
			throw VmException(_T("Read -1 stack address"));

		return Data[--Current];
	}

	inline T Peek()
	{
		return Data[Current - 1];
	}

private:
	size_t Current;
	T* Data;
	size_t Max;
};

class vm_system_impl
{
public:
	vm_system_impl()
	{
		currentStack = new vm_stack<size_t>(1024 * 512);
		currentStore = new vm_store<size_t>(0);
		stores = new vm_store<size_t>*[1024];
		stores[0] = currentStore;
		storesCount = 0;
	}

	size_t storesCount;
	vm_store<size_t>** stores;
	vm_store<size_t>* currentStore{ nullptr };
	vm_stack<size_t>* currentStack{ nullptr };

	inline bool toBool(size_t code)
	{
		return code != 0;
	}

	inline size_t toInt(bool code)
	{
		return code ? 1 : 0;
	}

	void execute(size_t* code, size_t len)
	{
		size_t i = 0;
		bool isHalted = false;

		while (!isHalted || len < i) {
			switch (code[i])
			{
			case vm_inst::iADD:
			{
				size_t a = currentStack->Pop();
				size_t b = currentStack->Pop();
				currentStack->Push(a + b);
			}
			break;

			case vm_inst::iSUB:
			{
				size_t a = currentStack->Pop();
				size_t b = currentStack->Pop();
				currentStack->Push(b - a);
			}
			break;

			case vm_inst::iMUL:
			{
				size_t a = currentStack->Pop();
				size_t b = currentStack->Pop();
				currentStack->Push(b * a);
			}
			break;

			case vm_inst::iDIV:
			{
				size_t a = currentStack->Pop();
				size_t b = currentStack->Pop();
				currentStack->Push(b / a);
			}
			break;

			case vm_inst::iEQ:
			{
				size_t a = currentStack->Pop();
				size_t b = currentStack->Pop();
				currentStack->Push(toInt(a == b));
			}

			break;
			case vm_inst::iLT:
			{
				size_t a = currentStack->Pop();
				size_t b = currentStack->Pop();
				currentStack->Push(b < a ? 1 : 0);
			}
			break;

			case vm_inst::iLTE:
			{
				size_t a = currentStack->Pop();
				size_t b = currentStack->Pop();
				currentStack->Push(b <= a ? 1 : 0);
			}
			break;

			case vm_inst::iGT:
			{
				size_t a = currentStack->Pop();
				size_t b = currentStack->Pop();
				currentStack->Push(b > a ? 1 : 0);
			}
			break;

			case vm_inst::iGTE:
			{
				size_t a = currentStack->Pop();
				size_t b = currentStack->Pop();
				currentStack->Push(b >= a ? 1 : 0);
			}
			break;

			case vm_inst::iAND:
			{
				size_t a = currentStack->Pop();
				size_t b = currentStack->Pop();
				currentStack->Push(toInt(toBool(a) && toBool(b)));
			}
			break;

			case vm_inst::iOR:
			{
				size_t a = currentStack->Pop();
				size_t b = currentStack->Pop();
				currentStack->Push(toInt(toBool(a) || toBool(b)));
			}
			break;

			case vm_inst::iDUP:
				currentStack->Push(currentStack->Pop());
				break;

			case vm_inst::iJMP:
			{
				size_t popValue = code[i + 1];
				i = popValue - 1;
			}
			break;

			case vm_inst::iJIF:
			{
				if (toBool(currentStack->Pop()))
					++i;
				else
					i = code[i + 1] - 1;
			}
			break;

			case vm_inst::iJNIF:
			{
				if (!toBool(currentStack->Pop()))
					i = code[i + 1] - 1;
				else
					++i;
			}
			break;

			case vm_inst::iINC:
				currentStack->Set(currentStack->Peek() + 1);
				break;

			case vm_inst::iDINC:
				currentStack->Set(currentStack->Peek() - 1);
				break;

			case vm_inst::iLOAD:
				currentStack->Push(currentStore->GetVariable(code[++i]));
				break;

			case vm_inst::iLOAD_0:
				currentStack->Push(currentStore->GetVariable(0));
				break;

			case vm_inst::iLOAD_1:
				currentStack->Push(currentStore->GetVariable(1));
				break;

			case vm_inst::iLOAD_2:
				currentStack->Push(currentStore->GetVariable(2));
				break;

			case vm_inst::iLOAD_3:
				currentStack->Push(currentStore->GetVariable(3));
				break;

			case vm_inst::iLOAD_4:
				currentStack->Push(currentStore->GetVariable(4));
				break;

			case vm_inst::iSTORE:
				currentStore->SetVariable(code[++i], currentStack->Pop());
				break;

			case vm_inst::iSTORE_0:
				currentStore->SetVariable(0, currentStack->Pop());
				break;

			case vm_inst::iSTORE_1:
				currentStore->SetVariable(1, currentStack->Pop());
				break;

			case vm_inst::iSTORE_2:
				currentStore->SetVariable(2, currentStack->Pop());
				break;

			case vm_inst::iSTORE_3:
				currentStore->SetVariable(3, currentStack->Pop());
				break;

			case vm_inst::iSTORE_4:
				currentStore->SetVariable(4, currentStack->Pop());
				break;

			case vm_inst::iCALL:
			{
				currentStore = new vm_store<size_t>(i + 1);
				stores[++storesCount] = currentStore;
				i = code[i + 1] - 1;
			}
			break;

			case vm_inst::iRETURN:
			{
				i = currentStore->GetStartAddress();
				delete stores[storesCount];
				stores[storesCount] = nullptr;
				currentStore = stores[--storesCount];
			}
			break;

			case vm_inst::iPOP:
				currentStack->Pop();
				break;

			case vm_inst::iPUSH:
				currentStack->Push(code[++i]);
				break;

			case vm_inst::iPRINT:
				console_out << currentStack->Pop();
				break;

			case vm_inst::iHALT:
				isHalted = true;
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
	return impl->currentStack->Pop();
}