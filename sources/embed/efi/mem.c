#include <brutal/debug.h>
#include <efi/lib.h>
#include <efi/srvs/bs.h>
#include <embed/mem.h>

Error embed_mem_acquire_pages(size_t count, uintptr_t *ret, enum embed_mem_flag flags)
{
	EfiStatus status = 0;

	switch(flags)
	{
		case EMBED_MEM_USER_PAGES:
			status = efi_st()->boot_services->allocate_pages(ALLOCATE_ANY_PAGES, EFI_USER_KERNEL_MEMORY, count, ret);
			break;

		case EMBED_MEM_DATA_PAGES:
			status = efi_st()->boot_services->allocate_pages(ALLOCATE_ANY_PAGES, EFI_BOOT_SERVICES_DATA, count, ret);
			break;

		case EMBED_MEM_ADDR:
			status = efi_st()->boot_services->allocate_pages(ALLOCATE_ADDRESS, EFI_USER_KERNEL_MEMORY, count, ret);
			break;

		default:
			break;
	}

    if (status != EFI_SUCCESS)
    {
        panic$("Failed to allocate {} bytes of memory: {}", count*4096, status);
        return ERR_UNDEFINED;
    }

	return ERR_SUCCESS;
}

Error embed_mem_acquire(size_t size, void **out_result, MAYBE_UNUSED enum embed_mem_flag flags)
{
	EfiStatus status = 0;

	if(flags == EMBED_MEM_NONE)
	{
    	status = efi_st()->boot_services->allocate_pool(EFI_BOOT_SERVICES_DATA, size, out_result);
	}


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



