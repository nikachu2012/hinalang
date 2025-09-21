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
    processingFunc = function;

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
    for (auto &&st : blk->statements)
    {
        if (ExprStatementAST *ex = dynamic_cast<ExprStatementAST *>(st))
        {
            generateExpr(ex->expr, vt);
        }
        else if (IfStatementAST *ifs = dynamic_cast<IfStatementAST *>(st))
        {
            generateIf(ifs, vt);
        }
        else if (WhileStatementAST *wh = dynamic_cast<WhileStatementAST *>(st))
        {
        }
        else if (ReturnStatementAST *ret = dynamic_cast<ReturnStatementAST *>(st))
        {
            generateReturn(ret, vt);
        }
        else if (dynamic_cast<FunctionDefineAST *>(st))
        {
            // ブロック内での関数定義はできないようにする
            Error::err("Function definitions are not allowed inside a block.");
        }
        else
        {
            Error::err("Unexpected statement type.");
        }
    }
}

void genIR::generateIf(IfStatementAST *ifs, VARIABLE_TABLE &vt)
{
    llvm::Value *condition = generateExpr(ifs->condition, vt);
    condition = builder.CreateIntCast(condition, builder.getInt1Ty(), true);

    llvm::BasicBlock *bThen = llvm::BasicBlock::Create(context, "then", processingFunc);
    llvm::BasicBlock *bElse = llvm::BasicBlock::Create(context, "else", processingFunc);
    llvm::BasicBlock *bEnd = llvm::BasicBlock::Create(context, "end", processingFunc);
    builder.CreateCondBr(condition, bThen, bElse);

    // generate then block
    builder.SetInsertPoint(bThen);
    generateBlock(ifs->block, vt);
    builder.CreateBr(bEnd);

    // generate else block
    builder.SetInsertPoint(bElse);
    generateBlock(ifs->elseBlock, vt);
    builder.CreateBr(bEnd);

    builder.SetInsertPoint(bEnd);
}

void genIR::generateReturn(ReturnStatementAST *ret, VARIABLE_TABLE &vt)
{
    llvm::Type *retType = processingFunc->getReturnType();

    if (retType->isVoidTy() && ret->expr != nullptr)
    {
        // 戻り値がvoidで値を返している時
        Error::err("void function '%s' should not return a value.", processingFunc->getName());
    }
    if (!retType->isVoidTy() && ret->expr == nullptr)
    {
        // 戻り値がvoidでないのに値を返していない時
        Error::err("non-void function '%s' should return a value", processingFunc->getName());
    }

    if (ret->expr == nullptr)
    {
        builder.CreateRetVoid();
        return;
    }

    llvm::Value *value = generateExpr(ret->expr, vt);
    llvm::Value *value2 = builder.CreateIntCast(value, retType, true);
    builder.CreateRet(value2);
}

llvm::Value *genIR::generateExpr(BaseAST *ex, VARIABLE_TABLE &vt)
{
    if (EquationAST *eq = dynamic_cast<EquationAST *>(ex))
    {
        return generateEquation(eq, vt);
    }
    else if (ImmediateIntAST *imi = dynamic_cast<ImmediateIntAST *>(ex))
    {
        // 数値を64bitの10進数として解析
        return builder.getInt(llvm::APInt(64, imi->value, 10));
    }
    else if (VariableAST *va = dynamic_cast<VariableAST *>(ex))
    {
        return generateVariable(va, vt);
    }
    else if (ImmediateStringAST *ims = dynamic_cast<ImmediateStringAST *>(ex))
    {
        return builder.CreateGlobalString(ims->value);
    }
    else if (DefineVariableAST *defv = dynamic_cast<DefineVariableAST *>(ex))
    {
        return generateDefineVariable(defv, vt);
    }
    else if (AssignAST *as = dynamic_cast<AssignAST *>(ex))
    {
        return generateAssign(as, vt);
    }
    else if (FunctionCallAST *fnc = dynamic_cast<FunctionCallAST *>(ex))
    {
        return generateFunctionCall(fnc, vt);
    }
    else
    {
        Error::err("Unexpected expr type.");
    }
    return nullptr;
}

