#ifndef EASYLANG_EXCEPTIONS_H
#define EASYLANG_EXCEPTIONS_H

#include <stdexcept>

class ParseError : public std::runtime_error
{
public:
    ParseError(char const * message): std::runtime_error(message)
    {
    }

	ParseError(std::string const & message) : std::runtime_error(message)
	{
	}
};

class ConvertError : public std::runtime_error
{
public:
    ConvertError(char const * message): std::runtime_error(message)
    {
    }
};

class ParameterError : public std::runtime_error
{
public:
    ParameterError(std::string const & message): std::runtime_error(message)
    {
    }
};

class NullException : public std::runtime_error
{
public:
	NullException(char const * message) : std::runtime_error(message)
	{
	}
};

#endif //EASYLANG_EXCEPTIONS_H
