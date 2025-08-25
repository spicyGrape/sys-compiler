#include "AST.hpp"
#include <iostream>
#include <ostream>
#include <sstream>

void CompUnitAST::Dump(std::ostream &output) const { func_def->Dump(output); }

void FuncDefAST::Dump(std::ostream &output) const
{
  output << "fun @" << ident << "(): ";
  func_type->Dump(output);
  output << " ";
  block->Dump(output);
}

void FuncTypeAST::Dump(std::ostream &output) const { output << "i32"; }

void BlockAST::Dump(std::ostream &output) const
{
  output << "{\n";
  output << "%entry:\n";
  stmt->Dump(output);
  output << "}\n";
}
void StmtAST::Dump(std::ostream &output) const
{
  std::ostringstream exps;
  exp->solve(exps, 0);
  output << exps.str();
  output << "ret %0" << std::endl;
}

void NumberAST::Dump(std::ostream &output) const { output << int_const; }

void NumberAST::solve(std::ostream &output, uint32_t reg) const
{
  output << "%" << reg << " = add 0, " << int_const << "\n";
}

void ExpAST::Dump(std::ostream &output) const { unary_exp->Dump(output); }
void ExpAST::solve(std::ostream &output, uint32_t reg) const
{
  unary_exp->solve(output, reg);
}

void UnaryExpAST::Dump(std::ostream &output) const
{
  if (type == Type::PRIMARY)
  {
    primary_exp->Dump(output);
  }
  else if (type == Type::UNARY)
  {
    output << unary_op << " ";
    unary_exp->Dump(output);
  }
}

void UnaryExpAST::solve(std::ostream &output, uint32_t reg) const
{
  if (type == Type::PRIMARY)
  {
    primary_exp->solve(output, reg);
  }
  else if (type == Type::UNARY)
  {
    if (unary_op == "+")
    {
      unary_exp->solve(output, reg);
    }
    else
    {
      unary_exp->solve(output, reg + 1);
      if (unary_op == "-")
      {
        output << "%" << reg << " = sub 0, %" << reg + 1 << "\n";
      }
      else if (unary_op == "!")
      {
        output << "%" << reg << " = eq 0, %" << reg + 1 << "\n";
      }
    }
  }
}

void PrimaryExpAST::Dump(std::ostream &output) const
{
  if (type == Type::PARENTHESIZED)
  {
    output << "(";
    exp->Dump(output);
    output << ")";
  }
  else if (type == Type::NUMBER)
  {
    number->Dump(output);
  }
}

void PrimaryExpAST::solve(std::ostream &output, uint32_t reg) const
{
  if (type == Type::PARENTHESIZED)
  {
    exp->solve(output, reg);
  }
  else if (type == Type::NUMBER)
  {
    number->solve(output, reg);
  }
}