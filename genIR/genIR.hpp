#ifndef GENIR_H_
#define GENIR_H_

#include <map>

#include "../parser/ast.hpp"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

typedef std::map<std::string, llvm::AllocaInst *> VARIABLE_TABLE;

class genIR
{
private:
    llvm::LLVMContext context;
    llvm::Module *module;
    llvm::IRBuilder<> builder;

    std::map<std::string, llvm::Function *> functionTable;
    llvm::Function *processingFunc;

    llvm::Type *getType(std::string s);

    void generateFunction(FunctionDefineAST *fn);
    void generateBlock(BlockAST *blk, VARIABLE_TABLE vt);

    void generateIf(IfStatementAST *ifs, VARIABLE_TABLE &vt);
    void generateWhile(WhileStatementAST *wh, VARIABLE_TABLE &vt);
    void generateReturn(ReturnStatementAST *ret, VARIABLE_TABLE &vt);

    llvm::Value *generateExpr(BaseAST *ex, VARIABLE_TABLE &vt);
    llvm::Value *generateEquation(EquationAST *eq, VARIABLE_TABLE &vt);
    llvm::Value *generateVariable(VariableAST *va, VARIABLE_TABLE &vt);
    llvm::Value *generateDefineVariable(DefineVariableAST *defv, VARIABLE_TABLE &vt);
    llvm::Value *generateAssign(AssignAST *as, VARIABLE_TABLE &vt);
    llvm::Value *generateFunctionCall(FunctionCallAST *fnc, VARIABLE_TABLE &vt);

public:
    genIR() : builder(context), functionTable()
    {
        module = new llvm::Module("main", context);
    }
    ~genIR()
    {
        delete module;
    }

    void generate(ProgramAST *t);
    void dumpIR();
};

#endif
