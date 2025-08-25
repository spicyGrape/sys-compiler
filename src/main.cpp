#include "AST.hpp"
#include "backend.hpp"
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "koopa.h"

using namespace std;

// 声明 lexer 的输入, 以及 parser 函数
// 为什么不引用 sysy.tab.hpp 呢? 因为首先里面没有 yyin 的定义
// 其次, 因为这个文件不是我们自己写的, 而是被 Bison 生成出来的
// 你的代码编辑器/IDE 很可能找不到这个文件, 然后会给你报错 (虽然编译不会出错)
// 看起来会很烦人, 于是干脆采用这种看起来 dirty 但实际很有效的手段
extern FILE *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);

int main(int argc, const char *argv[])
{
  // 解析命令行参数. 测试脚本/评测平台要求你的编译器能接收如下参数:
  // compiler 模式 输入文件 -o 输出文件
  assert(argc == 5);
  auto mode = argv[1];
  auto input = argv[2];
  auto output = argv[4];

  // 打开输入文件, 并且指定 lexer 在解析的时候读取这个文件
  yyin = fopen(input, "r");
  assert(yyin);

  // 调用 parser 函数, parser 函数会进一步调用 lexer 解析输入文件的
  unique_ptr<BaseAST> ast;
  auto ret = yyparse(ast);
  assert(!ret);

  // Front end end.
  // Mid end begin.

  ast->Dump(std::cout);

  std::string output_str(output);
  std::ofstream outfile(output_str);

  if (!outfile.is_open())
  {
    std::cerr << "Error: Could not open the file" << output_str << endl;
  }

  if (std::string(mode) == std::string("-koopa"))
  {
    ast->Dump(outfile);
  }

  // Mid end end.
  // Back end begin.

  if (std::string(mode) == std::string("-riscv"))
  {

    std::ostringstream ss;
    ast->Dump(ss);
    std::string koopa_str = ss.str();
    const char *koopa_cstr = koopa_str.c_str();
    Backend backend;
    std::string assembly = backend.generateAssembly(koopa_cstr);
    std::cout << "Generated Assembly:\n"
              << assembly << std::endl;
    outfile << assembly;
  }

  outfile.close();
  return 0;
}
