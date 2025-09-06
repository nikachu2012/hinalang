#include "lexer.hpp"

lexer::lexer(std::ifstream &f) : fs(f)
{
}

lexer::~lexer()
{
}

lexer::LEXER_TYPE lexer::lex()
{ 
    return LEXER_TYPE::END;
}
