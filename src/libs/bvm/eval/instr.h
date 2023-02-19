#pragma once

#include <brutal-base>

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
    ITER(PUSHI)        \
    ITER(PUSHU)        \
    ITER(PUSHF)        \
    ITER(PUSHN)        \
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
    ITER(JMPF)         \
                       \
    ITER(EQ)           \
                       \
    ITER(ADDI)         \
    ITER(SUBI)         \
    ITER(DIVI)         \
    ITER(MULI)         \
    ITER(MODI)         \
    ITER(NEGI)         \
                       \
    ITER(GTI)          \
    ITER(GTEQI)        \
                       \
    ITER(ADDU)         \
    ITER(SUBU)         \
    ITER(DIVU)         \
    ITER(MULU)         \
    ITER(MODU)         \
    ITER(NEGU)         \
                       \
    ITER(GTU)          \
    ITER(GTEQU)        \
                       \
    ITER(ADDN)         \
    ITER(SUBN)         \
    ITER(DIVN)         \
    ITER(MULN)         \
    ITER(MODN)         \
    ITER(NEGN)         \
                       \
    ITER(GTN)          \
    ITER(GTEQN)        \
                       \
    ITER(I2U)          \
    ITER(I2N)          \
    ITER(U2I)          \
    ITER(U2N)          \
    ITER(N2I)          \
    ITER(N2U)          \
                       \
    ITER(SUPER)        \
                       \
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

static_assert(__BVM_OP_COUNT <= __BVM_OP_MAX, "");

// clang-format on

typedef struct
{
    uint8_t opcode;

    union
    {
        uint64_t uarg;
        int64_t iarg;
        double farg;
    };
} BvmInstr;

Str bvm_op_str(BvmOp op);
