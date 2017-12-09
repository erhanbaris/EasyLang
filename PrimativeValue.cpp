#include "PrimativeValue.h"
#include <iostream>
#include <sstream>

/* + operator */
PrimativeValue* operator + (PrimativeValue const & lhs, PrimativeValue const & rhs)
{
    PrimativeValue* returnValue = new PrimativeValue;

    switch (lhs.Type)
    {
        case PrimativeValue::Type::PRI_BOOL:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(lhs.Bool || rhs.Bool);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(true);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_DOUBLE:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetDouble(lhs.Double + rhs.Double);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetDouble(lhs.Double + rhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetString(std::to_wstring(lhs.Double) + rhs.String);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_INTEGER:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetDouble(lhs.Integer + rhs.Double);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetInteger(lhs.Integer + rhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetString(std::to_wstring(lhs.Integer) + rhs.String);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_STRING:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetString(lhs.String + std::to_wstring(rhs.Integer));
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetString(lhs.String + std::to_wstring(rhs.Double));
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetString(lhs.String + rhs.String);
                    break;
            }
        }
            break;
    }

    return returnValue;
}

/* * operator */
PrimativeValue* operator * (PrimativeValue const & lhs, PrimativeValue const & rhs)
{
    PrimativeValue* returnValue = new PrimativeValue;

    switch (lhs.Type)
    {
        case PrimativeValue::Type::PRI_BOOL:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(lhs.Bool && rhs.Bool);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(true);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_DOUBLE:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetDouble(lhs.Double * rhs.Double);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetDouble(lhs.Double * rhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    std::wstringstream stream;

                    for (double i = 0.0; i < lhs.Double; ++i)
                        stream << rhs.String;

                    returnValue->SetString(stream.str());
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_INTEGER:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetDouble(rhs.Double * lhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetInteger(lhs.Integer * rhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                {std::wstringstream stream;

                    for (size_t i = 0; i < lhs.Integer; ++i)
                        stream << rhs.String;

                    returnValue->SetString(stream.str());
                }
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_STRING:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                {
                    std::wstringstream stream;

                    for (size_t i = 0; i < rhs.Integer; ++i)
                        stream << lhs.String;

                    returnValue->SetString(stream.str());
                }
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                {
                    std::wstringstream stream;

                    for (double i = 0.0; i < rhs.Double; ++i)
                        stream << lhs.String;

                    returnValue->SetString(stream.str());
                }
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetString(lhs.String + rhs.String);
                    break;
            }
        }
            break;
    }

    return returnValue;
}

/* - operator */
PrimativeValue* operator - (PrimativeValue const & lhs, PrimativeValue const & rhs)
{
    PrimativeValue* returnValue = new PrimativeValue;

    switch (lhs.Type)
    {
        case PrimativeValue::Type::PRI_BOOL:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(false);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(false);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(false);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(false);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_DOUBLE:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetDouble(lhs.Double);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetDouble(lhs.Double - rhs.Double);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetDouble(lhs.Double - rhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetDouble(lhs.Double);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_INTEGER:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetInteger(lhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetDouble(lhs.Integer - rhs.Double);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetInteger(lhs.Integer - rhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetInteger(lhs.Integer);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_STRING:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetString(lhs.String);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetString(lhs.String);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetString(lhs.String);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetString(lhs.String);
                    break;
            }
        }
            break;
    }

    return returnValue;
}

/* / operator */
PrimativeValue* operator / (PrimativeValue const & lhs, PrimativeValue const & rhs)
{
    PrimativeValue* returnValue = new PrimativeValue;

    switch (lhs.Type)
    {
        case PrimativeValue::Type::PRI_BOOL:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(false);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(false);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(false);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(false);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_DOUBLE:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetDouble(lhs.Double);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetDouble(lhs.Double / rhs.Double);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetDouble(lhs.Double / rhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetDouble(lhs.Double);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_INTEGER:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetInteger(lhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetDouble(lhs.Integer / rhs.Double);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetInteger(lhs.Integer / rhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetInteger(lhs.Integer);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_STRING:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetString(lhs.String);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetString(lhs.String);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetString(lhs.String);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetString(lhs.String);
                    break;
            }
        }
            break;
    }

    return returnValue;
}

/* > operator */
PrimativeValue* operator > (PrimativeValue const & lhs, PrimativeValue const & rhs)
{
    PrimativeValue* returnValue = new PrimativeValue;

    switch (lhs.Type)
    {
        case PrimativeValue::Type::PRI_BOOL:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(lhs.Bool);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(lhs.Bool);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(lhs.Bool);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(lhs.Bool);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_DOUBLE:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(lhs.Double > rhs.Double);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(lhs.Double > rhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(true);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_INTEGER:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(lhs.Integer > rhs.Double);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(lhs.Integer > rhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(true);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_STRING:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(true);
                    break;
            }
        }
            break;
    }

    return returnValue;
}

/* >= operator */
PrimativeValue* operator >= (PrimativeValue const & lhs, PrimativeValue const & rhs)
{
    PrimativeValue* returnValue = new PrimativeValue;

    switch (lhs.Type)
    {
        case PrimativeValue::Type::PRI_BOOL:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(lhs.Bool);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(lhs.Bool);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(lhs.Bool);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(lhs.Bool);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_DOUBLE:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(lhs.Double >= rhs.Double);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(lhs.Double >= rhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(true);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_INTEGER:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(lhs.Integer >= rhs.Double);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(lhs.Integer >= rhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(true);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_STRING:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(true);
                    break;
            }
        }
            break;
    }

    return returnValue;
}

/* < operator */
PrimativeValue* operator < (PrimativeValue const & lhs, PrimativeValue const & rhs)
{
    PrimativeValue* returnValue = new PrimativeValue;

    switch (lhs.Type)
    {
        case PrimativeValue::Type::PRI_BOOL:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(lhs.Bool);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(lhs.Bool);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(lhs.Bool);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(lhs.Bool);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_DOUBLE:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(lhs.Double < rhs.Double);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(lhs.Double < rhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(true);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_INTEGER:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(lhs.Integer < rhs.Double);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(lhs.Integer < rhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(true);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_STRING:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(true);
                    break;
            }
        }
            break;
    }

    return returnValue;
}

/* <= operator */
PrimativeValue* operator <= (PrimativeValue const & lhs, PrimativeValue const & rhs)
{
    PrimativeValue* returnValue = new PrimativeValue;

    switch (lhs.Type)
    {
        case PrimativeValue::Type::PRI_BOOL:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(lhs.Bool);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(lhs.Bool);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(lhs.Bool);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(lhs.Bool);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_DOUBLE:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(lhs.Double <= rhs.Double);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(lhs.Double <= rhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(true);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_INTEGER:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(lhs.Integer <= rhs.Double);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(lhs.Integer <= rhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(true);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_STRING:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(true);
                    break;
            }
        }
            break;
    }

    return returnValue;
}

/* != operator */
PrimativeValue* operator != (PrimativeValue const & lhs, PrimativeValue const & rhs)
{
    PrimativeValue* returnValue = new PrimativeValue;

    switch (lhs.Type)
    {
        case PrimativeValue::Type::PRI_BOOL:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(lhs.Bool != rhs.Bool);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(lhs.Bool);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(lhs.Bool);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(lhs.Bool);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_DOUBLE:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(lhs.Double != rhs.Double);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(lhs.Double != rhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(true);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_INTEGER:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(lhs.Integer != rhs.Double);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(lhs.Integer != rhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(true);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_STRING:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(lhs.String != rhs.String);
                    break;
            }
        }
            break;
    }

    return returnValue;
}

/* == operator */
PrimativeValue* operator == (PrimativeValue const & lhs, PrimativeValue const & rhs)
{
    PrimativeValue* returnValue = new PrimativeValue;

    switch (lhs.Type)
    {
        case PrimativeValue::Type::PRI_BOOL:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(lhs.Bool == rhs.Bool);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(lhs.Bool);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(lhs.Bool);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(lhs.Bool);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_DOUBLE:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(lhs.Double == rhs.Double);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(lhs.Double == rhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(true);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_INTEGER:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(lhs.Integer == rhs.Double);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(lhs.Integer == rhs.Integer);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(true);
                    break;
            }
        }
            break;

        case PrimativeValue::Type::PRI_STRING:
        {
            switch (rhs.Type)
            {
                case PrimativeValue::Type::PRI_BOOL:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_INTEGER:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_DOUBLE:
                    returnValue->SetBool(true);
                    break;

                case PrimativeValue::Type::PRI_STRING:
                    returnValue->SetBool(lhs.String == rhs.String);
                    break;
            }
        }
            break;
    }

    return returnValue;
}
