#include <brutal/debug.h>
#include <bvm/eval.h>
#include <math.h>

BvmVal bvm_call(BvmLocal *local, BvmMem *mem, BvmFunc *func, int argc)
{
    bvm_local_call(local, func, argc);

    while (bvm_eval(local, mem) == BVM_RES_RUN)
        ;
    return bvm_local_pop(local);
}

BvmRes bvm_eval(BvmLocal *local, BvmMem *mem)
{
    BvmInstr instr = bvm_local_ifetch(local);

    switch (instr.opcode)
    {
    case BVM_OP_NOP:
        break;

    case BVM_OP_LOADG:
        bvm_local_push(local, bvm_mem_load(mem, instr.uarg));
        break;

    case BVM_OP_LOADL:
        bvm_local_push(local, bvm_local_load_var(local, instr.uarg));
        break;

    case BVM_OP_LOADA:
        bvm_local_push(local, bvm_local_load_arg(local, instr.uarg));
        break;

    case BVM_OP_LOADO:
        bvm_local_push(
            local,
            bvm_val_load(
                bvm_local_pop(local),
                instr.uarg));
        break;

    case BVM_OP_LOADV:
        bvm_local_push(
            local,
            bvm_val_loadv(
                bvm_local_pop(local),
                instr.uarg));
        break;

    case BVM_OP_STOREG:
        bvm_mem_store(mem, instr.uarg, bvm_local_pop(local));
        break;

    case BVM_OP_STOREL:
        bvm_local_store_var(local, instr.uarg, bvm_local_pop(local));
        break;

    case BVM_OP_STOREA:
        bvm_local_store_arg(local, instr.uarg, bvm_local_pop(local));
        break;

    case BVM_OP_STOREO:
    {
        BvmVal obj = bvm_local_pop(local);
        BvmVal val = bvm_local_pop(local);

        bvm_val_store(obj, instr.uarg, val);
        break;
    }

    case BVM_OP_STOREV:
    {
        BvmVal val = bvm_local_pop(local);
        BvmVal obj = bvm_local_pop(local);

        bvm_val_storev(obj, instr.uarg, val);
        break;
    }

    case BVM_OP_PUSHI:
    {
        bvm_local_push(local, bvm_val_int(instr.iarg));
        break;
    }

    case BVM_OP_PUSHU:
    {
        bvm_local_push(local, bvm_val_uint(instr.uarg));
        break;
    }

    case BVM_OP_PUSHF:
    {
        bvm_local_push(local, bvm_val_uint(instr.farg));
        break;
    }

    case BVM_OP_PUSHN:
    {
        bvm_local_push(local, bvm_val_nil());
        break;
    }

    case BVM_OP_DUP:
    {
        BvmVal val = bvm_local_pop(local);
        bvm_local_push(local, val);
        bvm_local_push(local, val);
        break;
    }

    case BVM_OP_POP:
        bvm_local_pop(local);
        break;

    case BVM_OP_CALL:
    {
        BvmVal val = bvm_local_pop(local);
        bvm_local_call(local, val.func_, instr.uarg);

        if (val.func_->native)
        {
            bvm_local_ret(local, val.func_->native_(local, mem));
        }
        break;
    }

    case BVM_OP_NEW:
    {
        BvmVal val = bvm_local_pop(local);
        if (val.type != BVM_VAL_TYPE)
        {
            bvm_local_push(local, bvm_val_nil());
        }
        else
        {
            bvm_local_push(local, bvm_mem_new(mem, val.type_));
        }
        break;
    }

    case BVM_OP_RET:
    {
        return bvm_local_ret(local, bvm_local_pop(local));
    }

    case BVM_OP_ARGC:
    {
        bvm_local_push(local, bvm_val_int(local->frame_curr->argc));
        break;
    }

    case BVM_OP_HALT:
        return BVM_RES_HALT;

    case BVM_OP_JMP:
        bvm_local_jump(local, instr.iarg);
        break;

    case BVM_OP_JMPF:
    {
        BvmVal val = bvm_local_pop(local);

        if (!bvm_val_truthy(val))
        {
            bvm_local_jump(local, instr.iarg);
        }
        break;
    }

    case BVM_OP_EQ:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(lhs.uint_ == rhs.uint_));

        break;
    }

    case BVM_OP_ADDI:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(lhs.int_ + rhs.int_));
        break;
    }

    case BVM_OP_SUBI:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(lhs.int_ - rhs.int_));
        break;
    }

    case BVM_OP_DIVI:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(lhs.int_ / rhs.int_));
        break;
    }

    case BVM_OP_MULI:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(lhs.int_ * rhs.int_));
        break;
    }

    case BVM_OP_MODI:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(lhs.int_ % rhs.int_));
        break;
    }

    case BVM_OP_NEGI:
    {
        BvmVal val = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(-val.int_));
        break;
    }

    case BVM_OP_GTI:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(lhs.int_ > rhs.int_));
        break;
    }

    case BVM_OP_GTEQI:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(lhs.int_ >= rhs.int_));
        break;
    }

    case BVM_OP_ADDU:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_uint(lhs.uint_ + rhs.uint_));
        break;
    }

    case BVM_OP_SUBU:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_uint(lhs.uint_ - rhs.uint_));
        break;
    }

    case BVM_OP_DIVU:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_uint(lhs.uint_ / rhs.uint_));
        break;
    }

    case BVM_OP_MULU:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_uint(lhs.uint_ * rhs.uint_));
        break;
    }

    case BVM_OP_MODU:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_uint(lhs.uint_ % rhs.uint_));
        break;
    }

    case BVM_OP_NEGU:
    {
        BvmVal val = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_uint(-val.uint_));
        break;
    }

    case BVM_OP_GTU:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(lhs.uint_ > rhs.uint_));
        break;
    }

    case BVM_OP_GTEQU:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(lhs.uint_ >= rhs.uint_));
        break;
    }

    case BVM_OP_ADDN:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_num(lhs.num_ + rhs.num_));
        break;
    }

    case BVM_OP_SUBN:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_num(lhs.num_ - rhs.num_));
        break;
    }

    case BVM_OP_DIVN:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_num(lhs.num_ / rhs.num_));
        break;
    }

    case BVM_OP_MULN:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_num(lhs.num_ * rhs.num_));
        break;
    }

    case BVM_OP_MODN:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_num(fmod(lhs.num_, rhs.num_)));
        break;
    }

    case BVM_OP_NEGN:
    {
        BvmVal val = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_num(-val.num_));
        break;
    }

    case BVM_OP_GTN:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(lhs.num_ > rhs.num_));
        break;
    }

    case BVM_OP_GTEQN:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(lhs.num_ >= rhs.num_));
        break;
    }

    case BVM_OP_I2U:
        bvm_local_push(local, bvm_val_uint(bvm_local_pop(local).int_));
        break;

    case BVM_OP_I2N:
        bvm_local_push(local, bvm_val_num(bvm_local_pop(local).int_));
        break;

    case BVM_OP_U2I:
        bvm_local_push(local, bvm_val_int(bvm_local_pop(local).uint_));
        break;

    case BVM_OP_U2N:
        bvm_local_push(local, bvm_val_num(bvm_local_pop(local).uint_));
        break;

    case BVM_OP_N2I:
        bvm_local_push(local, bvm_val_int(bvm_local_pop(local).num_));
        break;

    case BVM_OP_N2U:
        bvm_local_push(local, bvm_val_uint(bvm_local_pop(local).num_));
        break;

    case BVM_OP_GETTYPE:
    {
        BvmVal val = bvm_local_pop(local);

        if (val.type == BVM_VAL_OBJ && val.obj_)
        {
            bvm_local_push(local, bvm_val_type(val.obj_->type));
        }
        else
        {
            bvm_local_push(local, bvm_val_nil());
        }

        break;
    }

    case BVM_OP_ISNIL:
        bvm_local_push(local, bvm_val_int(bvm_local_pop(local).type == BVM_VAL_NIL));
        break;

    case BVM_OP_ISUNDEF:
        bvm_local_push(local, bvm_val_int(bvm_local_pop(local).type == BVM_VAL_UNDEF));
        break;

    case BVM_OP_ISINT:
        bvm_local_push(local, bvm_val_int(bvm_local_pop(local).type == BVM_VAL_INT));
        break;

    case BVM_OP_ISUINT:
        bvm_local_push(local, bvm_val_int(bvm_local_pop(local).type == BVM_VAL_UINT));
        break;

    case BVM_OP_ISNUM:
        bvm_local_push(local, bvm_val_int(bvm_local_pop(local).type == BVM_VAL_NUM));
        break;

    case BVM_OP_ISOBJ:
        bvm_local_push(local, bvm_val_int(bvm_local_pop(local).type == BVM_VAL_OBJ));
        break;

    case BVM_OP_ISFUNC:
        bvm_local_push(local, bvm_val_int(bvm_local_pop(local).type == BVM_VAL_FUNC));
        break;

    case BVM_OP_ISTYPE:
        bvm_local_push(local, bvm_val_int(bvm_local_pop(local).type == BVM_VAL_TYPE));
        break;

    case BVM_OP_AND:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(bvm_val_truthy(lhs) && bvm_val_truthy(rhs)));
        break;
    }

    case BVM_OP_OR:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(bvm_val_truthy(lhs) || bvm_val_truthy(rhs)));
        break;
    }

    case BVM_OP_NOT:
    {
        BvmVal val = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(!bvm_val_truthy(val)));
        break;
    }

    case BVM_OP_LSHIFT:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_uint(lhs.uint_ << rhs.uint_));
        break;
    }

    case BVM_OP_RSHIFT:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_uint(lhs.uint_ >> rhs.uint_));
        break;
    }

    case BVM_OP_BAND:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_uint(lhs.uint_ & rhs.uint_));
        break;
    }

    case BVM_OP_BOR:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_uint(lhs.uint_ | rhs.uint_));
        break;
    }

    case BVM_OP_BXOR:
    {
        BvmVal rhs = bvm_local_pop(local);
        BvmVal lhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_uint(lhs.uint_ ^ rhs.uint_));
        break;
    }

    case BVM_OP_BNOT:
    {
        BvmVal val = bvm_local_pop(local);
        bvm_local_push(local, bvm_val_uint(~val.uint_));
        break;
    }

    default:
        log$("Unknown opcode: {}", instr.opcode);
        return BVM_RES_ERROR;
    }

    return BVM_RES_RUN;
}
