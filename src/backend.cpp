#include "backend.hpp"
#include "koopa.h"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>

std::string Backend::generateAssembly(const char *koopa_cstr)
{

  // 创建一个 raw program builder, 用来构建 raw program
  koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
  koopa_raw_program_t raw = parseToRaw(koopa_cstr, builder);
  std::string result = visit(raw);
  koopa_delete_raw_program_builder(builder);
  return result;
}

koopa_raw_program_t Backend::parseToRaw(const char *koopa_cstr, koopa_raw_program_builder_t builder)
{
  koopa_program_t program;
  koopa_error_code_t ret = koopa_parse_from_string(koopa_cstr, &program);
  assert(ret == KOOPA_EC_SUCCESS);

  // 将 Koopa IR 程序转换为 raw program
  koopa_raw_program_t raw = koopa_build_raw_program(builder, program);
  // 释放 Koopa IR 程序占用的内存
  koopa_delete_program(program);

  return raw;
}

// Helpers moved out of header
bool Backend::reg_of(koopa_raw_value_t v, Reg &out) const
{
  auto it = value_reg_.find(v);
  if (it == value_reg_.end())
    return false;
  out = it->second;
  return true;
}

Backend::Reg Backend::ensure_reg(koopa_raw_value_t v)
{
  auto it = value_reg_.find(v);
  if (it != value_reg_.end())
    return it->second;
  Reg r = reg_alloc_.allocate();
  value_reg_[v] = r;
  return r;
}

// 访问 raw program
std::string Backend::visit(const koopa_raw_program_t &program)
{
  std::string result;
  // 执行一些其他的必要操作
  // ...
  // 访问所有全局变量
  visit(program.values);
  // 访问所有函数
  assemblyPiece += visit(program.funcs);
  result += "  .text\n";
  for (const auto &name : globalIdent)
  {
    result += "  .global " + name + "\n";
  }
  result += assemblyPiece;
  return result;
}

// 访问 raw slice
std::string Backend::visit(const koopa_raw_slice_t &slice)
{
  std::string text;
  std::string global;
  std::string result;
  for (size_t i = 0; i < slice.len; ++i)
  {
    auto ptr = slice.buffer[i];
    // 根据 slice 的 kind 决定将 ptr 视作何种元素
    switch (slice.kind)
    {
    case KOOPA_RSIK_FUNCTION:
      // 访问函数
      result += visit(reinterpret_cast<koopa_raw_function_t>(ptr));
      break;
    case KOOPA_RSIK_BASIC_BLOCK:
      // 访问基本块
      result += visit(reinterpret_cast<koopa_raw_basic_block_t>(ptr));
      break;
    case KOOPA_RSIK_VALUE:
      // 访问指令
      result += visit(reinterpret_cast<koopa_raw_value_t>(ptr));
      break;
    default:
      // 我们暂时不会遇到其他内容, 于是不对其做任何处理
      assert(false);
    }
  }
  return result;
}

// 访问函数
std::string Backend::visit(const koopa_raw_function_t &func)
{
  std::string result;
  // 执行一些其他的必要操作
  std::string funcName = std::string(func->name);
  // Remove leading @ if present
  if (!funcName.empty() && funcName[0] == '@')
  {
    funcName = funcName.substr(1);
  }
  globalIdent.push_back(funcName);
  result += funcName + ":\n";
  // 访问所有基本块
  result += visit(func->bbs);
  return result;
}

// 访问基本块
std::string Backend::visit(const koopa_raw_basic_block_t &bb)
{
  std::string result;
  // 执行一些其他的必要操作
  // ...
  // 访问所有指令
  result += visit(bb->insts);
  return result;
}

