#ifndef EASYLANG_VM_H
#define EASYLANG_VM_H

#include <stdio.h>
#include <vector>
#include <stdexcept>
#include "Definitions.h"
#include "Macros.h"

class VmException : public std::runtime_error
{
public:
    VmException(string_type const & message) : std::runtime_error(AS_CHAR(message.c_str()))
    {
    }
};

typedef uint64_t Value;

class vm_gc;
class vm_array;
class vm_object;
class vm_system;
class vm_system_impl;
template <typename T> class vm_store;
template <typename T> class vm_stack;

typedef Value(*VmMethod)(vm_system* vm);
typedef void(*VmMethodCallback)(vm_system* vm, size_t totalArgs);
typedef char_type vm_char_t;
typedef union vm_double_u { vm_char_t Chars[8];  double Double; } vm_double_t;
typedef union vm_long_u { vm_char_t Chars[8];  long Long; } vm_long_t;
typedef union vm_int_u { vm_char_t Chars[4];  int Int; } vm_int_t;
typedef bool vm_bool_t;


// A mask that selects the sign bit.
#define SIGN_BIT ((uint64_t)1 << 63)

// The bits that must be set to indicate a quiet NaN.
#define QNAN ((uint64_t)0x7ffc000000000000)

// If the NaN bits are set, it's not a number.
#define IS_NUM(value) (((value) & QNAN) != QNAN)

// An object pointer is a NaN with a set sign bit.
#define IS_OBJ(value) (((value) & (QNAN | SIGN_BIT)) == (QNAN | SIGN_BIT))

#define IS_STRING(value) (IS_OBJ(value) && AS_OBJ(value)->Type == vm_object::vm_object_type::STR)
#define IS_ARRAY(value) (IS_OBJ(value) && AS_OBJ(value)->Type == vm_object::vm_object_type::ARRAY)

#define IS_FALSE(value)     ((value) == FALSE_VAL)
#define IS_BOOL(value)      (value == TRUE_VAL || value == FALSE_VAL)
#define IS_NULL(value)      ((value) == NULL_VAL)
#define IS_UNDEFINED(value) ((value) == UNDEFINED_VAL)

// Masks out the tag bits used to identify the singleton value.
#define MASK_TAG (7)

// Tag values for the different singleton values.
#define TAG_NAN       (0)
#define TAG_NULL      (1)
#define TAG_FALSE     (2)
#define TAG_TRUE      (3)
#define TAG_UNDEFINED (4)
#define TAG_UNUSED2   (5)
#define TAG_UNUSED3   (6)
#define TAG_UNUSED4   (7)

// Value -> 0 or 1.
#define AS_BOOL(value) ((value) == TRUE_VAL)

// Value -> Obj*.
#define AS_OBJ(value) ((vm_object*)(uintptr_t)((value) & ~(SIGN_BIT | QNAN)))

// Singleton values.
#define NULL_VAL      ((Value)(uint64_t)(QNAN | TAG_NULL))
#define FALSE_VAL     ((Value)(uint64_t)(QNAN | TAG_FALSE))
#define TRUE_VAL      ((Value)(uint64_t)(QNAN | TAG_TRUE))
#define UNDEFINED_VAL ((Value)(uint64_t)(QNAN | TAG_UNDEFINED))

// Gets the singleton type tag for a Value (which must be a singleton).
#define GET_TAG(value) ((int)((value) & MASK_TAG))

#define GET_VALUE_FROM_OBJ(obj) ((Value)(SIGN_BIT | QNAN | (uint64_t)(uintptr_t)(obj)))

typedef union
{
    uint64_t bits64;
    uint32_t bits32[2];
    double num;
} DoubleBits;

static inline bool isBool(Value value)
{
    return value == TRUE_VAL || value == FALSE_VAL;
}

static inline Value numberToValue(double num)
{
    DoubleBits data;
    data.num = num;
    return data.bits64;
}

static inline double valueToNumber(Value num)
{
    DoubleBits data;
    data.bits64 = num;
    return data.num;
}


class vm_gc {
public:
    static vm_object* Create();
    static void Set(vm_object* newObject);
    static void MakeDirty(vm_object* obj);
    static size_t Clean();
    
