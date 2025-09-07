#include <cstdio>
#include <fstream>

#include "lexer/lexer.hpp"

int main(void)
{
    std::ifstream f("sample.hina");
    if (!f)
    {
        printf("file open error\n");
        return 0;
    }

    lexer lex(f);

    lexer::LEXER_TYPE type;
    while ((type = lex.lex()) != lexer::LEXER_TYPE::END)
    {
        printf("%d %s\n", type, lex.getBuf().c_str());
    }

    f.close();
}
