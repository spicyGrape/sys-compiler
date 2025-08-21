#include "backend.hpp"
#include "assembly.hpp"
#include "koopa.h"
#include <iostream>
#include <cassert>
#include <string>

std::string Backend::generateAssembly(const char *koopa_cstr)
{
  koopa_raw_program_t raw = parseToRaw(koopa_cstr);
  std::string result = visit(raw);
  return result;
}

koopa_raw_program_t Backend::parseToRaw(const char *koopa_cstr)
{
  koopa_program_t program;
  koopa_error_code_t ret = koopa_parse_from_string(koopa_cstr, &program);
  assert(ret == KOOPA_EC_SUCCESS);

  // 创建一个 raw program builder, 用来构建 raw program
  koopa_raw_program_builder_t builder = koopa_new_raw_program_builder();
  // 将 Koopa IR 程序转换为 raw program
  koopa_raw_program_t raw = koopa_build_raw_program(builder, program);
  // 释放 Koopa IR 程序占用的内存
  koopa_delete_program(program);

  // DON'T FORGET TO DELETE BUILDER!
  // 泄漏了一个builder但是我不想管它
  return raw;
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
  std::cout << "fuck" << std::endl;
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
    return visit(kind.data.integer);
  default:
    // 其他类型暂时遇不到
    assert(false);
  }
}

std::string Backend::visit(const koopa_raw_return_t &ret)
{
  std::string result;
  // Implementation for return instruction
  switch (ret.value->kind.tag)
  {
  case KOOPA_RVT_INTEGER:
    // 如果返回值是整数, 则将其加载到寄存器 a0 中
    result += "li a0, " + visit(ret.value) + "\n";
    break;
  default:
    // 其他类型的返回值暂时不处理
    assert(false);
  }
  result += "ret\n";
  return result;
}

std::string Backend::visit(const koopa_raw_integer_t &integer)
{
  // Implementation for integer instruction
  return std::to_string(integer.value);
}
