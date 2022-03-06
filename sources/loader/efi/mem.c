#include <brutal/debug.h>
#include <efi/lib.h>
#include <loader/loader.h>

Error loader_acquire_pages(size_t count, uintptr_t *ret, LoaderMemFlag flags)
{
    EfiStatus status = 0;

    switch (flags)
    {
    case LOADER_MEM_KERNEL_PAGES:
        status = efi_st()->boot_services->allocate_pages(ALLOCATE_ANY_PAGES, EFI_USER_KERNEL_MEMORY, count, ret);
        break;

    case LOADER_MEM_DATA_PAGES:
        status = efi_st()->boot_services->allocate_pages(ALLOCATE_ANY_PAGES, EFI_BOOT_SERVICES_DATA, count, ret);
        break;

    case LOADER_MEM_ADDR:
        status = efi_st()->boot_services->allocate_pages(ALLOCATE_ADDRESS, EFI_USER_KERNEL_MEMORY, count, ret);
        break;

    default:
        panic$("Invalid flag(s) for memory allocation: {}", (int)flags);
        break;
    }

    if (status != EFI_SUCCESS)
    {
        panic$("Failed to allocate {} bytes of memory: {}", count * 4096, status);
        return ERR_UNDEFINED;
    }

    return ERR_SUCCESS;
}
