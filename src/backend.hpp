#pragma once
#include "koopa.h"
#include <string>
#include <vector>

class Backend
{
public:
    Backend() = default;
    ~Backend() = default;

    // Main entry point for converting Koopa IR to assembly
    std::string generateAssembly(const char *koopa_cstr);

private:
    // Parse Koopa IR string to raw program
    koopa_raw_program_t parseToRaw(const char *koopa_cstr, koopa_raw_program_builder_t builder);

    std::vector<std::string> text;
    std::vector<std::string> globalIdent;
    std::string assemblyPiece;

    // Visit functions for different Koopa IR elements
    std::string visit(const koopa_raw_program_t &program);
    std::string visit(const koopa_raw_slice_t &slice);
    std::string visit(const koopa_raw_function_t &func);
    std::string visit(const koopa_raw_basic_block_t &bb);
    std::string visit(const koopa_raw_value_t &value);
    std::string visit(const koopa_raw_return_t &ret);
    std::string visit(const koopa_raw_integer_t &integer);
};
