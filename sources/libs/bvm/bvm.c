#include <brutal/debug.h>
#include <bvm/bvm.h>

BvmRes bvm_eval(BvmLocal *local, BvmMem *mem)
{

    BvmInstr instr = bvm_mem_ifetch(mem, local->ip++);

    log$("{}:{x}(uh:{08x}, s{08x})", bvm_op_str(instr.opcode), instr.opcode, load_le(instr.uarg), load_le(instr.iarg));

    switch (instr.opcode)
    {
    case BVM_OP_NOP:
        break;

    case BVM_OP_LOADG:
        bvm_local_push(local, bvm_mem_load(mem, load_le(instr.uarg)));
        break;

    case BVM_OP_LOADL:
        bvm_local_push(local, bvm_local_load(local, load_le(instr.uarg)));
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
        bvm_local_store(local, load_le(instr.uarg), bvm_local_pop(local));
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
        assert_truth(val.type == BVM_VAL_FUNC);

        BvmFrame *frame = bvm_local_call(local, load_le(instr.uarg));
        BvmFunc *func = val.func_;

        if (func->native)
        {
            func->native_(local, frame, mem);
        }
        else
        {
            local->ip = func->managed_.entry;
        }

        break;
    }

    case BVM_OP_NEW:
        break;

    case BVM_OP_RET:
    {
        local->ip = bvm_local_ret(local);
        break;
    }

    case BVM_OP_HALT:
        return BVM_RES_HALT;

    case BVM_OP_JMP:
        local->ip += load_le(instr.iarg);
        break;

    case BVM_OP_JMPF:
    {
        BvmVal val = bvm_local_pop(local);
        uint16_t off = load_le(instr.iarg);

        if (!bvm_val_truthy(val))
        {
            local->ip += off;
        }
    }
    break;

    case BVM_OP_ADD:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_add(lhs, rhs));
    }
    break;

    case BVM_OP_SUB:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_sub(lhs, rhs));
    }
    break;

    case BVM_OP_DIV:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_div(lhs, rhs));
    }
    break;

    case BVM_OP_MUL:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_mul(lhs, rhs));
    }
    break;

    case BVM_OP_MOD:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_mod(lhs, rhs));
    }
    break;

    case BVM_OP_NEG:
    {
        BvmVal val = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_neg(val));
    }
    break;

    case BVM_OP_GT:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_gt(lhs, rhs));
    }
    break;

    case BVM_OP_EQ:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_eq(lhs, rhs));
    }
    break;

    case BVM_OP_GEQ:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_geq(lhs, rhs));
    }
    break;

    case BVM_OP_NEQ:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_neq(lhs, rhs));
    }
    break;

    case BVM_OP_ISA:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_isa(lhs, rhs));
    }
    break;

    case BVM_OP_AND:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_isa(lhs, rhs));
    }
    break;

    case BVM_OP_OR:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_or(lhs, rhs));
    }
    break;

    case BVM_OP_NOT:
    {
        BvmVal val = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_not(val));
    }
    break;

    case BVM_OP_LSHIFT:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_lshift(lhs, rhs));
    }
    break;

    case BVM_OP_RSHIFT:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_rshift(lhs, rhs));
    }
    break;

    case BVM_OP_BAND:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_band(lhs, rhs));
    }
    break;

    case BVM_OP_BOR:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_bor(lhs, rhs));
    }
    break;

    case BVM_OP_BXOR:
    {
        BvmVal lhs = bvm_local_pop(local);
        BvmVal rhs = bvm_local_pop(local);

        bvm_local_push(local, bvm_val_bxor(lhs, rhs));
    }
    break;

    case BVM_OP_BNOT:
    {
        BvmVal val = bvm_local_pop(local);
        bvm_local_push(local, bvm_val_bnot(val));
    }
    break;

    default:
        return BVM_RES_ERROR;
    }

    return BVM_RES_RUN;
}
