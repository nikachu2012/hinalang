#include "ast.hpp"

void BaseAST::dump(const int indentcount)
{
    puts("!!!BaseAST()");
}

void EquationAST::dump(const int indentcount)
{
    printf("Equation(operator: %s\n", op.c_str());

    indent(indentcount + 1);
    printf("lhs: ");
    lhs->dump(indentcount + 1);

    indent(indentcount + 1);
    printf("rhs: ");
    rhs->dump(indentcount + 1);

    indent(indentcount);
    puts(")");
}

void ImmediateIntAST::dump(const int indentcount)
{
    printf("ImmediateInt(value: %s)\n", value.c_str());
}
