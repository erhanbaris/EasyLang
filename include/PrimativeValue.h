#ifndef EASYLANG_PRIMATIVEVALUE_H
#define EASYLANG_PRIMATIVEVALUE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <iostream>
#include <memory>
#include "Definitions.h"

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
		string_type* String;
        std::vector<PrimativeValue*>* Array;
        std::unordered_map<string_type, PrimativeValue*>* Dictionary;
    };

    PrimativeValue() { Integer = 0; Type = Type::PRI_NULL; }
    PrimativeValue(int value) { Integer = value; Type = Type::PRI_INTEGER; }
    PrimativeValue(double value) { Double = value; Type = Type::PRI_DOUBLE;}
    PrimativeValue(string_type const & value) { String = new string_type(value); Type = Type::PRI_STRING;}
    PrimativeValue(bool value) { Bool = value; Type = Type::PRI_BOOL;}
    PrimativeValue(std::vector<PrimativeValue*>* value) { Array = value; Type = Type::PRI_ARRAY;}
    PrimativeValue(std::unordered_map<string_type, PrimativeValue*>* value) { Dictionary = value; Type = Type::PRI_DICTIONARY;}


	static PrimativeValue* CreateArray()
	{
		return new PrimativeValue(new std::vector<PrimativeValue*>());
	}

	static PrimativeValue* CreateArray(size_t length)
	{
		return new PrimativeValue(new std::vector<PrimativeValue*>(length));
	}

	static PrimativeValue* CreateDictionary()
	{
		return new PrimativeValue(new std::unordered_map<string_type, PrimativeValue*>());
	}

	static PrimativeValue* CreateBool()
	{
		return new PrimativeValue(false);
	}

	static PrimativeValue* CreateBool(bool value)
	{
		return new PrimativeValue(value);
	}

	static PrimativeValue* CreateString(string_type const & value)
	{
		return new PrimativeValue(value);
	}

	static PrimativeValue* CreateString()
	{
		return new PrimativeValue(L"");
	}

	static PrimativeValue* CreateDouble(double value)
	{
		return new PrimativeValue(value);
	}

	static PrimativeValue* CreateDouble()
	{
		return new PrimativeValue(0.0);
	}

	static PrimativeValue* CreateInteger(int value)
	{
		return new PrimativeValue(value);
	}

	static PrimativeValue* CreateInteger()
	{
		return new PrimativeValue(0);
	}


    ~PrimativeValue() { }

	string_type Describe()
	{
		switch (Type)
		{
		case PrimativeValue::Type::PRI_BOOL:
			return Bool ? _T("(BOOL) true") : _T("(BOOL) false");
		case PrimativeValue::Type::PRI_DOUBLE:
			return _T("(DOUBLE) ") + AS_STRING(Double);
		case PrimativeValue::Type::PRI_INTEGER:
			return _T("(INTEGER) ") + AS_STRING(Integer);
		case PrimativeValue::Type::PRI_STRING:
			return _T("(STRING) ") + *String;
        case PrimativeValue::Type::PRI_ARRAY:
        {
			string_stream stream;
            stream << _T("(ARRAY) ");

            if (Array != nullptr && !Array->empty())
            {
                stream << _T("Size: ")
                       << Array->size()
                       << '\n';

                for (int i = 0; i < Array->size(); ++i) {
                    stream  << i << _T(". ")
                            << Array->at(i)->Describe()
                            << '\n';
                }
            }
            else
                stream << _T("Size: 0");

            return stream.str();
        }
        }

		return _T("(NULL)");
	}

	PrimativeValue* Clone()
	{
		switch (Type)
		{
			case PrimativeValue::Type::PRI_BOOL:
				return new PrimativeValue(Bool);
			case PrimativeValue::Type::PRI_DOUBLE:
				return new PrimativeValue(Double);
			case PrimativeValue::Type::PRI_INTEGER:
				return new PrimativeValue(Integer);
			case PrimativeValue::Type::PRI_STRING:
				return new PrimativeValue(*String);
			case PrimativeValue::Type::PRI_ARRAY:
			{
				auto* returnValue = new PrimativeValue();
				returnValue->SetArray(new std::vector<PrimativeValue*>());

				if (Array != nullptr && !Array->empty())
				{
					for (int i = 0; i < Array->size(); ++i) {
						returnValue->Array->push_back(Array->at(i)->Clone());
					}
				}

				return returnValue;
			}
		}

		return nullptr;
	}

    void SetInteger(int value) { Integer = value; Type = Type::PRI_INTEGER; }
    void SetDouble(double value) { Double = value; Type = Type::PRI_DOUBLE; }
    void SetString(string_type value)
    {
        String = new string_type(value);
        Type = Type::PRI_STRING;
    }
    void SetBool(bool value) { Bool = value; Type = Type::PRI_BOOL; }
	void SetArray(std::vector<PrimativeValue*>* value) { Array = value; Type = Type::PRI_ARRAY; }
	void SetDictionary(std::unordered_map<string_type, PrimativeValue*>* value) { Dictionary = value; Type = Type::PRI_DICTIONARY; }
    void SetNull() { Integer = 0; Type = Type::PRI_NULL; }

    bool IsInteger() { return Type == Type::PRI_INTEGER; }
    bool IsDouble() { return Type == Type::PRI_DOUBLE; }
    bool IsString() { return Type == Type::PRI_STRING; }
    bool IsBool() { return Type == Type::PRI_BOOL; }
    bool IsArray() { return Type == Type::PRI_ARRAY; }
    bool IsDictionary() { return Type == Type::PRI_DICTIONARY; }
    bool IsNull() { return Type == Type::PRI_NULL; }
    
    void Append(PrimativeValue * rhs)
    {
        switch (this->Type)
        {
            case PrimativeValue::Type::PRI_STRING:
            {
                switch (rhs->Type)
                {
                    case PrimativeValue::Type::PRI_INTEGER:
                        this->String = new string_type(*this->String + AS_STRING(rhs->Integer));
                        break;
                        
                    case PrimativeValue::Type::PRI_DOUBLE:
                        this->String = new string_type(*this->String + AS_STRING(rhs->Double));
                        break;
                        
                    case PrimativeValue::Type::PRI_STRING:
                        this->String = new string_type(*this->String + *rhs->String);
                        break;
                }
            }
                break;
                
            case PrimativeValue::Type::PRI_ARRAY:
                this->Array->push_back(rhs);
                break;
        }
    }

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
