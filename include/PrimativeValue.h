#ifndef EASYLANG_PRIMATIVEVALUE_H
#define EASYLANG_PRIMATIVEVALUE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <iostream>

struct PrimativeValue {
    enum class Type {
        PRI_INTEGER,
        PRI_DOUBLE,
        PRI_STRING,
        PRI_BOOL,
        PRI_ARRAY,
		PRI_DICTIONARY,
        PRI_NULL
    };

    Type Type;

    union {
        int Integer;
        double Double;
        bool Bool;
        std::wstring* String;
        std::vector<PrimativeValue*>* Array;
        std::unordered_map<std::wstring, PrimativeValue*>* Dictionary;
    };

    PrimativeValue() { Integer = 0; Type = Type::PRI_NULL; }
    PrimativeValue(int value) { Integer = value; Type = Type::PRI_INTEGER; }
    PrimativeValue(double value) { Double = value; Type = Type::PRI_DOUBLE;}
    PrimativeValue(std::wstring const & value) { String = new std::wstring(value); Type = Type::PRI_STRING;}
    PrimativeValue(bool value) { Bool = value; Type = Type::PRI_BOOL;}
    PrimativeValue(std::vector<PrimativeValue*>* value) { Array = value; Type = Type::PRI_ARRAY;}
    PrimativeValue(std::unordered_map<std::wstring, PrimativeValue*>* value) { Dictionary = value; Type = Type::PRI_DICTIONARY;}

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
        case PrimativeValue::Type::PRI_ARRAY:
        {
            std::wstringstream stream;
            stream << L"(ARRAY) ";

            if (Array != nullptr && !Array->empty())
            {
                stream << L"Size: "
                       << Array->size()
                       << '\n';

                for (int i = 0; i < Array->size(); ++i) {
                    stream  << i << L". "
                            << Array->at(i)->Describe()
                            << '\n';
                }
            }
            else
                stream << L"Size: 0";

            return stream.str();
        }
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
    bool IsArray() { return Type == Type::PRI_ARRAY; }
    bool IsDictionary() { return Type == Type::PRI_DICTIONARY; }
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

typedef std::shared_ptr<std::vector<PrimativeValue*> > FunctionArgs;

#endif //EASYLANG_PRIMATIVEVALUE_H
