#include <cstdio>
#include <fstream>

#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "genIR/genIR.hpp"

int main(void)
{
    std::ifstream f("sample.hina");
    if (!f)
    {
        printf("file open error\n");
        return 0;
    }

    lexer lex(f);

    parser par(lex);
    ProgramAST *program = par.parseProgram();
    
    genIR gen;
    gen.generate(program);
    gen.dumpIR();

    f.close();
}
