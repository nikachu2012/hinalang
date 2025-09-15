#include "parser.hpp"

BlockAST *parser::parseBlock()
{
    if (lex.lex() != lexer::LEXER_TYPE::LEFT_BRACE)
    {
        Error::err("Expected '{'.");
    }

    std::vector<BaseStatementAST *> statements;

    while (lex.lex() != lexer::LEXER_TYPE::RIGHT_BRACE)
    {
        lex.pbToken();
        statements.push_back(parseStatement());
    }

    return new BlockAST(statements);
}

BaseStatementAST *parser::parseStatement()
{
    lexer::LEXER_TYPE tempLex = lex.lex();

    if (tempLex == lexer::LEXER_TYPE::KEYWORD && lex.getBuf() == "if")
    {
        // if statement
        BaseAST *condition = parseExpr1();
        BlockAST *block = parseBlock();

        tempLex = lex.lex();
        if (tempLex == lexer::LEXER_TYPE::KEYWORD && lex.getBuf() == "else")
        {
            BlockAST *elseBlock = parseBlock();
            return new IfStatementAST(condition, block, elseBlock);
        }
        else
        {
            lex.pbToken();
            BlockAST *elseBlock = new BlockAST({});
            return new IfStatementAST(condition, block, elseBlock);
        }
    }
    else if (tempLex == lexer::LEXER_TYPE::KEYWORD && lex.getBuf() == "while")
    {
        // while statement
        BaseAST *condition = parseExpr1();
        BlockAST *block = parseBlock();

        return new WhileStatementAST(condition, block);
    }
    else if (tempLex == lexer::LEXER_TYPE::KEYWORD && lex.getBuf() == "return")
    {
        // return statement

        // 戻り値なしの時
        if (lex.lex() == lexer::LEXER_TYPE::SEMICOLON)
        {
            return new ReturnStatementAST(nullptr);
        }

        lex.pbToken();
        BaseAST *expr = parseExpr1();

        if (lex.lex() != lexer::LEXER_TYPE::SEMICOLON)
        {
            /* code */
            Error::err("Expected ';'");
        }

        return new ReturnStatementAST(expr);
    }
    else if (tempLex == lexer::LEXER_TYPE::KEYWORD && lex.getBuf() == "fn")
    {
        return parseFunctionDef();
    }
    else
    {
        // expr statement
        lex.pbToken();

        BaseAST *expr = parseExpr1();

        // 式の次がセミコロンかチェック
        if (lex.lex() == lexer::LEXER_TYPE::SEMICOLON)
        {
            return new ExprStatementAST(expr);
        }
        else
        {
            Error::err("Missing semicolon.");
        }
    }

    return nullptr;
}

FunctionDefineAST *parser::parseFunctionDef()
{
    // "fn"はすでに読まれたものとする

    if (lex.lex() != lexer::LEXER_TYPE::KEYWORD)
    {
        Error::err("Expected function name.");
    }
    std::string funcName = lex.getBuf();

    if (lex.lex() != lexer::LEXER_TYPE::LEFT_BRACKET)
    {
        Error::err("Expected '('");
    }

    std::map<std::string, std::string> arguments;
    while (true)
    {
        lexer::LEXER_TYPE lexx = lex.lex();
        if (lexx == lexer::LEXER_TYPE::RIGHT_BRACKET)
        {
            break;
        }

        // declare-argument = keyword keyword [ "," keyword keyword ]*
        //                    ^^^^^^^^^^^^^^^
        if (lexx != lexer::LEXER_TYPE::KEYWORD)
        {
            Error::err("Expected argument type.");
        }
        std::string argType = lex.getBuf();
        if (lex.lex() != lexer::LEXER_TYPE::KEYWORD)
        {
            Error::err("Expected argument name.");
        }
        std::string argName = lex.getBuf();

        // argumentsに値を書き込み
        if (!arguments.insert({argName, argType}).second)
        {
            // argumentsに挿入できなかったとき
            Error::err("Argument \"%s\" is already exist.", argName.c_str());
        }
        
        lexx = lex.lex();
        if (lexx == lexer::LEXER_TYPE::RIGHT_BRACKET)
        {
            break;
        }
        else if (lexx == lexer::LEXER_TYPE::COMMA)
        {
            continue;
        }
        else
        {
            Error::err("Expected ','.");
        }
    }

    // "fn" keyword "(" declare-argument ")" "->" keyword block
    //                                       ^^^^
    if (lex.lex() != lexer::LEXER_TYPE::RIGHT_ARROW)
    {
        Error::err("Expected '->'.");
    }

    if (lex.lex() != lexer::LEXER_TYPE::KEYWORD)
    {
        Error::err("Expected return type.");
    }
    std::string retType = lex.getBuf();

    if (lex.lex() == lexer::LEXER_TYPE::SEMICOLON)
    {
        return new FunctionDefineAST(funcName, retType, arguments, nullptr);
    }
    else
    {
        lex.pbToken();
        BlockAST *block = parseBlock();
        return new FunctionDefineAST(funcName, retType, arguments, block);
    }
}

BaseAST *parser::parseExpr1()
{
    BaseAST *lhs = parseExpr2();

    while (true)
    {
        lexer::LEXER_TYPE op = lex.lex();

        if (op != lexer::LEXER_TYPE::OPERATOR)
        {
            lex.pbToken();
            return lhs;
        }

        std::string opVal = lex.getBuf();
        if (opVal == "<" || opVal == "<=" || opVal == ">" || opVal == ">=" || opVal == "==" || opVal == "!=")
        {
            BaseAST *rhs = parseExpr2();
            lhs = new EquationAST(lhs, rhs, opVal);

            continue;
        }
        else
        {
            lex.pbToken();
            return lhs;
        }
    }
}