// 访问指令
std::string Backend::visit(const koopa_raw_value_t &value)
{
  // 根据指令类型判断后续需要如何访问
  const auto &kind = value->kind;
  switch (kind.tag)
  {
  case KOOPA_RVT_RETURN:
    // 访问 return 指令
    return visit(kind.data.ret);
  case KOOPA_RVT_INTEGER:
    // 访问 integer 指令
    {
      Reg r = ensure_reg(value);
      return visit(kind.data.integer, r);
    }
  case KOOPA_RVT_BINARY:
    // 访问 binary 指令
    {
      Reg r = ensure_reg(value);
      return visit(kind.data.binary, r);
    }
  default:
    // 其他类型暂时遇不到
    assert(false);
  }
  return "程序跑飞啦～";
}

std::string Backend::visit(const koopa_raw_return_t &ret)
{
  std::string result;
  // Implementation for return instruction
  switch (ret.value->kind.tag)
  {
  case KOOPA_RVT_INTEGER:
    // 如果返回值是整数, 则将其加载到寄存器 a0 中
    result += "li a0, " + std::to_string(ret.value->kind.data.integer.value) + "\n";
    break;
  default:
    Reg r = ensure_reg(ret.value);
    result += "mv a0, " + std::string(reg_name(r)) + "\n";
  }
  result += "ret\n";
  return result;
}

std::string Backend::visit(const koopa_raw_integer_t &integer, Reg r)
{
  // Implementation for integer instruction
  return "li " + std::string(reg_name(r)) + ", " + std::to_string(integer.value) + "\n";
}

std::string Backend::visit(const koopa_raw_binary_t &binary, Reg r)
{
  std::string out;
  Reg rs1;
  out += get_reg_from_value(binary.lhs, rs1);

  Reg rs2;
  out += get_reg_from_value(binary.rhs, rs2);
  switch (binary.op)
  {
  case KOOPA_RBO_ADD:
    // Optimization for addition with zero
    if (binary.lhs->kind.tag == KOOPA_RVT_INTEGER && binary.lhs->kind.data.integer.value == 0)
    {
      out = "";
      reg_alloc_.release(rs1);
      if (binary.rhs->kind.tag == KOOPA_RVT_INTEGER)
      {
        reg_alloc_.release(rs2);
        out += "li " + std::string(reg_name(r)) + ", " + std::to_string(binary.rhs->kind.data.integer.value) + "\n";
        return out;
      }
      else
      {
        out += "mv " + std::string(reg_name(r)) + ", " + std::string(reg_name(rs2)) + "\n";
      }
      return out;
    }
    // Fall back to normal addition
    else
    {
      out += "add " + std::string(reg_name(r)) + ", " + reg_name(rs1) + ", " + reg_name(rs2) + "\n";
      break;
    }
  case KOOPA_RBO_SUB:
  {
    out += "sub " + std::string(reg_name(r)) + ", " + reg_name(rs1) + ", " + reg_name(rs2) + "\n";
    break;
  }
  case KOOPA_RBO_EQ:
  {
    out += "xor " + std::string(reg_name(r)) + ", " + reg_name(rs1) + ", " + reg_name(rs2) + "\n";
    out += "seqz " + std::string(reg_name(r)) + ", " + std::string(reg_name(r)) + "\n";
  }
  break;
  default:
    break;
  }
  // Release temps if they were allocated here
  if (binary.lhs->kind.tag == KOOPA_RVT_INTEGER)
    reg_alloc_.release(rs1);
  if (binary.rhs->kind.tag == KOOPA_RVT_INTEGER)
    reg_alloc_.release(rs2);
  return out;
}

std::string Backend::get_reg_from_value(const koopa_raw_value_t &value, Reg &r)
{
  // If value is an integer literal
  if (value->kind.tag == KOOPA_RVT_INTEGER)
  {
    r = reg_alloc_.allocate();
    return "li " + std::string(reg_name(r)) + ", " + std::to_string(value->kind.data.integer.value) + "\n";
  }
  // If value is a previously computed SSA value
  else
  {
    bool ok = reg_of(value, r);
    assert(ok);
    return "";
  }
}