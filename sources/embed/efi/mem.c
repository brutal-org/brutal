#include <brutal/debug.h>
#include <efi/lib.h>
#include <efi/srvs/bs.h>
#include <embed/mem.h>

Error embed_mem_acquire(size_t size, void **out_result, MAYBE_UNUSED enum embed_mem_flag flags)
{
    EfiStatus status = efi_st()->boot_services->allocate_pool(EFI_BOOT_SERVICES_DATA, size, out_result);

    if (status != EFI_SUCCESS)
    {
        panic$("Failed to allocate {} bytes of memory: {}", size, status);
        return ERR_UNDEFINED;
    }

    return ERR_SUCCESS;
}

Error embed_mem_release(void *addr, MAYBE_UNUSED size_t size)
{
    efi_st()->boot_services->free_pool(addr);

    return ERR_SUCCESS;
}

void embed_mem_lock(void)
{
}

void embed_mem_unlock(void)
{
}
