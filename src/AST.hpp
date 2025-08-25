#pragma once
#include <memory>
#include <ostream>
#include <string>
class BaseAST
{
public:
  virtual ~BaseAST() = default;

  virtual void Dump(std::ostream &) const = 0;
};

// Base class for all expression-related AST nodes
class BaseExpAST : public BaseAST
{
public:
  virtual ~BaseExpAST() = default;

  // Method for evaluating/solving the expression
  virtual void solve(std::ostream &, uint32_t) const = 0;

  // You could add other common expression methods here, such as:
  // virtual int evaluate() const = 0;  // for constant folding
  // virtual bool isConstant() const = 0;  // to check if expression is constant
};

class CompUnitAST : public BaseAST
{
public:
public:
  std::unique_ptr<BaseAST> func_def;
  void Dump(std::ostream &) const override;
};

class FuncDefAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;
  void Dump(std::ostream &output) const override;
};

class FuncTypeAST : public BaseAST
{
public:
  std::string type;
  void Dump(std::ostream &) const override;
};

class BlockAST : public BaseAST
{
public:
  std::unique_ptr<BaseAST> stmt;
  void Dump(std::ostream &) const override;
};

class StmtAST : public BaseAST
{
public:
  std::unique_ptr<BaseExpAST> exp;
  void Dump(std::ostream &) const override;
};

class NumberAST : public BaseExpAST
{
public:
  int int_const;
  void Dump(std::ostream &) const override;
  void solve(std::ostream &, uint32_t) const override;
};

class ExpAST : public BaseExpAST
{
public:
  std::unique_ptr<BaseExpAST> unary_exp;
  void Dump(std::ostream &) const override;
  void solve(std::ostream &, uint32_t) const override;
};

class UnaryExpAST : public BaseExpAST
{
public:
  enum class Type
  {
    PRIMARY, // Contains only a primary expression
    UNARY    // Contains unary operator + unary expression
  };

  Type type;

  // For PRIMARY type
  std::unique_ptr<BaseExpAST> primary_exp;

  // For UNARY type
  std::string unary_op;
  std::unique_ptr<BaseExpAST> unary_exp;

  // Constructor for PRIMARY type
  UnaryExpAST(std::unique_ptr<BaseExpAST> primary)
      : type(Type::PRIMARY), primary_exp(std::move(primary)) {}

  // Constructor for UNARY type
  UnaryExpAST(const std::string &op, std::unique_ptr<BaseExpAST> unary)
      : type(Type::UNARY), unary_op(op), unary_exp(std::move(unary)) {}

  void Dump(std::ostream &) const override;
  void solve(std::ostream &, uint32_t) const override;
};

class PrimaryExpAST : public BaseExpAST
{
public:
  enum class Type
  {
    PARENTHESIZED, // Contains an expression in parentheses
    NUMBER         // Contains a number
  };

  Type type;

  // For PARENTHESIZED type
  std::unique_ptr<BaseExpAST> exp;

  // For NUMBER type
  std::unique_ptr<BaseExpAST> number;

  // Constructor for PARENTHESIZED type
  PrimaryExpAST(std::unique_ptr<BaseExpAST> expression)
      : type(Type::PARENTHESIZED), exp(std::move(expression)) {}

  // Constructor for NUMBER type
  PrimaryExpAST(std::unique_ptr<BaseExpAST> num, bool is_number)
      : type(Type::NUMBER), number(std::move(num)) {}

  void Dump(std::ostream &) const override;
  void solve(std::ostream &, uint32_t) const override;
};
