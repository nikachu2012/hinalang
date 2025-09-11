#ifndef AST_HPP_
#define AST_HPP_

#include <cstdio>
#include <string>
#include <typeinfo>

#include "../error/error.hpp"

constexpr void indent(const int count)
{
    for (int i = 0; i < count * 2; i++)
        putchar(' ');
}

/**
 * @brief 式を表現するクラス
 * BNF上でいうexpr1, expr2, expr3, expr4, factor解析時のデータ構造
 */
class BaseAST
{
public:
    BaseAST() {}
    virtual ~BaseAST() {}

    virtual void dump(const int indent);
};

/**
 * @brief A+Bのような演算式を表現するクラス
 */
class EquationAST : public BaseAST
{
public:
    BaseAST *lhs;
    BaseAST *rhs;
    std::string op;

    EquationAST(BaseAST *lhs, BaseAST *rhs, std::string op) : lhs(lhs), rhs(rhs), op(op) {}
    ~EquationAST()
    {
        delete lhs;
        delete rhs;
    }

    void dump(const int indentcount);
};

/**
 * @brief 整数の即値を表現するクラス
 */
class ImmediateIntAST : public BaseAST
{
public:
    std::string value; // ビット数制限をなくすために文字列

    ImmediateIntAST(std::string val) : value(val) {}
    ~ImmediateIntAST() {}

    void dump(const int indentcount);
};

class VariableAST : public BaseAST
{
public:
    std::string name;

    VariableAST(std::string n) : name(n) {}
    ~VariableAST() {}

    void dump(const int indentcount);
};

class ImmediateStringAST : public BaseAST
{
public:
    std::string value;

    ImmediateStringAST(std::string val) : value(val) {}
    ~ImmediateStringAST() {}

    void dump(const int indentcount);
};
#endif
