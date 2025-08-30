#pragma once
#include "koopa.h"
#include <string>
#include <cassert>
#include <vector>
#include <unordered_map>

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

    // Strongly-typed RISC-V register names
    enum class Reg
    {
        t0,
        t1,
        t2,
        t3,
        t4,
        t5,
        t6,
        a0,
        a1,
        a2,
        a3,
        a4,
        a5,
        a6,
        a7
    };

    // Convert enum to assembly name
    static inline const char *reg_name(Reg r)
    {
        switch (r)
        {
        case Reg::t0:
            return "t0";
        case Reg::t1:
            return "t1";
        case Reg::t2:
            return "t2";
        case Reg::t3:
            return "t3";
        case Reg::t4:
            return "t4";
        case Reg::t5:
            return "t5";
        case Reg::t6:
            return "t6";
        case Reg::a0:
            return "a0";
        case Reg::a1:
            return "a1";
        case Reg::a2:
            return "a2";
        case Reg::a3:
            return "a3";
        case Reg::a4:
            return "a4";
        case Reg::a5:
            return "a5";
        case Reg::a6:
            return "a6";
        case Reg::a7:
            return "a7";
        }
        return "";
    }

    // Tiny linear-scan style allocator for caller-saved temps
    struct RegisterAllocator
    {
        std::vector<Reg> free_list;

        RegisterAllocator()
        {
            // Prefer t-registers for temporaries, then a-registers (except a0 which is return)
            free_list = {
                Reg::t0, Reg::t1, Reg::t2, Reg::t3, Reg::t4, Reg::t5, Reg::t6,
                Reg::a2, Reg::a3, Reg::a4, Reg::a5, Reg::a6, Reg::a7};
        }

        Reg allocate()
        {
            // In a minimal compiler, assume enough temporaries; assert otherwise
            assert(!free_list.empty());
            Reg r = free_list.back();
            free_list.pop_back();
            return r;
        }

        void release(Reg r)
        {
            free_list.push_back(r);
        }
    };

    RegisterAllocator reg_alloc_;

    // Map Koopa IR SSA values to the physical register holding their result
    std::unordered_map<koopa_raw_value_t, Reg> value_reg_;

    // Helper: get the register assigned to a value if any (defined in .cpp)
    bool reg_of(koopa_raw_value_t v, Reg &out) const;

    // Assign a new register to a Koopa value (if not already assigned) (defined in .cpp)
    Reg ensure_reg(koopa_raw_value_t v);

    // Deal with loading values into a register (allocates if integer literal)
    std::string get_reg_from_value(const koopa_raw_value_t &value, Reg &r);

    // Visit functions for different Koopa IR elements
    std::string visit(const koopa_raw_program_t &program);
    std::string visit(const koopa_raw_slice_t &slice);
    std::string visit(const koopa_raw_function_t &func);
    std::string visit(const koopa_raw_basic_block_t &bb);
    std::string visit(const koopa_raw_value_t &value);
    std::string visit(const koopa_raw_return_t &ret);
    std::string visit(const koopa_raw_integer_t &integer, Reg r);
    std::string visit(const koopa_raw_binary_t &binary, Reg r);
};
