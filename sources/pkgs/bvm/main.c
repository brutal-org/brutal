#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <bvm/builder.h>
#include <bvm/bvm.h>

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    BvmMem mem;
    bvm_mem_init(&mem, alloc_global());

    BvmLocal local;
    bvm_local_init(&local, alloc_global());

    BvmSig *sig = bvm_sig_create(nullptr, false, alloc_global());

    BvmFunc *func = bvm_func_create(sig, alloc_global());

    bvm_func_emit(func, BVM_OP_ARGC);
    bvm_func_emit(func, BVM_OP_ARGC);
    bvm_func_emit(func, BVM_OP_ADDI);
    bvm_func_emit(func, BVM_OP_HALT);

    bvm_local_call(&local, func, 0);

    while (bvm_eval(&local, &mem) == BVM_RES_RUN)
        ;

    bvm_local_deinit(&local);
    bvm_mem_deinit(&mem);

    return 0;
}
