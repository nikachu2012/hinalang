#include "parser.hpp"

BaseAST *parser::parseExpr1()
{
    BaseAST *lhs = parseExpr2();

    // 仮でそのまま返すようにする
    return lhs;
}

BaseAST *parser::parseExpr2()
{
    BaseAST *lhs = parseExpr3();

    // 仮でそのまま返すようにする
    return lhs;
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
    BaseAST *program = parseExpr3();

    // program.dump(0);
    program->dump(0);
}
