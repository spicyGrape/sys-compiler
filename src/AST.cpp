#include "AST.hpp"
#include <iostream>
#include <ostream>

void CompUnitAST::Dump() const { func_def->Dump(); }
void CompUnitAST::Dump(std::ostream &output) const { func_def->Dump(output); }

void FuncDefAST::Dump() const {
  std::cout << "fun @" << ident << "(): ";
  func_type->Dump();
  std::cout << " ";
  block->Dump();
}
void FuncDefAST::Dump(std::ostream &output) const {
  output << "fun @" << ident << "(): ";
  func_type->Dump(output);
  output << " ";
  block->Dump(output);
}

void FuncTypeAST::Dump() const { std::cout << "i32"; }
void FuncTypeAST::Dump(std::ostream &output) const { output << "i32"; }

void BlockAST::Dump() const {
  std::cout << "{\n";
  std::cout << "%entry:\n";
  stmt->Dump();
  std::cout << "}\n";
}
void BlockAST::Dump(std::ostream &output) const {
  output << "{\n";
  output << "%entry:\n";
  stmt->Dump(output);
  output << "}\n";
}

void StmtAST::Dump() const {
  std::cout << "ret ";
  number->Dump();
  std::cout << std::endl;
}
void StmtAST::Dump(std::ostream &output) const {
  output << "ret ";
  number->Dump(output);
  output << std::endl;
}

void NumberAST::Dump() const { std::cout << int_const; }
void NumberAST::Dump(std::ostream &output) const { output << int_const; }
