#include "AST.hpp"
#include <iostream>

void CompUnitAST::Dump() const { func_def->Dump(); }

void FuncDefAST::Dump() const {
  std::cout << "fun @" << ident << "(): ";
  func_type->Dump();
  std::cout << " ";
  block->Dump();
}

void FuncTypeAST::Dump() const { std::cout << "i32"; }

void BlockAST::Dump() const {
  std::cout << "{\n";
  std::cout << "%entry:\n";
  stmt->Dump();
  std::cout << "}\n";
}

void StmtAST::Dump() const {
  std::cout << "ret ";
  number->Dump();
  std::cout << std::endl;
}

void NumberAST::Dump() const { std::cout << int_const; }
