#include "lib/iolib.h"
#include "Macros.h"
#include "System.h"
#include "Exceptions.h"

Void print_(PrimativeValue message)
{
	console_out << (string_type)message;
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
