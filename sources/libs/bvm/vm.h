#pragma once

#include <bvm/gc.h>
#include <bvm/mod.h>
#include <bvm/stack.h>

typedef enum
{
    BVM_OP_NOP,

    BVM_OP_LOADL,
    BVM_OP_LOADG,
    BVM_OP_LOADO,

    BVM_OP_STOREL,
    BVM_OP_STOREG,
    BVM_OP_STOREO,

    BVM_OP_CALL,
    BVM_OP_RET,
    BVM_OP_HALT,

    BVM_OP_JMP,
    BVM_OP_JMPF,

    BVM_OP_ADD,
    BVM_OP_SUB,
    BVM_OP_DIV,
    BVM_OP_MUL,
    BVM_OP_MOD,
    BVM_OP_NEG,

    BVM_OP_GT,
    BVM_OP_EQ,
    BVM_OP_GEQ,
    BVM_OP_NEQ,
    BVM_OP_ISA,

    BVM_OP_AND,
    BVM_OP_OR,
    BVM_OP_NOT,

    BVM_OP_LSHIFT,
    BVM_OP_RSHIFT,
    BVM_OP_BAND,
    BVM_OP_BOR,
    BVM_OP_BXOR,
    BVM_OP_BNOT,

    BVM_OP_NEW,
} BvmOp;

typedef struct bvm
{
    bool alive;
    uintptr_t ip;

    BvmGc *gc;
    BvmStack *stack;
    BvmMod *mod;
    Buf code;
} Bvm;

void bvm_init(Bvm *self, BvmGc *gc);

void bvm_deinit(Bvm *self);

bool bvm_eval(Bvm *self);
