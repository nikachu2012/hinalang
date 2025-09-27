#include <cstdio>
#include <fstream>

#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "genIR/genIR.hpp"
#include "error/error.hpp"
#include "llvm/Support/CommandLine.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/IR/LegacyPassManager.h"

llvm::cl::opt<std::string> inputSource(
    llvm::cl::Positional,
    llvm::cl::desc("<input source>"),
    llvm::cl::Required);
llvm::cl::opt<std::string> OutputFile(
    "o",
    llvm::cl::desc("output file"),
    llvm::cl::value_desc("filename"),
    llvm::cl::init("out.o"));


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

    // オブジェクトファイルの生成
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    llvm::Triple targetTriple = llvm::Triple(llvm::sys::getDefaultTargetTriple());
    gen.getModule()->setTargetTriple(targetTriple);

    std::string error;
    const llvm::Target *target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
    if (!target)
    {
        llvm::errs() << error;
        Error::err("can't lookup target (triple: %s)", targetTriple.str().c_str());
        return 1;
    }

    std::string cpu = "generic";
    std::string features = "";

    llvm::TargetOptions targetOpt;
    auto targetMachine = target->createTargetMachine(
        targetTriple, cpu, features, targetOpt, llvm::Reloc::PIC_);
    gen.getModule()->setDataLayout(targetMachine->createDataLayout());

    std::error_code errorCode;
    llvm::raw_fd_ostream stream(OutputFile, errorCode);

    if (stream.has_error())
    {
        /* code */
        Error::err("File '%s' save failed. (err: %s)", OutputFile.c_str(), errorCode.message().c_str());
    }

    llvm::legacy::PassManager pass;
    targetMachine->addPassesToEmitFile(pass, stream, nullptr, llvm::CodeGenFileType::ObjectFile);
    pass.run(*gen.getModule());
    stream.flush();

    // gen.getModule()->print(stream, nullptr);

    return 0;
}
