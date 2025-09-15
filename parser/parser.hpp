#ifndef PARSER_HPP_
#define PARSER_HPP_

#include <iostream>
#include <string>
#include <vector>

#include "../error/error.hpp"
#include "../lexer/lexer.hpp"
#include "ast.hpp"

class parser
{
private:
    lexer &lex;

    BlockAST *parseBlock();
    BaseStatementAST *parseStatement();

    FunctionDefineAST *parseFunctionDef();

    BaseAST *parseExpr1();
    BaseAST *parseExpr2();
    BaseAST *parseExpr3();
    BaseAST *parseExpr4();
    BaseAST *parseFactor();

public:
    parser(lexer &lex) : lex(lex) {}
    ~parser() {}

    void parseProgram();
};

#endif
