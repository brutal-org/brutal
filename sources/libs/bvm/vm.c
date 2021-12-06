#include <brutal/debug.h>
#include <bvm/vm.h>

static uint8_t bvm_ifetch8(Bvm *self)
{
    return buf_load_u8(&self->code, self->ip++);
}

static uint16_t bvm_ifetch16(Bvm *self)
{
    return bvm_ifetch8(self) | bvm_ifetch8(self) << 8;
}

bool bvm_eval(Bvm *self)
{
    uint8_t opcode = bvm_ifetch8(self);

    switch (opcode)
    {
    case BVM_OP_NOP:
        break;

    case BVM_OP_LOADL:
        bvm_stack_push(self->stack, bvm_stack_load(self->stack, bvm_ifetch8(self)));
        break;

    case BVM_OP_LOADG:
        bvm_stack_push(self->stack, bvm_mod_load(self->mod, bvm_ifetch8(self)));
        break;

    case BVM_OP_LOADO:
        break;

    case BVM_OP_STOREL:
        bvm_stack_store(self->stack, bvm_ifetch8(self), bvm_stack_pop(self->stack));
        break;

    case BVM_OP_STOREG:
        bvm_mod_store(self->mod, bvm_ifetch8(self), bvm_stack_pop(self->stack));
        break;

    case BVM_OP_STOREO:
        break;

    case BVM_OP_CALL:
    {
        BvmVal val = bvm_stack_pop(self->stack);
        assert_truth(val.type == BVM_VAL_FUNC);

        uint16_t narg = bvm_ifetch16(self);
        BvmFrame *frame = bvm_stack_call(self->stack, self->ip, narg);

        BvmFunc *func = val.func_;

        if (func->native)
        {
            func->native_(self, frame);
        }
        else
        {
            self->ip = func->managed_.entry;
        }

        break;
    }

    case BVM_OP_RET:
    {
        uint16_t nret = bvm_ifetch16(self);
        self->ip = bvm_stack_ret(self->stack, nret);
        break;
    }

    case BVM_OP_HALT:
        return false;

    case BVM_OP_JMP:
        self->ip += bvm_ifetch16(self);
        break;

    case BVM_OP_JMPF:
    {
        BvmVal val = bvm_stack_pop(self->stack);
        uint16_t off = bvm_ifetch16(self);

        if (!bvm_val_truth(val))
        {
            self->ip += off;
        }
    }
    break;

    case BVM_OP_ADD:
    {
        BvmVal lhs = bvm_stack_pop(self->stack);
        BvmVal rhs = bvm_stack_pop(self->stack);

        bvm_stack_push(self->stack, bvm_val_add(lhs, rhs));
    }
    break;

    case BVM_OP_SUB:
    {
        BvmVal lhs = bvm_stack_pop(self->stack);
        BvmVal rhs = bvm_stack_pop(self->stack);

        bvm_stack_push(self->stack, bvm_val_sub(lhs, rhs));
    }
    break;

    case BVM_OP_DIV:
    {
        BvmVal lhs = bvm_stack_pop(self->stack);
        BvmVal rhs = bvm_stack_pop(self->stack);

        bvm_stack_push(self->stack, bvm_val_div(lhs, rhs));
    }
    break;

    case BVM_OP_MUL:
    {
        BvmVal lhs = bvm_stack_pop(self->stack);
        BvmVal rhs = bvm_stack_pop(self->stack);

        bvm_stack_push(self->stack, bvm_val_mul(lhs, rhs));
    }
    break;

    case BVM_OP_MOD:
    {
        BvmVal lhs = bvm_stack_pop(self->stack);
        BvmVal rhs = bvm_stack_pop(self->stack);

        bvm_stack_push(self->stack, bvm_val_mod(lhs, rhs));
    }
    break;

    case BVM_OP_NEG:
    {
        BvmVal val = bvm_stack_pop(self->stack);

        bvm_stack_push(self->stack, bvm_val_neg(val));
    }
    break;

    case BVM_OP_GT:
    {
        BvmVal lhs = bvm_stack_pop(self->stack);
        BvmVal rhs = bvm_stack_pop(self->stack);

        bvm_stack_push(self->stack, bvm_val_gt(lhs, rhs));
    }
    break;

    case BVM_OP_EQ:
    {
        BvmVal lhs = bvm_stack_pop(self->stack);
        BvmVal rhs = bvm_stack_pop(self->stack);

        bvm_stack_push(self->stack, bvm_val_eq(lhs, rhs));
    }
    break;

    case BVM_OP_GEQ:
    {
        BvmVal lhs = bvm_stack_pop(self->stack);
        BvmVal rhs = bvm_stack_pop(self->stack);

        bvm_stack_push(self->stack, bvm_val_geq(lhs, rhs));
    }
    break;

    case BVM_OP_NEQ:
    {
        BvmVal lhs = bvm_stack_pop(self->stack);
        BvmVal rhs = bvm_stack_pop(self->stack);

        bvm_stack_push(self->stack, bvm_val_neq(lhs, rhs));
    }
    break;

    case BVM_OP_ISA:
    {
        BvmVal lhs = bvm_stack_pop(self->stack);
        BvmVal rhs = bvm_stack_pop(self->stack);

        bvm_stack_push(self->stack, bvm_val_isa(lhs, rhs));
    }
    break;

    case BVM_OP_AND:
    {
        BvmVal lhs = bvm_stack_pop(self->stack);
        BvmVal rhs = bvm_stack_pop(self->stack);

        bvm_stack_push(self->stack, bvm_val_isa(lhs, rhs));
    }
    break;

    case BVM_OP_OR:
    {
        BvmVal lhs = bvm_stack_pop(self->stack);
        BvmVal rhs = bvm_stack_pop(self->stack);

        bvm_stack_push(self->stack, bvm_val_or(lhs, rhs));
    }
    break;

    case BVM_OP_NOT:
    {
        BvmVal lhs = bvm_stack_pop(self->stack);
        BvmVal rhs = bvm_stack_pop(self->stack);

        bvm_stack_push(self->stack, bvm_val_not(lhs, rhs));
    }
    break;

    case BVM_OP_LSHIFT:
    {
        BvmVal lhs = bvm_stack_pop(self->stack);
        BvmVal rhs = bvm_stack_pop(self->stack);

        bvm_stack_push(self->stack, bvm_val_lshift(lhs, rhs));
    }
    break;

    case BVM_OP_RSHIFT:
    {
        BvmVal lhs = bvm_stack_pop(self->stack);
        BvmVal rhs = bvm_stack_pop(self->stack);

        bvm_stack_push(self->stack, bvm_val_rshift(lhs, rhs));
    }
    break;

    case BVM_OP_BAND:
    {
        BvmVal lhs = bvm_stack_pop(self->stack);
        BvmVal rhs = bvm_stack_pop(self->stack);

        bvm_stack_push(self->stack, bvm_val_band(lhs, rhs));
    }
    break;

    case BVM_OP_BOR:
    {
        BvmVal lhs = bvm_stack_pop(self->stack);
        BvmVal rhs = bvm_stack_pop(self->stack);

        bvm_stack_push(self->stack, bvm_val_bor(lhs, rhs));
    }
    break;

    case BVM_OP_BXOR:
    {
        BvmVal lhs = bvm_stack_pop(self->stack);
        BvmVal rhs = bvm_stack_pop(self->stack);

        bvm_stack_push(self->stack, bvm_val_bxor(lhs, rhs));
    }
    break;

    case BVM_OP_BNOT:
    {
        BvmVal val = bvm_stack_pop(self->stack);
        bvm_stack_push(self->stack, bvm_val_bnot(val));
    }
    break;

    case BVM_OP_NEW:
        break;
    }

    return true;
}