BaseAST *parser::parseExpr2()
{
    BaseAST *lhs = parseExpr3();

    while (true)
    {
        lexer::LEXER_TYPE op = lex.lex();

        if (op != lexer::LEXER_TYPE::OPERATOR)
        {
            lex.pbToken();
            return lhs;
        }

        std::string opVal = lex.getBuf();
        if (opVal == "<<" || opVal == ">>")
        {
            BaseAST *rhs = parseExpr3();
            lhs = new EquationAST(lhs, rhs, opVal);

            continue;
        }
        else
        {
            lex.pbToken();
            return lhs;
        }
    }
}

// expr3 = expr4 [ { "+" | "-" } expr4 ]*
BaseAST *parser::parseExpr3()
{
    BaseAST *lhs = parseExpr4();

    while (true)
    {
        // expr3 = expr4 [ { "+" | "-" } expr4 ]*
        //     この部分を解析 ^^^^^^^^^^^^^
        lexer::LEXER_TYPE op = lex.lex();

        // expr4の後に演算子が来ない=演算式ではない
        if (op != lexer::LEXER_TYPE::OPERATOR)
        {
            // 1個先読みして違かったので戻す
            lex.pbToken();
            return lhs;
        }

        std::string opVal = lex.getBuf();
        if (opVal == "+" || opVal == "-")
        {
            // expr4の後に + または - が来る場合
            BaseAST *rhs = parseExpr4();
            lhs = new EquationAST(lhs, rhs, opVal);

            continue;
        }
        else
        {
            lex.pbToken();
            return lhs;
        }
    }
}

// expr4 = factor [ { "*" | "/" | "%" } factor ]*
BaseAST *parser::parseExpr4()
{
    BaseAST *lhs = parseFactor();

    while (true)
    {
        lexer::LEXER_TYPE op = lex.lex();

        if (op != lexer::LEXER_TYPE::OPERATOR)
        {
            lex.pbToken();

            return lhs;
        }

        std::string opVal = lex.getBuf();
        if (opVal == "*" || opVal == "/" || opVal == "%")
        {

            BaseAST *rhs = parseFactor();
            lhs = new EquationAST(lhs, rhs, opVal);

            continue;
        }
        else
        {
            lex.pbToken();
            return lhs;
        }
    }
}

BaseAST *parser::parseFactor()
{
    lexer::LEXER_TYPE factor = lex.lex();

    if (factor == lexer::LEXER_TYPE::NUMBER)
    {
        return new ImmediateIntAST(lex.getBuf());
    }
    else if (factor == lexer::LEXER_TYPE::KEYWORD)
    {
        std::string tempName = lex.getBuf();

        // 一つ先を読む
        factor = lex.lex();
        if (factor == lexer::LEXER_TYPE::KEYWORD)
        {
            std::string variableName = lex.getBuf();

            // keyword keyword "=" expr1
            //         ここを解析 ^^^
            factor = lex.lex();
            if (factor == lexer::LEXER_TYPE::OPERATOR && lex.getBuf() == "=")
            {
                BaseAST *value = parseExpr1();
                return new DefineVariableAST(tempName, variableName, value);
            }
            else
            {
                Error::err("Expected '='");
            }
        }
        else if (factor == lexer::LEXER_TYPE::OPERATOR && lex.getBuf() == "=")
        {
            BaseAST *value = parseExpr1();
            return new AssignAST(tempName, value);
        }
        else if (factor == lexer::LEXER_TYPE::LEFT_BRACKET)
        {
            // 関数呼び出し
            std::vector<BaseAST *> arguments;

            while (true)
            {
                factor = lex.lex();
                // 引数なしの時
                if (factor == lexer::LEXER_TYPE::RIGHT_BRACKET)
                {
                    break;
                }
                else
                {
                    lex.pbToken();
                }

                BaseAST *value = parseExpr1();
                arguments.push_back(value);

                factor = lex.lex();
                if (factor == lexer::LEXER_TYPE::RIGHT_BRACKET)
                {
                    break;
                }
                else if (factor == lexer::LEXER_TYPE::COMMA)
                {
                    continue;
                }
                else
                {
                    Error::err("Expected ',' or ')'.");
                }
            }

            return new FunctionCallAST(tempName, arguments);
        }
        else
        {
            lex.pbToken();
            return new VariableAST(tempName);
        }
    }
    else if (factor == lexer::LEXER_TYPE::STRING)
    {
        return new ImmediateStringAST(lex.getBuf());
    }
    else if (factor == lexer::LEXER_TYPE::LEFT_BRACKET)
    {
        BaseAST *expr = parseExpr1();

        factor = lex.lex();
        if (factor != lexer::LEXER_TYPE::RIGHT_BRACKET)
        {
            Error::err("Expected ')'");
        }

        return expr;
    }
    else
    {
        Error::err("Invalid syntax. (lexer: %d, buf: %s)", factor, lex.getBuf().c_str());
    }

    return new BaseAST();
}

void parser::parseProgram()
{
    while (lex.lex() != lexer::LEXER_TYPE::END)
    {
        lex.pbToken();

        parseStatement()->dump(0);
    }
}
