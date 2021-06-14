#include <brutal/base.h>
#include <host/mem.h>
#include <sys/mman.h>

void host_mem_lock(void)
{
    // FIXME: no-op
}

void host_mem_unlock(void)
{
    // FIXME: no-op
}

error_t host_mem_acquire(size_t size, void **out_result, MAYBE_UNUSED enum host_mem_flag flags)
{
    *out_result = mmap(nullptr, size, PROT_READ | PROT_WRITE, 0, -1, 0);
    return ERR_SUCCESS; // FIXME: handle error
}

error_t host_mem_commit(MAYBE_UNUSED void *addr, MAYBE_UNUSED size_t size)
{
    // no-op
    return ERR_SUCCESS;
}

error_t host_mem_decommit(MAYBE_UNUSED void *addr, MAYBE_UNUSED size_t size)
{
    // no-op
    return ERR_SUCCESS;
}

error_t host_mem_release(void *addr, size_t size)
{
    munmap(addr, size);
    return ERR_SUCCESS; // FIXME: handle error
}