    static size_t GetTotalItems();
    static size_t GetDeletedItems();
    static size_t GetDirtyItems();

    static class _init
    {
    public:
        _init();
    } _initializer;

private:
    static vm_object* head;
    static vm_object* lastObject;

    static size_t TotalItems;
    static size_t DeletedItems;
    static size_t DirtyItems;
};


class vm_object
{
public:
    enum class vm_object_type : char {
        EMPTY,
        STR,
        ARRAY,
        DICT
    };

    vm_object()
    {
        Type = vm_object_type::EMPTY;
        IsDeleted = false;
        NextObject = nullptr;
        RefCounter = 1;
        vm_gc::Set(this);
    }

    ~vm_object()
    {
        if (Type == vm_object_type::STR)
            delete[] (char_type*)Pointer;
        else if (Type == vm_object_type::ARRAY)
            delete (vm_array*)Pointer;
    }

    vm_object(char_type* b)
    {
        size_t totalLen = strlen(b);
        Pointer = new char_type[totalLen + 1];
        memcpy(Pointer, b, totalLen);
        ((char_type*)Pointer)[totalLen] = 0;

        Type = vm_object_type::STR;
        NextObject = nullptr;
        IsDeleted = false;
        RefCounter = 1;
        vm_gc::Set(this);
    }

    vm_object(string_type const & b)
    {
        size_t totalLen = b.size();
        Pointer = new char_type[totalLen + 1];
        memcpy(Pointer, b.c_str(), totalLen);
        ((char_type*)Pointer)[totalLen] = 0;
        Type = vm_object_type::STR;
        NextObject = nullptr;
        IsDeleted = false;
        RefCounter = 1;
        vm_gc::Set(this);
    }

    vm_object(vm_array const * array)
    {
        Pointer = const_cast<void*>((void*)array);
        Type = vm_object_type::ARRAY;
        NextObject = nullptr;
        IsDeleted = false;
        RefCounter = 1;
        vm_gc::Set(this);
    }

    void MakeDirty()
    {
        vm_gc::MakeDirty(this);
    }

    size_t RefCounter;
    vm_object_type Type;
    bool IsDeleted;
    vm_object* NextObject;

    union {
        void* Pointer{nullptr};
        VmMethodCallback Method;
    };
};

class vm_array{
public:
    mutable size_t Length{16};
    mutable size_t Indicator{0};
    mutable Value* Array {nullptr};


    void push(Value obj)
    {
        Array[Indicator++] = obj;
        if (Indicator == Length)
            resizeTo(Length * 2);
    }

    Value get(size_t index) const
    {
        return Array[index];
    }

    void resizeTo(size_t newSize) const
    {
        auto* tmpNewArray = new vm_object[newSize];
        memcpy(tmpNewArray, Array, Length);
        Length = newSize;
    }

    vm_array()
    {
        Array = new Value[Length];
    }
    
    ~vm_array()
    {
        for (int i = 0; i < Indicator; ++i) {
            if (IS_OBJ((Array[i])))
                delete AS_OBJ(Array[i]);
        }

        delete[] Array;
    }
};

