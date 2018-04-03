#ifndef EASYLANG_DEFINITIONS_H
#define EASYLANG_DEFINITIONS_H

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

#if defined(_UNICODE)
#include <comdef.h>

#define EASY_STRING_TYPE std::wstring
#define AS_STRING std::to_wstring
#define EASY_STRING_STREAM std::wstringstream
#define console_out std::wcout
#define console_in std::wcin
#define EASY_CHAR_TYPE wchar_t
#define EASY_FILE_STREAM_TYPE std::wifstream
#define AS_CHAR(text) static_cast<char*>(_bstr_t(text))

#if !defined(_T)
#define _T(x) L##x
#endif // not defined _T
#else
#ifndef _T
#define _T(x) x
#endif

#define EASY_STRING_TYPE std::string
#define AS_STRING std::to_string
#define EASY_STRING_STREAM std::stringstream
#define console_out std::cout
#define console_in std::cin
#define EASY_CHAR_TYPE char
#define EASY_FILE_STREAM_TYPE std::ifstream
#define AS_CHAR(text) text

#endif

typedef EASY_STRING_TYPE string_type;
typedef EASY_STRING_STREAM string_stream;
typedef EASY_CHAR_TYPE char_type;
typedef EASY_FILE_STREAM_TYPE file_stream;

#endif