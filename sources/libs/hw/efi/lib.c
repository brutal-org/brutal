#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <brutal/io.h>
#include <efi/lib.h>

static bool _init = false;
static EFIHandle _handle;
static EFISystemTable *_st;

static uint64_t latest_map_key = 0;

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
    efi_st()->boot_services->exit_boot_services(efi_handle(), latest_map_key);
}

EFIMemoryDescriptor *get_efi_memory_map(size_t *size, size_t *desc_size)
{
    uint8_t tmp_mmap[1];
    uint64_t mmap_size = sizeof(tmp_mmap);
    uint64_t map_key = 0;
    uint64_t descriptor_size = 0;
    uint32_t descriptor_version = 0;

    efi_st()->boot_services->get_memory_map(&mmap_size, (EFIMemoryDescriptor *)tmp_mmap, &map_key, &descriptor_size, &descriptor_version);

    mmap_size += 4096;

    EFIMemoryDescriptor *mmap = alloc_malloc(alloc_global(), mmap_size);

    efi_st()->boot_services->get_memory_map(&mmap_size, mmap, &map_key, &descriptor_size, &descriptor_version);

    uint64_t entry_count = (mmap_size / descriptor_size);
    *size = entry_count;
    *desc_size = descriptor_size;

    latest_map_key = map_key;

    return mmap;
}
