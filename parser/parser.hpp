#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <iostream>
#include <string>

#include "../error/error.hpp"
#include "../lexer/lexer.hpp"
#include "ast.hpp"

class parser
{
private:
    lexer &lex;

    BaseAST *parseExpr3();
    BaseAST *parseExpr4();
    BaseAST *parseFactor();

public:
    parser(lexer &lex) : lex(lex) {}
    ~parser() {}

    void parseProgram();
};

#endif
