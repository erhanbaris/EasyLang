#include "lib/iolib.h"
#include "Macros.h"
#include "System.h"
#include "Exceptions.h"

Void print_(Any& data)
{
	if (data.is<bool>())
		console_out << data.cast<bool>();
	else if (data.is<int>())
		console_out << data.cast<int>();
	else if (data.is<double>())
		console_out << data.cast<double>();
	else if (data.is<string_type>())
		console_out << data.cast<string_type>();
	else if (data.is<std::vector<Any>>())
		console_out << data.cast<bool>();
	else if (data.is<std::unordered_map<string_type, Any>>())
		console_out << data.cast<bool>();
	return Void();
}

string_type readline_()
{
	string_type text;
	std::getline(console_in, text);
	return text;
}

IOLibInit::IOLibInit()
{
    System::SystemPackages[_T("io")] = std::unordered_map<string_type, Caller*>();

	System::SystemPackages[_T("io")][_T("print")] = def_function(print_);
    System::SystemPackages[_T("io")][_T("readline")] = def_function(readline_);
}
