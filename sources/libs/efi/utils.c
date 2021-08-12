#include "utils.h"
#include <brutal/alloc.h>
#include <brutal/host/mem.h>
#include "brutal/base/error.h"
#include "efi/file.h"
#include "efi/lip.h"
#include "efi/st.h"
#include "misc.h"

EFISystemTable *st;
EFIHandle image_handle;

static EFIFileProtocol *rootdir;
static EFISimpleFileSystemProtocol *rootfs;

static File current_file;

void init_lib(EFISystemTable *st2, EFIHandle image_handle2)
{
    st = st2;
    image_handle = image_handle2;
}

static EFIStatus get_rootdir(EFISimpleFileSystemProtocol *rootfs, EFIFileProtocol **rootdir)
{
    return rootfs->open_volume(rootfs, rootdir);
}

static EFIStatus get_rootfs(EFIHandle loader, EFISystemTable *system, EFIHandle device, EFISimpleFileSystemProtocol **rootfs)
{
    EFIGUID guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

    return system->boot_services->open_protocol(
        device,
        &guid,
        (void **)rootfs,
        loader,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
}

static EFIStatus get_loader_image(EFIHandle loader, EFISystemTable *system, EFILoadedImage **image)
{
    EFIGUID guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;

    return system->boot_services->open_protocol(
        loader,
        &guid,
        (void **)image,
        loader,
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);
}

File open_file(u16 *path)
{
    File f;

    f.status = 0;

    EFILoadedImage *image;

    get_loader_image(image_handle, st, &image);
    get_rootfs(image_handle, st, image->device_handle, &rootfs);
    get_rootdir(rootfs, &rootdir);

    /* Open the file as readonly */
    f.status = rootdir->open(rootdir, &f.efi_file, path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);

    current_file = f;

    return f;
}

EFIFileInfo get_file_info(File *file)
{

    EFIGUID guid = EFI_FILE_INFO_ID;
    u64 info_size = sizeof(file->info);

    file->efi_file->get_info(file->efi_file, &guid, &info_size, &file->info);

    return file->info;
}

void read_file(File *file, void *buffer)
{

    u64 file_size = file->info.file_size;

    file->buffer = buffer;

    file->status = file->efi_file->read(file->efi_file, &file_size, buffer);
}

void close_file(File *file)
{

    file->status = rootdir->close(file->efi_file);
}

void efi_printf(char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    u16 buffer[1024];

    u16vsnprintf(buffer, sizeof(buffer), fmt, args);
    st->console_out->output_string(st->console_out, buffer);

    va_end(args);
}

Error host_mem_acquire(size_t size, void **out_result, MAYBE_UNUSED enum host_mem_flag flags)
{
    auto status = st->boot_services->allocate_pool(EFI_BOOT_SERVICES_DATA, size, out_result);

    if (status != EFI_SUCCESS)
    {
        return ERR_UNDEFINED;
    }

    return ERR_SUCCESS;
}

Error host_mem_release(void *addr, MAYBE_UNUSED size_t size)
{
    auto status = st->boot_services->free_pool(addr);

    if (status != EFI_SUCCESS)
    {
        return ERR_UNDEFINED;
    }

    return ERR_SUCCESS;
}

void host_mem_lock(void)
{
    return;
}

void host_mem_unlock(void)
{
    return;
}

void *to_utf16(void *ptr, char *buffer)
{
    ptr = efi_malloc(strlen(buffer) << 1);

    memset(ptr, 0, sizeof(ptr));
    for (size_t i = 0; i < sizeof(buffer) / sizeof(*buffer); i++)
    {
        *(char16 *)(ptr + (i << 1)) = buffer[i];
    }

    return ptr;
}
