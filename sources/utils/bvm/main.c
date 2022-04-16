#include <bvm/eval.h>
#include <bvm/obj.h>
#include <brutal-alloc>
#include <brutal-debug>

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    BvmGlobal mem;
    bvm_global_init(&mem, alloc_global());

    BvmLocal local;
    bvm_local_init(&local, alloc_global());

    BvmFunc *func = bvm_func_create(alloc_global());

    /* long long int fibb(int n) {
    int fnow = 0, fnext = 1, tempf;
    while(--n>0){
        tempf = fnow + fnext;
        fnow = fnext;
        fnext = tempf;
        }
        return fnext;
}*/
    // int fnow = 0, fnext = 1, tempf;
    int v_fnow = bvm_func_local(func);
    int v_fnext = bvm_func_local(func);
    int v_tempf = bvm_func_local(func);

    bvm_func_emitu(func, BVM_OP_PUSHU, 0);
    bvm_func_emitu(func, BVM_OP_STOREL, v_fnow);

    bvm_func_emitu(func, BVM_OP_PUSHU, 1);
    bvm_func_emitu(func, BVM_OP_STOREL, v_fnext);

    uint64_t jmp_cond = bvm_func_emit(func, BVM_OP_JMP);

    // tempf = fnow + fnext;
    uint64_t loop_start = bvm_func_emitu(func, BVM_OP_LOADL, v_fnow);
    bvm_func_emitu(func, BVM_OP_LOADL, v_fnext);
    bvm_func_emit(func, BVM_OP_ADDI);
    bvm_func_emitu(func, BVM_OP_STOREL, v_tempf);

    // fnow = fnext;
    bvm_func_emitu(func, BVM_OP_LOADL, v_fnext);
    bvm_func_emitu(func, BVM_OP_STOREL, v_fnow);

    // fnext = tempf;
    bvm_func_emitu(func, BVM_OP_LOADL, v_tempf);
    bvm_func_emitu(func, BVM_OP_STOREL, v_fnext);

    // while(--n>0)
    uint64_t cond_start = bvm_func_emitu(func, BVM_OP_LOADA, 0);
    bvm_func_jump(func, jmp_cond, cond_start);
    bvm_func_emiti(func, BVM_OP_PUSHI, 1);
    bvm_func_emit(func, BVM_OP_SUBI);
    bvm_func_emit(func, BVM_OP_DUP);
    bvm_func_emitu(func, BVM_OP_STOREA, 0);

    bvm_func_emiti(func, BVM_OP_PUSHI, 0);
    bvm_func_emit(func, BVM_OP_GTI);
    bvm_func_emit(func, BVM_OP_NOT);

    bvm_func_emitu(func, BVM_OP_JMPF, loop_start);

    // return fnext;
    bvm_func_emitu(func, BVM_OP_LOADL, v_fnext);
    bvm_func_emit(func, BVM_OP_RET);

    bvm_local_push(&local, bvm_val_int(10));
    bvm_call(&local, &mem, func, 1, alloc_global());

    bvm_local_deinit(&local);
    bvm_global_deinit(&mem);

    return 0;
}
