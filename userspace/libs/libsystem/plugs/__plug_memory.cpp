#include <abi/Syscalls.h>

#include <abi/Result.h>
#include <libsystem/system/Memory.h>

HjResult memory_alloc(size_t size, uintptr_t *out_address)
{
    return hj_memory_alloc(size, out_address);
}

HjResult memory_free(uintptr_t address)
{
    return hj_memory_free(address);
}

HjResult memory_include(int handle, uintptr_t *out_address, size_t *out_size)
{
    return hj_memory_include(handle, out_address, out_size);
}

HjResult memory_get_handle(uintptr_t address, int *out_handle)
{
    return hj_memory_get_handle(address, out_handle);
}
