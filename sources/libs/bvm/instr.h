#pragma once

#include <brutal/base.h>

#define BVM_FOREACH_OP \
    ITER(NOP)          \
    ITER(LOADG)        \
    ITER(LOADL)        \
    ITER(LOADO)        \
    ITER(LOADV)        \
    ITER(STOREG)       \
    ITER(STOREL)       \
    ITER(STOREO)       \
    ITER(STOREV)       \
    ITER(DUP)          \
    ITER(POP)          \
    ITER(CALL)         \
    ITER(NEW)          \
    ITER(RET)          \
    ITER(HALT)         \
    ITER(JMP)          \
    ITER(JMPF)         \
    ITER(ADD)          \
    ITER(SUB)          \
    ITER(DIV)          \
    ITER(MUL)          \
    ITER(MOD)          \
    ITER(NEG)          \
    ITER(GT)           \
    ITER(EQ)           \
    ITER(GEQ)          \
    ITER(NEQ)          \
    ITER(ISA)          \
    ITER(AND)          \
    ITER(OR)           \
    ITER(NOT)          \
    ITER(LSHIFT)       \
    ITER(RSHIFT)       \
    ITER(BAND)         \
    ITER(BOR)          \
    ITER(BXOR)         \
    ITER(BNOT)

// clang-format off

typedef enum
{
#define ITER(name) BVM_OP_##name,
    BVM_FOREACH_OP
#undef ITER

    __BVM_OP_COUNT,
    __BVM_OP_MAX = 0xff,
} BvmOp;

static_assert(__BVM_OP_COUNT <= __BVM_OP_MAX);

// clang-format on

typedef struct ALIGNED(4)
{
    uint8_t opcode;

    union
    {
        le_uint16_t uarg;
        le_int16_t iarg;
    };
} BvmInstr;

static_assert(sizeof(BvmInstr) == 4);

Str bvm_op_str(BvmOp op);
