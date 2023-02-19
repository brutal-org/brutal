#include <bvm/eval/eval.h>
#include <bvm/obj/func.h>
#include <bvm/obj/obj.h>
#include <bvm/obj/val.h>
#include <math.h>
#include <brutal-debug>

BvmVal bvm_call(BvmLocal *local, BvmGlobal *global, BvmFunc *func, int argc, Alloc *alloc)
{
    bvm_local_call(local, func, argc);

    while (bvm_eval(local, global, alloc) == BVM_RES_RUN)
        ;

    return bvm_local_pop(local);
}

BvmRes bvm_eval(BvmLocal *local, BvmGlobal *global, Alloc *alloc)
{
    BvmInstr instr = bvm_local_ifetch(local);

    switch (instr.opcode)
    {
    case BVM_OP_NOP:
        break;

    case BVM_OP_LOADG:
        bvm_local_push(local, bvm_global_load(global, instr.uarg));
        break;

    case BVM_OP_LOADL:
        bvm_local_push(local, bvm_local_load_var(local, instr.uarg));
        break;

    case BVM_OP_LOADA:
        bvm_local_push(local, bvm_local_load_arg(local, instr.uarg));
        break;

    case BVM_OP_LOADO:
    {
        BvmVal val = bvm_local_pop(local);

        if (val.type != BVM_OBJ)
        {
            bvm_local_push(local, bvm_val_undef());
            break;
        }

        bvm_local_push(local, bvm_obj_load(val.obj_, instr.uarg));
        break;
    }

    case BVM_OP_LOADV:
    {
        BvmVal val = bvm_local_pop(local);

        if (val.type != BVM_OBJ)
        {
            bvm_local_push(local, bvm_val_undef());
            break;
        }

        bvm_local_push(local, bvm_obj_loadv(val.obj_, instr.uarg));
        break;
    }

    case BVM_OP_STOREG:
        bvm_global_store(global, instr.uarg, bvm_local_pop(local));
        break;

    case BVM_OP_STOREL:
        bvm_local_store_var(local, instr.uarg, bvm_local_pop(local));
        break;

    case BVM_OP_STOREA:
        bvm_local_store_arg(local, instr.uarg, bvm_local_pop(local));
        break;

    case BVM_OP_STOREO:
    {
        BvmVal obj_val = bvm_local_pop(local);
        BvmVal val = bvm_local_pop(local);

        if (obj_val.type != BVM_OBJ)
        {
            break;
        }

        bvm_obj_store(obj_val.obj_, instr.uarg, val);
        break;
    }

    case BVM_OP_STOREV:
    {
        BvmVal obj_val = bvm_local_pop(local);
        BvmVal val = bvm_local_pop(local);

        if (obj_val.type != BVM_OBJ)
        {
            break;
        }

        bvm_obj_storev(obj_val.obj_, instr.uarg, val);
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
            bvm_local_ret(local, val.func_->native_(local, global));
        }
        break;
    }

    case BVM_OP_NEW:
    {
        BvmVal val = bvm_local_pop(local);

        if (val.type != BVM_OBJ)
        {
            bvm_local_push(local, bvm_val_nil());
        }
        else
        {
            BvmObj *obj = bvm_obj_create(instr.uarg, val.obj_, alloc);
            BvmVal obj_val = bvm_val_obj(obj);
            bvm_local_push(local, obj_val);
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

    case BVM_OP_SUPER:
    {
        BvmVal val = bvm_local_pop(local);

        if (val.type == BVM_OBJ && val.obj_)
        {
            bvm_local_push(local, bvm_val_obj(val.obj_->proto));
        }
        else
        {
            bvm_local_push(local, bvm_val_undef());
        }

        break;
    }

    case BVM_OP_ISNIL:
        bvm_local_push(local, bvm_val_int(bvm_local_pop(local).type == BVM_NIL));
        break;

    case BVM_OP_ISUNDEF:
        bvm_local_push(local, bvm_val_int(bvm_local_pop(local).type == BVM_UNDEF));
        break;

    case BVM_OP_ISINT:
        bvm_local_push(local, bvm_val_int(bvm_local_pop(local).type == BVM_INT));
        break;

    case BVM_OP_ISUINT:
        bvm_local_push(local, bvm_val_int(bvm_local_pop(local).type == BVM_UINT));
        break;

    case BVM_OP_ISNUM:
        bvm_local_push(local, bvm_val_int(bvm_local_pop(local).type == BVM_NUM));
        break;

    case BVM_OP_ISOBJ:
        bvm_local_push(local, bvm_val_int(bvm_local_pop(local).type == BVM_OBJ));
        break;

    case BVM_OP_ISFUNC:
        bvm_local_push(local, bvm_val_int(bvm_local_pop(local).type == BVM_FUNC));
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
