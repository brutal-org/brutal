#include <bvm/eval/instr.h>

Str bvm_op_str(BvmOp op)
{
    const char *str[] = {
#define ITER(name) #name,
        BVM_FOREACH_OP
#undef ITER
    };

    if (op < __BVM_OP_COUNT)
    {
        return str$(str[op]);
    }
    else
    {
        return str$("BAD");
    }
}