llvm::Value *genIR::generateEquation(EquationAST *eq, VARIABLE_TABLE &vt)
{
    llvm::Value *lhs = generateExpr(eq->lhs, vt);
    llvm::Value *rhs = generateExpr(eq->rhs, vt);
    rhs = builder.CreateIntCast(rhs, lhs->getType(), true);

    if (eq->op == "+")
    {
        return builder.CreateAdd(lhs, rhs);
    }
    else if (eq->op == "-")
    {
        return builder.CreateSub(lhs, rhs);
    }
    else if (eq->op == "*")
    {
        return builder.CreateMul(lhs, rhs);
    }
    else if (eq->op == "/")
    {
        return builder.CreateSDiv(lhs, rhs);
    }
    else if (eq->op == "%")
    {
        return builder.CreateSRem(lhs, rhs);
    }
    else if (eq->op == "<<")
    {
        return builder.CreateShl(lhs, rhs);
    }
    else if (eq->op == ">>")
    {
        return builder.CreateAShr(lhs, rhs);
    }
    else if (eq->op == "==")
    {
        return builder.CreateICmpEQ(lhs, rhs);
    }
    else if (eq->op == "!=")
    {
        return builder.CreateICmpNE(lhs, rhs);
    }
    else if (eq->op == "<")
    {
        return builder.CreateICmpSLT(lhs, rhs);
    }
    else if (eq->op == "<=")
    {
        return builder.CreateICmpSLE(lhs, rhs);
    }
    else if (eq->op == ">")
    {
        return builder.CreateICmpSGT(lhs, rhs);
    }
    else if (eq->op == ">=")
    {
        return builder.CreateICmpSGE(lhs, rhs);
    }
    else
    {
        Error::err("Unexpected operator '%s'.", eq->op.c_str());
    }

    return nullptr;
}

llvm::Value *genIR::generateVariable(VariableAST *va, VARIABLE_TABLE &vt)
{
    VARIABLE_TABLE::iterator var = vt.find(va->name);
    if (var == vt.end())
    {
        Error::err("Variable '%s' is not defined.", va->name.c_str());
    }

    llvm::Value *value = builder.CreateLoad(
        var->second->getAllocatedType(), var->second);
    return value;
}

llvm::Value *genIR::generateDefineVariable(DefineVariableAST *defv, VARIABLE_TABLE &vt)
{
    if (vt.find(defv->dest) != vt.end())
    {
        Error::err("Variable '%s' is already exist.", defv->dest.c_str());
    }

    llvm::Type *type = getType(defv->type);
    llvm::AllocaInst *alloc = builder.CreateAlloca(type);
    vt.insert({defv->dest, alloc});

    // 変数に代入する値を取得
    llvm::Value *value = generateExpr(defv->value, vt);

    // 変数保存先に合わせて型を変更
    llvm::Value *value2 = builder.CreateIntCast(value, type, true);
    builder.CreateStore(value2, alloc);

    return value2;
}

llvm::Value *genIR::generateAssign(AssignAST *as, VARIABLE_TABLE &vt)
{
    VARIABLE_TABLE::iterator var = vt.find(as->dest);
    if (var == vt.end())
    {
        Error::err("Variable '%s' is not defined.", as->dest.c_str());
    }

    llvm::Value *value = generateExpr(as->value, vt);
    llvm::Value *value2 = builder.CreateIntCast(value, var->second->getAllocatedType(), true);
    builder.CreateStore(value2, var->second);

    return value2;
}

llvm::Value *genIR::generateFunctionCall(FunctionCallAST *fnc, VARIABLE_TABLE &vt)
{
    auto func = functionTable.find(fnc->name);
    if (func == functionTable.end())
    {
        Error::err("Function '%s' is not defined.", fnc->name.c_str());
    }

    std::vector<llvm::Value *> args;
    for (size_t i = 0; i < fnc->args.size(); i++)
    {
        llvm::Value *value = generateExpr(fnc->args[i], vt);
        llvm::Value *value2 = builder.CreateIntCast(
            value, func->second->getArg(i)->getType(), true);
        args.push_back(value2);
    }

    return builder.CreateCall(func->second, args);
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
