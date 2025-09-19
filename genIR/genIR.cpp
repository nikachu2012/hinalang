#include "genIR.hpp"

void genIR::generate(ProgramAST *t)
{
}

void genIR::dumpIR()
{
    module->print(llvm::errs(), nullptr);
}
