#include <brutal/alloc/global.h>
#include <brutal/log/assert.h>
#include <brutal/text/utf16.h>
#include <efi/lib.h>
#include <efi/protos.h>
#include <host/io.h>

static EFILoadedImage *image_loader = nullptr;
static EFISimpleFileSystemProtocol *rootfs = nullptr;
static EFIFileProtocol *rootdir = nullptr;

static EFILoadedImage *efi_image_loader(void)
{
    if (image_loader)
    {
        return image_loader;
    }

    log$("Opening image loader...");

    EFIGUID guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;

    EFIStatus status = efi_st()->boot_services->open_protocol(
        efi_handle(),
        &guid,
        (void **)&image_loader,
        efi_handle(),
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

    log$("Opening image loader... ok");

    assert_truth(status == EFI_SUCCESS);

    return image_loader;
}

static EFISimpleFileSystemProtocol *efi_rootfs(void)
{
    if (rootfs)
    {
        return rootfs;
    }

    log$("Opening rootfs...");

    EFIGUID guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

    EFIStatus status = efi_st()->boot_services->open_protocol(
        efi_image_loader()->device_handle,
        &guid,
        (void **)&rootfs,
        efi_handle(),
        NULL,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

    log$("Opening rootfs...ok");

    assert_truth(status == EFI_SUCCESS);

    return rootfs;
}

EFIFileProtocol *efi_rootdir(void)
{
    if (rootdir)
    {
        return rootdir;
    }

    log$("Opening rootdir...");

    EFIStatus status = efi_rootfs()->open_volume(efi_rootfs(), &rootdir);

    log$("Opening rootdir...ok");

    assert_truth(status == EFI_SUCCESS);

    return rootdir;
}

HostIoOpenFileResult host_io_file_open(Str path)
{
    uint16_t *cstr = str_to_cstr_utf16(path, alloc_global());

    // HACK: EFI expect \ instead of /.
    for (size_t i = 0; cstr[i]; i++)
    {
        if (cstr[i] == u'/')
        {
            cstr[i] = u'\\';
        }
    }

    EFIFileProtocol *file = nullptr;

    EFIStatus status = efi_rootdir()->open(efi_rootdir(), &file, cstr, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);

    alloc_free(alloc_global(), cstr);

    if (status != EFI_SUCCESS)
    {
        assert_unreachable();
        return ERR(HostIoOpenFileResult, ERR_UNDEFINED);
    }
    else
    {
        return OK(HostIoOpenFileResult, file);
    }
}

HostIoOpenFileResult host_io_file_create(Str path)
{
    UNUSED(path);
    assert_unreachable();
}

MaybeError host_io_file_close(HostIoFileHandle handle)
{
    rootdir->close((EFIFileProtocol *)handle);
    return SUCCESS;
}

IoReadResult host_io_read_file(HostIoFileHandle handle, uint8_t *data, size_t size)
{
    EFIFileProtocol *file = handle;
    uint64_t read_write_size = size;
    EFIStatus status = file->read(file, &read_write_size, data);

    if (status != EFI_SUCCESS)
    {
        return ERR(IoReadResult, ERR_UNDEFINED);
    }
    else
    {
        return OK(IoReadResult, read_write_size);
    }
}

IoWriteResult host_io_write_file(HostIoFileHandle handle, uint8_t const *data, size_t size)
{
    UNUSED(handle);
    UNUSED(data);
    UNUSED(size);
    assert_unreachable();
}

IoReadResult host_io_read_std(IoStdChannel channel, uint8_t *data, size_t size)
{
    UNUSED(channel);
    UNUSED(data);
    UNUSED(size);
    assert_unreachable();
}

IoWriteResult host_io_write_std(IoStdChannel channel, uint8_t const *data, size_t size)
{
    assert_not_equal((int)channel, IO_STD_IN);

    uint16_t *cstr = str_to_cstr_utf16_dos(str_n$(size, (char *)data), alloc_global());

    efi_st()->console_out->output_string(efi_st()->console_out, cstr);

    alloc_free(alloc_global(), cstr);

    return OK(IoWriteResult, size);
}
