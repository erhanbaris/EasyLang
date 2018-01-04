#include "Vm.h"

class vm_object
{
	enum class vm_object_type {
		INT,
		DOUBLE,
		STR
	};

	vm_object_type Type;

	union {
		size_t Int;
		double Double;
		void * Ptr;
	};
};

template <typename T>
class vm_store
{
public:
	vm_store(size_t startAddress)
	{
		StartAddress = startAddress;
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
		Variables.clear();
	}

private:
	size_t StartAddress;
	std::unordered_map<size_t, T> Variables;
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
		stores.push_back(currentStore);
	}

	std::vector<vm_store<size_t>*> stores;
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
};

vm_system::vm_system()
{
	this->impl = new vm_system_impl();
}

void vm_system::execute(std::vector<size_t> code)
{
	for (size_t i = 0; i < code.size(); ++i) {
		switch (code[i])
		{
		case vm_inst::iADD:
		{
			size_t a = impl->currentStack->Pop();
			size_t b = impl->currentStack->Pop();
			impl->currentStack->Push(a + b);
		}
		break;

		case vm_inst::iSUB:
		{
			size_t a = impl->currentStack->Pop();
			size_t b = impl->currentStack->Pop();
			impl->currentStack->Push(b - a);
		}
		break;

		case vm_inst::iMUL:
		{
			size_t a = impl->currentStack->Pop();
			size_t b = impl->currentStack->Pop();
			impl->currentStack->Push(b * a);
		}
		break;

		case vm_inst::iDIV:
		{
			size_t a = impl->currentStack->Pop();
			size_t b = impl->currentStack->Pop();
			impl->currentStack->Push(b / a);
		}
		break;

		case vm_inst::iEQ:
		{
			size_t a = impl->currentStack->Pop();
			size_t b = impl->currentStack->Pop();
			impl->currentStack->Push(impl->toInt(a == b));
		}

		break;
		case vm_inst::iLT:
		{
			size_t a = impl->currentStack->Pop();
			size_t b = impl->currentStack->Pop();
			impl->currentStack->Push(b < a ? 1 : 0);
		}
		break;

		case vm_inst::iLTE:
		{
			size_t a = impl->currentStack->Pop();
			size_t b = impl->currentStack->Pop();
			impl->currentStack->Push(b <= a ? 1 : 0);
		}
		break;

		case vm_inst::iGT:
		{
			size_t a = impl->currentStack->Pop();
			size_t b = impl->currentStack->Pop();
			impl->currentStack->Push(b > a ? 1 : 0);
		}
		break;

		case vm_inst::iGTE:
		{
			size_t a = impl->currentStack->Pop();
			size_t b = impl->currentStack->Pop();
			impl->currentStack->Push(b >= a ? 1 : 0);
		}
		break;

		case vm_inst::iAND:
		{
			size_t a = impl->currentStack->Pop();
			size_t b = impl->currentStack->Pop();
			impl->currentStack->Push(impl->toInt(impl->toBool(a) && impl->toBool(b)));
		}
		break;

		case vm_inst::iOR:
		{
			size_t a = impl->currentStack->Pop();
			size_t b = impl->currentStack->Pop();
			impl->currentStack->Push(impl->toInt(impl->toBool(a) || impl->toBool(b)));
		}
		break;

		case vm_inst::iDUP:
			impl->currentStack->Push(impl->currentStack->Pop());
			break;

		case vm_inst::iJMP:
		{
			size_t popValue = code[i + 1];
			i = popValue - 1;
		}
		break;

		case vm_inst::iJIF:
		{
			if (impl->toBool(impl->currentStack->Pop()))
				++i;
			else
				i = code[i + 1] - 1;
		}
		break;

		case vm_inst::iJNIF:
		{
			if (!impl->toBool(impl->currentStack->Pop()))
				i = code[i + 1] - 1;
			else
				++i;
		}
		break;

		case vm_inst::iINC:
			impl->currentStack->Set(impl->currentStack->Peek() + 1);
			break;

		case vm_inst::iDINC:
			impl->currentStack->Set(impl->currentStack->Peek() - 1);
			break;

		case vm_inst::iLOAD:
			impl->currentStack->Push(impl->currentStore->GetVariable(code[++i]));
			break;

		case vm_inst::iSTORE:
			impl->currentStore->SetVariable(code[++i], impl->currentStack->Pop());
			break;

		case vm_inst::iCALL:
		{
			impl->currentStore = new vm_store<size_t>(i + 1);
			impl->stores.push_back(impl->currentStore);
			i = code[i + 1] - 1;
		}
		break;

		case vm_inst::iRETURN:
		{
			impl->stores.erase(impl->stores.begin() + (impl->stores.size() - 1));
			i = impl->currentStore->GetStartAddress();
			delete impl->currentStore;
			impl->currentStore = impl->stores[impl->stores.size() - 1];
		}
		break;

		case vm_inst::iPOP:
			impl->currentStack->Pop();
			break;

		case vm_inst::iPUSH:
			impl->currentStack->Push(code[++i]);
			break;

		case vm_inst::iPRINT:
			console_out << impl->currentStack->Pop();
			break;

		case vm_inst::iHALT:
			i = code.size();
			break;
		}
	}
}

size_t vm_system::getUInt()
{
	return impl->currentStack->Pop();
}