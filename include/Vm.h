#ifndef EASYLANG_VM_H
#define EASYLANG_VM_H

#include <stdio.h>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include "Definitions.h"
#include "Macros.h"

class VmException : public std::runtime_error
{
public:
    VmException(string_type const & message) : std::runtime_error(message.c_str())
    {
    }
};

typedef struct vm_object_s
{
    enum class vm_object_type {

    };

    vm_object_type Type;

    union {
        size_t Int;
        double Double;
        void * Ptr;
    };
} vm_object_t;

template <typename T>
class vm_store
{
public:
    vm_store(size_t startAddress)
    {
        StartAddress = startAddress;
    }

    T GetVariable(size_t index)
    {
        return Variables[index];
    }

    void SetVariable(size_t index, T obj)
    {
        Variables[index] = obj;
    }

    size_t GetStartAddress()
    {
        return StartAddress;
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

    void Push(T obj)
    {
        if (Current + 1 >= Max)
            throw VmException(_T("Stack overflow"));

        Data[Current++] = obj;
    }

    void Set(T obj)
    {
        Data[Current - 1] = obj;
    }

    T Pop()
    {
        if (Current - 1 < 0)
            throw VmException(_T("Read -1 stack address"));

        return Data[--Current];
    }

    T Peek()
    {
        return Data[Current - 1];
    }

private:
    size_t Current;
    T* Data;
    size_t Max;
};


DECLARE_ENUM(vm_inst,
             iADD = 1,
             iSUB,
             iMUL,
             iDIV,

             iLT,
             iLTE,
             iGT,
             iGTE,
             iAND,
             iOR,
             iDUP, // dublicate
             iPOP,
             iJMP, // jump
             iJIF, // if jump
             iJNIF, // if not jump
             iINC,
             iDINC,
             iLOAD, // Load variable
             iSTORE, // Store variable

             iCALL,
             iRETURN,

             iPUSH,
             iPRINT,
             iHALT)


class vm_system
{
public:
    vm_system()
    {
        currentStack = new vm_stack<size_t>(1024);
        currentStore = new vm_store<size_t>(0);
        stores.push_back(currentStore);
    }

    void execute(std::vector<size_t> code)
    {
        for (size_t i = 0; i < code.size(); ++i) {
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
                        i = code[i + 1] - 1;
                    else
                        ++i;
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

                case vm_inst::iSTORE:
                    currentStore->SetVariable(code[++i], currentStack->Pop());
                    break;

                case vm_inst::iCALL:
                {
                    currentStore = new vm_store<size_t>(i + 1);
                    stores.push_back(currentStore);
                    i = code[i + 1] - 1;
                }
                    break;

                case vm_inst::iRETURN:
                {
                    stores.erase(stores.begin() + (stores.size() - 1));
                    i = currentStore->GetStartAddress();
                    currentStore = stores[stores.size() - 1];
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
                    i = code.size();
                    break;
            }
        }
    }

private:
    std::vector<vm_store<size_t>*> stores;
    vm_store<size_t>* currentStore {nullptr};
    vm_stack<size_t>* currentStack {nullptr};

    bool toBool(size_t code)
    {
        return code != 0;
    }

    size_t toInt(bool code)
    {
        return code ? 1 : 0;
    }
};


#endif //EASYLANG_VM_H
