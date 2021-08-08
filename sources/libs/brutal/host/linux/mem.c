#include <brutal/host/mem.h>
#include <brutal/log.h>
#include <errno.h>
#include <string.h>
#include <sys/mman.h>

Error error_from_errno(void)
{
    if (errno == 0)
    {
        return ERR_SUCCESS;
    }
    else
    {
        log("{}", strerror(errno));
        return make_error(ERR_KIND_UNDEFINED, strerror(errno));
    }
}

void host_mem_lock(void)
{
    // FIXME: no-op
}

void host_mem_unlock(void)
{
    // FIXME: no-op
}

Error host_mem_acquire(size_t size, void **out_result, MAYBE_UNUSED enum host_mem_flag flags)
{
    *out_result = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    return error_from_errno();
}

Error host_mem_release(void *addr, size_t size)
{
    munmap(addr, size);
    return error_from_errno();
}
