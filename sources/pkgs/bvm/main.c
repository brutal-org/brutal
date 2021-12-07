#include <brutal/alloc.h>
#include <bvm/bvm.h>

int main(int argc, char const *argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    BvmMem mem;
    bvm_mem_init(&mem, alloc_global());

    BvmLocal local;
    bvm_local_init(&local, &mem);

    bvm_local_deinit(&local);
    bvm_mem_deinit(&mem);

    return 0;
}