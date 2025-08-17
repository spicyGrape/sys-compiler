#pragma once
#include <memory>
#include <string>
class BaseAST {
public:
  virtual ~BaseAST() = default;

  virtual void Dump() const = 0;
};

class CompUnitAST : public BaseAST {
public:
public:
  std::unique_ptr<BaseAST> func_def;
  void Dump() const override;
};

class FuncDefAST : public BaseAST {
public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;
  void Dump() const override;
};

class FuncTypeAST : public BaseAST {
public:
  std::string type;
  void Dump() const override;
};

class BlockAST : public BaseAST {
public:
  std::unique_ptr<BaseAST> stmt;
  void Dump() const override;
};

class StmtAST : public BaseAST {
public:
  std::unique_ptr<BaseAST> number;
  void Dump() const override;
};

class NumberAST : public BaseAST {
public:
  int int_const;
  void Dump() const override;
};