DECLARE_ENUM(vm_inst,
OPT_HALT,    //    0
OPT_ADD,    //    1
OPT_SUB,    //    2
OPT_MUL,    //    3
OPT_DIV,    //    4
OPT_EQ,    //    5
OPT_LT,    //    6
OPT_LTE,    //    7
OPT_GT,    //    8
OPT_GTE,    //    9
OPT_AND,    //    10
OPT_OR,    //    11
OPT_DUP,    //    12
OPT_POP,    //    13
OPT_CONST_STR,    //    14
OPT_CONST_INT,    //    15
OPT_CONST_INT_0,    //    16
OPT_CONST_INT_1,    //    17
OPT_CONST_BOOL_TRUE,    //    18
OPT_CONST_BOOL_FALSE,    //    19
OPT_CONST_DOUBLE,    //    20
OPT_CONST_DOUBLE_0,    //    21
OPT_CONST_DOUBLE_1,    //    22
OPT_DELETE,    //    23
OPT_JMP,    //    24
OPT_IF_EQ,    //    25
OPT_JIF,    //    26
OPT_JNIF,    //    27
OPT_INC,    //    28
OPT_DINC,    //    29
OPT_LOAD,    //    30
OPT_LOAD_0,    //    31
OPT_LOAD_1,    //    32
OPT_LOAD_2,    //    33
OPT_LOAD_3,    //    34
OPT_LOAD_4,    //    35
OPT_LOAD_5,    //    36
OPT_LOAD_6,    //    37
OPT_LOAD_7,    //    38
OPT_LOAD_8,    //    39
OPT_LOAD_9,    //    40
OPT_LOAD_10,    //    41
OPT_LOAD_11,    //    42
OPT_LOAD_12,    //    43
OPT_LOAD_13,    //    44
OPT_LOAD_14,    //    45
OPT_LOAD_15,    //    46
OPT_LOAD_16,    //    47
OPT_STORE,    //    48
OPT_STORE_0,    //    49
OPT_STORE_1,    //    50
OPT_STORE_2,    //    51
OPT_STORE_3,    //    52
OPT_STORE_4,    //    53
OPT_STORE_5,    //    54
OPT_STORE_6,    //    55
OPT_STORE_7,    //    56
OPT_STORE_8,    //    57
OPT_STORE_9,    //    58
OPT_STORE_10,    //    59
OPT_STORE_11,    //    60
OPT_STORE_12,    //    61
OPT_STORE_13,    //    62
OPT_STORE_14,    //    63
OPT_STORE_15,    //    64
OPT_STORE_16,    //    65
OPT_GLOAD,    //    66
OPT_GLOAD_0,    //    67
OPT_GLOAD_1,    //    68
OPT_GLOAD_2,    //    69
OPT_GLOAD_3,    //    70
OPT_GLOAD_4,    //    71
OPT_GLOAD_5,    //    72
OPT_GLOAD_6,    //    73
OPT_GLOAD_7,    //    74
OPT_GLOAD_8,    //    75
OPT_GLOAD_9,    //    76
OPT_GLOAD_10,    //    77
OPT_GLOAD_11,    //    78
OPT_GLOAD_12,    //    79
OPT_GLOAD_13,    //    80
OPT_GLOAD_14,    //    81
OPT_GLOAD_15,    //    82
OPT_GLOAD_16,    //    83
OPT_GSTORE,    //    84
OPT_GSTORE_0,    //    85
OPT_GSTORE_1,    //    86
OPT_GSTORE_2,    //    87
OPT_GSTORE_3,    //    88
OPT_GSTORE_4,    //    89
OPT_GSTORE_5,    //    90
OPT_GSTORE_6,    //    91
OPT_GSTORE_7,    //    92
OPT_GSTORE_8,    //    93
OPT_GSTORE_9,    //    94
OPT_GSTORE_10,    //    95
OPT_GSTORE_11,    //    96
OPT_GSTORE_12,    //    97
OPT_GSTORE_13,    //    98
OPT_GSTORE_14,    //    99
OPT_GSTORE_15,    //    100
OPT_GSTORE_16,    //    101
OPT_CALL,    //    102
OPT_RETURN,    //    103
OPT_PUSH,    //    104
OPT_PRINT,    //    105
OPT_NEG,    //    106
OPT_CALL_NATIVE,    //    107
OPT_METHOD_DEF,    //    108
OPT_INITARRAY,    //    109
OPT_INITDICT,    //    110
OPT_NOT_EQ,    //    111
OPT_INDEX,    //    112
OPT_INITEMPTYARRAY,    //    113
OPT_APPEND,    //    113
OPT_EMPTY    //    114
)

class vm_system
{
public:
    vm_system();
    ~vm_system();
    void execute(char_type* code, size_t len, size_t startIndex);
    void addMethod(string_type const & name, VmMethod method);
    void dumpOpcode(char_type* code, size_t len);
    void dumpStack();
    void dump(char_type* code, size_t len);
    size_t getUInt();
    Value getObject();

private:
    vm_system_impl* impl;
};


#endif //EASYLANG_VM_H
