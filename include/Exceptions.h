#ifndef EASYLANG_EXCEPTIONS_H
#define EASYLANG_EXCEPTIONS_H

#include <stdexcept>
#include "Definitions.h"

class ParseError : public std::runtime_error
{
public:
    ParseError(string_type const & message) : std::runtime_error(AS_CHAR(message.c_str()))
    {
    }
};

class ConvertError : public std::runtime_error
{
public:
    ConvertError(string_type  const & message): std::runtime_error(AS_CHAR(message.c_str()))
    {
    }
};

class ParameterError : public std::runtime_error
{
public:
    ParameterError(string_type const & message): std::runtime_error(AS_CHAR(message.c_str()))
    {
    }
};

class NullException : public std::runtime_error
{
public:
    NullException(string_type const & message) : std::runtime_error(AS_CHAR(message.c_str()))
    {
    }
};

#endif //EASYLANG_EXCEPTIONS_H
