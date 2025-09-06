#include "error.hpp"

void Error::note(const char *errMsg, ...)
{
    va_list va;
    va_start(va, errMsg);

    fprintf(stderr, ERROR_NOTE_STYLE "%10s:" ERROR_RESET " ", "note");
    vfprintf(stderr, errMsg, va);
    fputc('\n', stderr); // 改行
    
    va_end(va);
}

void Error::err(const char *errMsg, ...)
{
    va_list va;
    va_start(va, errMsg);

    fprintf(stderr, ERROR_ERROR_STYLE "%10s:" ERROR_RESET " ", "error");
    vfprintf(stderr, errMsg, va);
    fputc('\n', stderr); // 改行

    va_end(va);

    exit(EXIT_FAILURE);
}
