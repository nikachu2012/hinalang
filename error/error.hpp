#ifndef ERROR_H_
#define ERROR_H_

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#define ERROR_RED_COLOR "\e[31m"
#define ERROR_GREEN_COLOR "\e[34m"
#define ERROR_BOLD "\e[1m"
#define ERROR_RESET "\e[0m"

#define ERROR_NOTE_STYLE ERROR_GREEN_COLOR ERROR_BOLD
#define ERROR_ERROR_STYLE ERROR_RED_COLOR ERROR_BOLD

class Error
{
public:
    static void note(const char *errMsg, ...);
    static void err(const char *errMsg, ...);
};

#endif
