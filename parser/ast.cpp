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

void FunctionCallAST::dump(const int indentcount)
{
    printf("FunctionCall(name: %s\n", name.c_str());
    indent(indentcount + 1);

    puts("args: [");
    for (auto &&i : args)
    {
        indent(indentcount + 2);
        i->dump(indentcount + 2);
    }
    indent(indentcount + 1);
    puts("]");
    indent(indentcount);
    puts(")");
}

void BaseStatementAST::dump(const int indentcount)
{
    puts("!!!BaseStatement()");
}

void ExprStatementAST::dump(const int indentcount)
{
    expr->dump(indentcount);
}

void BlockAST::dump(int indentcount)
{
    printf("Block: [\n");
    for (auto &&i : statements)
    {
        indent(indentcount + 1);
        i->dump(indentcount + 1);
    }
    indent(indentcount);
    puts("]");
}

void IfStatementAST::dump(const int indentcount)
{
    printf("If(condition: ");
    condition->dump(indentcount + 1);

    indent(indentcount + 1);
    printf("true: ");
    block->dump(indentcount + 1);

    indent(indentcount + 1);
    printf("false: ");
    elseBlock->dump(indentcount + 1);

    indent(indentcount);
    puts(")");
}

void WhileStatementAST::dump(const int indentcount)
{
    printf("While(condition: ");
    condition->dump(indentcount + 1);

    indent(indentcount + 1);
    printf("block: ");
    block->dump(indentcount + 1);

    indent(indentcount);
    puts(")");
}

void ReturnStatementAST::dump(const int indentcount)
{
    printf("Return(expr: ");
    if (expr == nullptr)
    {
        puts("(void return)");
    }
    else
    {
        expr->dump(indentcount + 1);
    }
    indent(indentcount);
    puts(")");
}
