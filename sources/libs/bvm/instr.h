#pragma once

#include <brutal/base.h>

#define BVM_FOREACH_OP \
    ITER(NOP)          \
                       \
    ITER(LOADG)        \
    ITER(LOADL)        \
    ITER(LOADA)        \
    ITER(LOADO)        \
    ITER(LOADV)        \
                       \
    ITER(STOREG)       \
    ITER(STOREL)       \
    ITER(STOREA)       \
    ITER(STOREO)       \
    ITER(STOREV)       \
                       \
    ITER(DUP)          \
    ITER(POP)          \
                       \
    ITER(CALL)         \
    ITER(RET)          \
    ITER(ARGC)         \
    ITER(NEW)          \
    ITER(HALT)         \
                       \
    ITER(JMP)          \
    ITER(JEQ)          \
    ITER(JGT)          \
    ITER(JLT)          \
    ITER(JGTEQ)        \
    ITER(JLTEQ)        \
                       \
    ITER(ADDI)         \
    ITER(SUBI)         \
    ITER(DIVI)         \
    ITER(MULI)         \
    ITER(MODI)         \
    ITER(NEGI)         \
    ITER(CMPI)         \
                       \
    ITER(ADDU)         \
    ITER(SUBU)         \
    ITER(DIVU)         \
    ITER(MULU)         \
    ITER(MODU)         \
    ITER(NEGU)         \
    ITER(CMPU)         \
                       \
    ITER(ADDN)         \
    ITER(SUBN)         \
    ITER(DIVN)         \
    ITER(MULN)         \
    ITER(MODN)         \
    ITER(NEGN)         \
    ITER(CMPN)         \
                       \
    ITER(I2U)          \
    ITER(I2N)          \
    ITER(U2I)          \
    ITER(U2N)          \
    ITER(N2I)          \
    ITER(N2U)          \
                       \
    ITER(GETTYPE)      \
    ITER(ISNIL)        \
    ITER(ISUNDEF)      \
    ITER(ISINT)        \
    ITER(ISUINT)       \
    ITER(ISNUM)        \
    ITER(ISOBJ)        \
    ITER(ISFUNC)       \
    ITER(ISTYPE)       \
    ITER(ISA)          \
                       \
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
