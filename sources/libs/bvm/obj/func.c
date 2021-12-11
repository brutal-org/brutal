#include <bvm/obj/func.h>

BvmSig *bvm_sig_create(BvmType *ret, bool variadic, Alloc *alloc)
{
    BvmSig *self = alloc_make(alloc, BvmSig);

    self->ret = ret;
    self->variadic = variadic;
    vec_init(&self->args, alloc);

    return self;
}

void bvm_sig_arg(BvmSig *self, BvmType *type)
{
    vec_push(&self->args, type);
}

BvmFunc *bvm_func_create(BvmSig *sig, Alloc *alloc)
{
    BvmFunc *self = alloc_make(alloc, BvmFunc);

    self->native = false;
    self->sig = sig;
    vec_init(&self->managed_.code, alloc);

    return self;
}

int bvm_func_local(BvmFunc *self)
{
    return self->managed_.locals++;
}

uint64_t bvm_func_emit(BvmFunc *self, BvmOp op)
{
    uint64_t addr = self->managed_.code.len;
    BvmInstr instr = {.opcode = op, .iarg = 0};
    vec_push(&self->managed_.code, instr);
    return addr;
}

uint64_t bvm_func_emitu(BvmFunc *self, BvmOp op, uint64_t u)
{
    uint64_t addr = self->managed_.code.len;
    BvmInstr instr = {.opcode = op, .uarg = u};
    vec_push(&self->managed_.code, instr);
    return addr;
}

uint64_t bvm_func_emiti(BvmFunc *self, BvmOp op, int64_t i)
{
    uint64_t addr = self->managed_.code.len;
    BvmInstr instr = {.opcode = op, .iarg = i};
    vec_push(&self->managed_.code, instr);
    return addr;
}

uint64_t bvm_func_emitn(BvmFunc *self, BvmOp op, double n)
{
    uint64_t addr = self->managed_.code.len;
    BvmInstr instr = {.opcode = op, .farg = n};
    vec_push(&self->managed_.code, instr);
    return addr;
}

void bvm_func_jump(BvmFunc *self, uint64_t addr, uint64_t dest)
{
    vec_at(&self->managed_.code, addr).uarg = dest;
}
