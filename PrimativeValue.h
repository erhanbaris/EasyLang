#ifndef EASYLANG_PRIMATIVEVALUE_H
#define EASYLANG_PRIMATIVEVALUE_H

#include <string>

struct PrimativeValue {
    enum class Type {
        PRI_INTEGER,
        PRI_DOUBLE,
        PRI_STRING,
        PRI_BOOL,
		PRI_LIST,
		PRI_DICT,
        PRI_NULL
    };

    Type Type;

    union {
        int Integer;
        double Double;
        std::wstring* String;
        bool Bool;
    };

    PrimativeValue() { Integer = 0; Type = Type::PRI_NULL; }
    PrimativeValue(int value) { Integer = value; Type = Type::PRI_INTEGER; }
    PrimativeValue(double value) { Double = value; Type = Type::PRI_DOUBLE;}
    PrimativeValue(std::wstring value) { String = new std::wstring(value); Type = Type::PRI_STRING;}
    PrimativeValue(bool value) { Bool = value; Type = Type::PRI_BOOL;}

    ~PrimativeValue() { }

	std::wstring Describe()
	{
		switch (Type)
		{
		case PrimativeValue::Type::PRI_BOOL:
			return Bool ? L"(BOOL) true" : L"(BOOL) false";
		case PrimativeValue::Type::PRI_DOUBLE:
			return L"(DOUBLE) " + std::to_wstring(Double);
		case PrimativeValue::Type::PRI_INTEGER:
			return L"(INTEGER) " + std::to_wstring(Integer);
		case PrimativeValue::Type::PRI_STRING:
			return L"(STRING) " + *String;			
		}

		return L"(NULL)";
	}

    void SetInteger(int value) { Integer = value; Type = Type::PRI_INTEGER; }
    void SetDouble(double value) { Double = value; Type = Type::PRI_DOUBLE; }
    void SetString(std::wstring value)
    {
        String = new std::wstring(value);
        Type = Type::PRI_STRING;
    }
    void SetBool(bool value) { Bool = value; Type = Type::PRI_BOOL; }
    void SetNull() { Integer = 0; Type = Type::PRI_NULL; }

    bool IsInteger() { return Type == Type::PRI_INTEGER; }
    bool IsDouble() { return Type == Type::PRI_DOUBLE; }
    bool IsString() { return Type == Type::PRI_STRING; }
    bool IsBool() { return Type == Type::PRI_BOOL; }
    bool IsNull() { return Type == Type::PRI_NULL; }

    PrimativeValue & operator=(const PrimativeValue &rhs)
    {
        switch (rhs.Type)
        {
            case PrimativeValue::Type::PRI_BOOL:
                SetBool(rhs.Bool);
                break;

            case PrimativeValue::Type::PRI_DOUBLE:
                SetDouble(rhs.Double);
                break;

            case PrimativeValue::Type::PRI_INTEGER:
                SetInteger(rhs.Integer);
                break;

            case PrimativeValue::Type::PRI_STRING:
                SetString(*rhs.String);
                break;
        }

        return *this;
    }

    PrimativeValue * operator=(const PrimativeValue *rhs)
    {
        if (rhs == nullptr)
            return nullptr;

        switch (rhs->Type)
        {
            case PrimativeValue::Type::PRI_BOOL:
                SetBool(rhs->Bool);
                break;

            case PrimativeValue::Type::PRI_DOUBLE:
                SetDouble(rhs->Double);
                break;

            case PrimativeValue::Type::PRI_INTEGER:
                SetInteger(rhs->Integer);
                break;

            case PrimativeValue::Type::PRI_STRING:
                SetString(*rhs->String);
                break;
        }

        return this;
    }
};

PrimativeValue* operator + (PrimativeValue const & lhs, PrimativeValue const & rhs);
PrimativeValue* operator - (PrimativeValue const & lhs, PrimativeValue const & rhs);
PrimativeValue* operator * (PrimativeValue const & lhs, PrimativeValue const & rhs);
PrimativeValue* operator / (PrimativeValue const & lhs, PrimativeValue const & rhs);

PrimativeValue* operator > (PrimativeValue const & lhs, PrimativeValue const & rhs);
PrimativeValue* operator >= (PrimativeValue const & lhs, PrimativeValue const & rhs);

PrimativeValue* operator < (PrimativeValue const & lhs, PrimativeValue const & rhs);
PrimativeValue* operator <= (PrimativeValue const & lhs, PrimativeValue const & rhs);

PrimativeValue* operator != (PrimativeValue const & lhs, PrimativeValue const & rhs);
PrimativeValue* operator == (PrimativeValue const & lhs, PrimativeValue const & rhs);

PrimativeValue* operator && (PrimativeValue const & lhs, PrimativeValue const & rhs);
PrimativeValue* operator || (PrimativeValue const & lhs, PrimativeValue const & rhs);


#endif //EASYLANG_PRIMATIVEVALUE_H
