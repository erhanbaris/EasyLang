#ifndef EASYLANG_CONSOLE_H
#define EASYLANG_CONSOLE_H

#include "termcolor.hpp"
/*
#define COLOR_BASE(text, color) std::cout << color ; console_out << _T( text );

#define COLOR_RED(text) COLOR_BASE(text, termcolor::red)
#define COLOR_GREY(text) COLOR_BASE(text, termcolor::grey)
#define COLOR_GREEN(text) COLOR_BASE(text, termcolor::green)
#define COLOR_BLUE(text) COLOR_BASE(text, termcolor::blue)
#define COLOR_YELLOW(text) COLOR_BASE(text, termcolor::yellow)
#define COLOR_CYAN(text) COLOR_BASE(text, termcolor::cyan)
#define COLOR_WHITE(text) COLOR_BASE(text, termcolor::white)
#define COLOR_MAGENTA(text) COLOR_BASE(text, termcolor::magenta)
*/

#define COLOR_BASE(text, color) console_out << _T( text );

#define COLOR_RED(text) COLOR_BASE(text, termcolor::red)
#define COLOR_GREY(text) COLOR_BASE(text, termcolor::grey)
#define COLOR_GREEN(text) COLOR_BASE(text, termcolor::green)
#define COLOR_BLUE(text) COLOR_BASE(text, termcolor::blue)
#define COLOR_YELLOW(text) COLOR_BASE(text, termcolor::yellow)
#define COLOR_CYAN(text) COLOR_BASE(text, termcolor::cyan)
#define COLOR_WHITE(text) COLOR_BASE(text, termcolor::white)
#define COLOR_MAGENTA(text) COLOR_BASE(text, termcolor::magenta)

#endif /* EASYLANG_CONSOLE_H */
