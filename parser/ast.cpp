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

void VariableAST::dump(const int indentcount)
{
    printf("Variable(name: %s)\n", name.c_str());
}

void ImmediateStringAST::dump(const int indentcount)
{
    printf("ImmediateString(value: \"%s\")\n", value.c_str());
}

void DefineVariableAST::dump(const int indentcount)
{
    printf("DefineVariable(type: %s, dest: %s\n", type.c_str(), dest.c_str());
    indent(indentcount + 1);
    printf("value: ");
    value->dump(indentcount + 1);
    indent(indentcount);
    puts(")");
}

void AssignAST::dump(const int indentcount)
{
    printf("Assign(dest: %s\n", dest.c_str());
    indent(indentcount + 1);
    printf("value: ");
    value->dump(indentcount + 1);
    indent(indentcount);
    puts(")");
}
