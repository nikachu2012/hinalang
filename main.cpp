#include <cstdio>
#include <fstream>

#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "genIR/genIR.hpp"
#include "error/error.hpp"
#include "llvm/Support/CommandLine.h"

llvm::cl::opt<std::string> inputSource(
    llvm::cl::Positional,
    llvm::cl::desc("<input source>"),
    llvm::cl::Required);
llvm::cl::opt<std::string> OutputFile(
    "o",
    llvm::cl::desc("output file"),
    llvm::cl::value_desc("filename"),
    llvm::cl::init("out.ll"));

int main(int argc, char **argv)
{
    llvm::cl::ParseCommandLineOptions(argc, argv, "hinalang compiler");

    std::ifstream f(inputSource);
    if (!f)
    {
        Error::err("File '%s' open failed.", inputSource.c_str());
        return 1;
    }

    lexer lex(f);

    parser par(lex);
    ProgramAST *program = par.parseProgram();

    genIR gen;
    gen.generate(program);

    f.close();

    std::error_code errorCode;
    llvm::raw_fd_ostream stream(OutputFile, errorCode);

    if (stream.has_error())
    {
        /* code */
        Error::err("File '%s' save failed. (err: %s)", OutputFile.c_str(), errorCode.message().c_str());
    }

    gen.getModule()->print(stream, nullptr);

    return 0;
}
