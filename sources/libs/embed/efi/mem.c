#include <brutal-alloc>
#include <brutal-debug>
#include <brutal-sync>
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

static HeapAlloc _heap;

static Alloc *ensure_heap(void)
{
    static bool init = false;
    if (!init)
    {
        heap_alloc_init(&_heap, NODE_DEFAULT);
        init = true;
    }
    return (Alloc *)&_heap;
}

void embed_heap_lock(void)
{
}

void embed_heap_unlock(void)
{
}

void *embed_heap_acquire(size_t size)
{
    return alloc_acquire(ensure_heap(), size);
}

void *embed_heap_resize(void *ptr, size_t size)
{
    return alloc_resize(ensure_heap(), ptr, size);
}

void embed_heap_release(void *ptr)
{
    alloc_release(ensure_heap(), ptr);
}
