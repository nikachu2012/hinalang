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
    std::vector<std::string> argsName;
    for (auto &&a : fn->arguments)
    {
        args.push_back(getType(a.second));
        argsName.push_back(a.first);
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
        // 関数にentryブロックを追加
        llvm::BasicBlock *bEntry = llvm::BasicBlock::Create(context, "entry", function);
        builder.SetInsertPoint(bEntry);

        // 変数テーブルの作成
        VARIABLE_TABLE vt;
        for (size_t i = 0; i < args.size(); i++)
        {
            // 引数に来た値を持った変数を作る
            llvm::AllocaInst *alloc = builder.CreateAlloca(function->getArg(i)->getType());
            builder.CreateStore(function->getArg(i), alloc);

            vt.insert({argsName[i], alloc});
        }

        generateBlock(fn->block, vt);
    }
}

void genIR::generateBlock(BlockAST *blk, VARIABLE_TABLE vt)
{
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
