#include <efi/lib.h>
#include <brutal-alloc>
#include <brutal-debug>
#include <brutal-io>

static bool _init = false;
static EFIHandle _handle;
static EFISystemTable *_st;
static uint64_t _mmap_key = 0;

void efi_init(EFIHandle handle, EFISystemTable *st)
{
    assert_truth(!_init);
    _handle = handle;
    _st = st;

    _init = true;
}

EFIHandle efi_handle(void)
{
    assert_truth(_init);
    return _handle;
}

EFISystemTable *efi_st(void)
{
    assert_truth(_init);
    return _st;
}

void efi_deinit(void)
{
    efi_st()->boot_services->exit_boot_services(efi_handle(), _mmap_key);
}

EFIMemoryDescriptor *efi_mmap_snapshot(size_t *size, size_t *desc_size)
{
    uint64_t mmap_size = 0;
    uint64_t descriptor_size = 0;
    uint32_t descriptor_version = 0;

    efi_st()->boot_services->get_memory_map(&mmap_size, nullptr, &_mmap_key, &descriptor_size, &descriptor_version);

    // Allocating the pool creates at least one new descriptor... for the chunk of memory changed to EfiLoaderData
    // Not sure that UEFI firmware must allocate on a memory type boundry... if not, then two descriptors might be created
    // https://stackoverflow.com/questions/39407280/uefi-simple-example-of-using-exitbootservices-with-gnu-efi
    mmap_size += 2 * descriptor_size;

    EFIMemoryDescriptor *mmap = alloc_malloc(alloc_global(), mmap_size);

    efi_st()->boot_services->get_memory_map(&mmap_size, mmap, &_mmap_key, &descriptor_size, &descriptor_version);

    *size = (mmap_size / descriptor_size);
    *desc_size = descriptor_size;

    return mmap;
}
