#include <bvm/ctx/local.h>
#include <bvm/obj/func.h>
#include <brutal-debug>

void bvm_local_init(BvmLocal *self, Alloc *alloc)
{
    self->alloc = alloc;

    self->stack = alloc_make_array(alloc, BvmVal, BVM_MAX_STACK);
    self->stack_top = self->stack;
    self->frame = alloc_make_array(alloc, BvmFrame, BVM_MAX_FRAME);
    self->frame_curr = nullptr;
}

void bvm_local_deinit(BvmLocal *self)
{
    alloc_free(self->alloc, self->stack);
    alloc_free(self->alloc, self->frame);
}

void bvm_local_push(BvmLocal *self, BvmVal val)
{
    *self->stack_top = val;
    self->stack_top++;
}

BvmVal bvm_local_pop(BvmLocal *self)
{
    self->stack_top--;
    return *self->stack_top;
}

BvmFrame *bvm_local_call(BvmLocal *self, BvmFunc *func, int argc)
{
    if (self->frame_curr == nullptr)
    {
        self->frame_curr = self->frame;
    }
    else
    {
        self->frame_curr++;
    }

    BvmFrame *frame = self->frame_curr;

    frame->func = func;
    frame->ip = 0;

    frame->args = self->stack_top - argc;
    frame->argc = argc;

    if (func->native)
    {
        frame->vars = self->stack_top;
        frame->varc = 0;
    }
    else
    {
        frame->vars = self->stack_top + func->managed_.locals;
        frame->varc = func->managed_.locals;

        for (size_t i = 0; i < frame->varc; i++)
        {
            frame->vars[i] = bvm_val_nil();
        }
    }

    return frame;
}

BvmRes bvm_local_ret(BvmLocal *self, BvmVal val)
{
    self->stack_top = self->frame->args;

    bvm_local_push(self, val);

    if (self->frame_curr == self->frame)
    {
        self->frame_curr = nullptr;
        return BVM_RES_RET;
    }
    else
    {
        self->frame_curr--;
        return BVM_RES_RUN;
    }
}

BvmVal bvm_local_load_var(BvmLocal *self, int index)
{
    return self->frame_curr->vars[index];
}

void bvm_local_store_var(BvmLocal *self, int index, BvmVal val)
{
    self->frame_curr->vars[index] = val;
}

BvmVal bvm_local_load_arg(BvmLocal *self, int index)
{
    return self->frame_curr->args[index];
}

void bvm_local_store_arg(BvmLocal *self, int index, BvmVal val)
{
    self->frame_curr->args[index] = val;
}

BvmInstr bvm_local_ifetch(BvmLocal *self)
{
    BvmFrame *frame = self->frame_curr;
    BvmInstr instr = vec_at(&frame->func->managed_.code, frame->ip);
    // log$("IP={}, {} U:{08x}, S:{08x}", frame->ip, bvm_op_str(instr.opcode), instr.uarg, instr.iarg);

    frame->ip++;

    return instr;
}

void bvm_local_jump(BvmLocal *self, uint64_t addr)
{
    self->frame_curr->ip = addr;
}

void bvm_local_dump(BvmLocal *self)
{
    BvmFrame *frame = self->frame;

    for (size_t i = 0; i < frame->argc; i++)
    {
        log$("ARG[{}]={}", i, frame->args[i].uint_);
    }

    for (size_t i = 0; i < frame->varc; i++)
    {
        log$("VAR[{}]={}", i, frame->vars[i].uint_);
    }

    for (BvmVal *s = self->stack_top; s >= frame->args + frame->argc; s--)
    {
        log$("- {}", s->uint_);
    }
}
