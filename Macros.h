#ifndef EASYLANG_MACROS_H
#define EASYLANG_MACROS_H

#include <string>
#include <sstream>

static std::string TrimEnumString(const std::string &s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && isspace(*it)) { it++; }
    std::string::const_reverse_iterator rit = s.rbegin();
    while (rit.base() != it && isspace(*rit)) { rit++; }
    return std::string(it, rit.base());
}
static void SplitEnumArgs(const char* szArgs, std::string Array[], int nMax)
{
    std::stringstream ss(szArgs);
    std::string strSub;
    int nIdx = 0;
    while (ss.good() && (nIdx < nMax)) {
        getline(ss, strSub, ',');
        Array[nIdx] = TrimEnumString(strSub);
        nIdx++;
    }
};
#define DECLARE_ENUM(ename, ...) \
    enum ename { __VA_ARGS__, MAX_NUMBER_OF_##ename }; \
    static std::string ename##Strings[MAX_NUMBER_OF_##ename]; \
    static const char* ename##ToString(ename e) { \
        if (ename##Strings[0].empty()) { SplitEnumArgs(#__VA_ARGS__, ename##Strings, MAX_NUMBER_OF_##ename); } \
        return ename##Strings[e].c_str(); \
    } \
    static ename StringTo##ename(const char* szEnum) { \
        for (int i = 0; i < MAX_NUMBER_OF_##ename; i++) { if (ename##Strings[i] == szEnum) { return (ename)i; } } \
        return MAX_NUMBER_OF_##ename; \
    }


#define INIT_BEGIN namespace\
{\
	class __init\
	{\
	public:\
		__init()\
		{
#define INIT_END }\
	};\
	__init init;\
}

#endif //EASYLANG_MACROS_H
