#ifndef LEXER_H_
#define LEXER_H_

#include <fstream>
#include <string>
#include "../error/error.hpp"

class lexer
{
private:
    std::ifstream &fs;
    std::streampos prev_pos;
    std::string buf;

    char read() { return fs.get(); }
    void pb() { fs.unget(); }

    static inline bool isDecimal(char c);
    static inline bool isKeyword(char c);

public:
    enum class LEXER_TYPE
    {
        END = -1,
        KEYWORD,
        NUMBER,
        STRING,
        OPERATOR,
        COMMA,         // ,
        SEMICOLON,     // ;
        RIGHT_ARROW,   // ->
        LEFT_BRACKET,  // (
        RIGHT_BRACKET, // )
        LEFT_BRACE,    // {
        RIGHT_BRACE,   // }
    };

    lexer(std::ifstream &f);
    ~lexer();
    LEXER_TYPE lex();
    std::string getBuf() { return buf; }
    void pbToken();
};

#endif
