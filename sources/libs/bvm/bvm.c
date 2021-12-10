#include <brutal/debug.h>
#include <bvm/bvm.h>
#include <math.h>

BvmRes bvm_eval(BvmLocal *local, BvmMem *mem)
{
    BvmInstr instr = bvm_local_ifetch(local);

    log$("{}:{x}(u:{08x}, s:{08x})", bvm_op_str(instr.opcode), instr.opcode, load_le(instr.uarg), load_le(instr.iarg));

    switch (instr.opcode)
    {
    case BVM_OP_NOP:
        break;

    case BVM_OP_LOADG:
        bvm_local_push(local, bvm_mem_load(mem, load_le(instr.uarg)));
        break;

    case BVM_OP_LOADL:
        bvm_local_push(local, bvm_local_load_var(local, load_le(instr.uarg)));
        break;

    case BVM_OP_LOADA:
        bvm_local_push(local, bvm_local_load_arg(local, load_le(instr.uarg)));
        break;

    case BVM_OP_LOADO:
        bvm_local_push(
            local,
            bvm_val_load(
                bvm_local_pop(local),
                load_le(instr.uarg)));
        break;

    case BVM_OP_LOADV:
        bvm_local_push(
            local,
            bvm_val_loadv(
                bvm_local_pop(local),
                load_le(instr.uarg)));
        break;

    case BVM_OP_STOREG:
        bvm_mem_store(mem, load_le(instr.uarg), bvm_local_pop(local));
        break;

    case BVM_OP_STOREL:
        bvm_local_store_var(local, load_le(instr.uarg), bvm_local_pop(local));
        break;

    case BVM_OP_STOREA:
        bvm_local_store_arg(local, load_le(instr.uarg), bvm_local_pop(local));
        break;

    case BVM_OP_STOREO:
    {
        BvmVal obj = bvm_local_pop(local);
        BvmVal val = bvm_local_pop(local);

        bvm_val_store(
            obj,
            load_le(instr.uarg),
            val);
        break;
    }

    case BVM_OP_STOREV:
    {
        BvmVal val = bvm_local_pop(local);
        BvmVal obj = bvm_local_pop(local);

        bvm_val_storev(
            obj,
            load_le(instr.uarg),
            val);
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
        bvm_local_call(local, val.func_, load_le(instr.uarg));

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
        bvm_local_ret(local, bvm_local_pop(local));
        break;
    }

    case BVM_OP_ARGC:
    {
        bvm_local_push(local, bvm_val_int(local->frame_curr->argc));
        break;
    }

    case BVM_OP_HALT:
        return BVM_RES_HALT;

    case BVM_OP_JMP:
        bvm_local_jump(local, load_le(instr.iarg));
        break;

    case BVM_OP_JEQ:
    {
        BvmVal val = bvm_local_pop(local);

        if (val.int_ == 0)
        {
            bvm_local_jump(local, load_le(instr.iarg));
        }
        break;
    }

    case BVM_OP_JGT:
    {
        BvmVal val = bvm_local_pop(local);

        if (val.int_ > 0)
        {
            bvm_local_jump(local, load_le(instr.iarg));
        }
        break;
    }

    case BVM_OP_JLT:
    {
        BvmVal val = bvm_local_pop(local);

        if (val.int_ < 0)
        {
            bvm_local_jump(local, load_le(instr.iarg));
        }
        break;
    }

    case BVM_OP_JGTEQ:
    {
        BvmVal val = bvm_local_pop(local);

        if (val.int_ >= 0)
        {
            bvm_local_jump(local, load_le(instr.iarg));
        }
        break;
    }

    case BVM_OP_JLTEQ:
    {
        BvmVal val = bvm_local_pop(local);

        if (val.int_ <= 0)
        {
            bvm_local_jump(local, load_le(instr.iarg));
        }
        break;
    }

    case BVM_OP_ADDI:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(lhs.int_ + rhs.int_));
        break;
    }

    case BVM_OP_SUBI:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(lhs.int_ - rhs.int_));
        break;
    }

    case BVM_OP_DIVI:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(lhs.int_ / rhs.int_));
        break;
    }

    case BVM_OP_MULI:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(lhs.int_ * rhs.int_));
        break;
    }

    case BVM_OP_MODI:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(lhs.int_ % rhs.int_));
        break;
    }

    case BVM_OP_NEGI:
    {
        BvmVal val = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(-val.int_));
        break;
    }

    case BVM_OP_CMPI:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        if (lhs.int_ > rhs.int_)
        {
            bvm_local_push(local, bvm_val_int(1));
        }
        else if (lhs.int_ < rhs.int_)
        {
            bvm_local_push(local, bvm_val_int(-1));
        }
        else
        {
            bvm_local_push(local, bvm_val_int(0));
        }
        break;
    }

    case BVM_OP_ADDU:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_uint(lhs.uint_ + rhs.uint_));
        break;
    }

    case BVM_OP_SUBU:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_uint(lhs.uint_ - rhs.uint_));
        break;
    }

    case BVM_OP_DIVU:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_uint(lhs.uint_ / rhs.uint_));
        break;
    }

    case BVM_OP_MULU:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_uint(lhs.uint_ * rhs.uint_));
        break;
    }

    case BVM_OP_MODU:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_uint(lhs.uint_ % rhs.uint_));
        break;
    }

    case BVM_OP_NEGU:
    {
        BvmVal val = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_uint(-val.uint_));
        break;
    }

    case BVM_OP_CMPU:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        if (lhs.uint_ > rhs.uint_)
        {
            bvm_local_push(local, bvm_val_int(1));
        }
        else if (lhs.uint_ < rhs.uint_)
        {
            bvm_local_push(local, bvm_val_int(-1));
        }
        else
        {
            bvm_local_push(local, bvm_val_int(0));
        }
        break;
    }

    case BVM_OP_ADDN:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_num(lhs.num_ + rhs.num_));
        break;
    }

    case BVM_OP_SUBN:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_num(lhs.num_ - rhs.num_));
        break;
    }

    case BVM_OP_DIVN:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_num(lhs.num_ / rhs.num_));
        break;
    }

    case BVM_OP_MULN:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_num(lhs.num_ * rhs.num_));
        break;
    }

    case BVM_OP_MODN:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_num(fmod(lhs.num_, rhs.num_)));
        break;
    }

    case BVM_OP_NEGN:
    {
        BvmVal val = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_num(-val.num_));
        break;
    }

    case BVM_OP_CMPN:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        if (lhs.num_ > rhs.num_)
        {
            bvm_local_push(local, bvm_val_int(1));
        }
        else if (lhs.num_ < rhs.num_)
        {
            bvm_local_push(local, bvm_val_int(-1));
        }
        else
        {
            bvm_local_push(local, bvm_val_int(0));
        }
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
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_int(bvm_val_truthy(lhs) && bvm_val_truthy(rhs)));
        break;
    }

    case BVM_OP_OR:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

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
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_uint(lhs.uint_ << rhs.uint_));
        break;
    }

    case BVM_OP_RSHIFT:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_uint(lhs.uint_ >> rhs.uint_));
        break;
    }

    case BVM_OP_BAND:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_uint(lhs.uint_ & rhs.uint_));
        break;
    }

    case BVM_OP_BOR:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_uint(lhs.uint_ | rhs.uint_));
        break;
    }

    case BVM_OP_BXOR:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

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
