#ifndef EASYLANG_EXCEPTIONS_H
#define EASYLANG_EXCEPTIONS_H

#include <stdexcept>

class ParseError : public std::runtime_error
{
public:
    ParseError(char const * message): std::runtime_error(message)
    {
    }
    friend std::ostream& operator<< (std::ostream &out, const ParseError &e);
private:
    // Character position where error occurred. size_t m_position;
};

#endif //EASYLANG_EXCEPTIONS_H
