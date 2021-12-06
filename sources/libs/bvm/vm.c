#include <bvm/vm.h>

bool bvm_eval(Bvm *self)
{
    while (self->alive)
    {
        uint8_t opcode = buf_load_u8(&self->code, self->ip++);

        switch (opcode)
        {
        case BVM_OP_NOP:
            break;

        case BVM_OP_LOADL:
            break;

        case BVM_OP_LOADG:
            break;

        case BVM_OP_LOADO:
            break;

        case BVM_OP_STOREL:
            break;

        case BVM_OP_STOREG:
            break;

        case BVM_OP_STOREO:
            break;

        case BVM_OP_CALL:
            break;

        case BVM_OP_RET:
            break;

        case BVM_OP_HALT:
            break;

        case BVM_OP_JMP:
            break;

        case BVM_OP_JMPF:
            break;

        case BVM_OP_ADD:
            break;

        case BVM_OP_SUB:
            break;

        case BVM_OP_DIV:
            break;

        case BVM_OP_MUL:
            break;

        case BVM_OP_MOD:
            break;

        case BVM_OP_NEG:
            break;

        case BVM_OP_GT:
            break;

        case BVM_OP_EQ:
            break;

        case BVM_OP_GEQ:
            break;

        case BVM_OP_NEQ:
            break;

        case BVM_OP_ISA:
            break;

        case BVM_OP_AND:
            break;

        case BVM_OP_OR:
            break;

        case BVM_OP_NOT:
            break;

        case BVM_OP_LSHIFT:
            break;

        case BVM_OP_RSHIFT:
            break;

        case BVM_OP_BAND:
            break;

        case BVM_OP_BOR:
            break;

        case BVM_OP_BXOR:
            break;

        case BVM_OP_BNOT:
            break;

        case BVM_OP_NEW:
            break;
        }
    }
}
