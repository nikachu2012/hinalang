#include "genIR.hpp"

llvm::Type *genIR::getType(std::string s)
{
    if (s == "void")
        return builder.getVoidTy();

    else if (s == "bool")
        return builder.getInt1Ty();

    else if (s == "i8")
        return builder.getInt8Ty();

    else if (s == "i16")
        return builder.getInt16Ty();

    else if (s == "i32")
        return builder.getInt32Ty();

    else if (s == "i64")
        return builder.getInt64Ty();

    else if (s == "i256")
        return builder.getIntNTy(256);

    else if (s == "ptr")
        return builder.getPtrTy();

    else
    {
        Error::err("Unexpected type name '%s'.", s.c_str());
    }

    return nullptr;
}

void genIR::generateFunction(FunctionDefineAST *fn)
{
    // 関数が存在するか確認
    if (functionTable.count(fn->name) != 0)
    {
        Error::err("Function '%s' is already defined.", fn->name.c_str());
    }

    // 引数リストの生成
    std::vector<llvm::Type *> args;
    for (auto &&a : fn->arguments)
    {
        args.push_back(getType(a.second));
    }

    // 関数の作成
    llvm::FunctionType *functionType = llvm::FunctionType::get(
        getType(fn->returnType), args, false);
    llvm::Function *function = llvm::Function::Create(
        functionType, llvm::Function::ExternalLinkage, fn->name, module);
    functionTable.insert({fn->name, function});

    if (fn->block == nullptr)
    {
        // プロトタイプ宣言の時
        return;
    }
    else
    {
        // 通常の関数宣言の時
        // あとで実装
    }
}

void genIR::generate(ProgramAST *t)
{
    for (auto &&i : t->block->statements)
    {
        if (FunctionDefineAST *fn = dynamic_cast<FunctionDefineAST *>(i))
        {
            generateFunction(fn);
        }
        else
        {
            Error::err("Unexpected root statements.");
        }
    }
}

void genIR::dumpIR()
{
    module->print(llvm::errs(), nullptr);
}
