#include <brutal/alloc.h>
#include <brutal/debug/assert.h>
#include <brutal/text/utf16.h>
#include <efi/lib.h>
#include <efi/protos.h>
#include <embed/io.h>

static EFILoadedImage *_image_loader = nullptr;
static EFISimpleFileSystemProtocol *_rootfs = nullptr;
static EfiFileProtocol *_rootdir = nullptr;

static EFILoadedImage *efi_image_loader(void)
{
    if (_image_loader)
    {
        return _image_loader;
    }

    log$("Opening image loader...");

    EfiGuid guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;

    EfiStatus status = efi_st()->boot_services->open_protocol(
        efi_handle(),
        &guid,
        (void **)&_image_loader,
        efi_handle(),
        nullptr,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

    log$("Opening image loader... ok");

    assert_truth(status == EFI_SUCCESS);

    return _image_loader;
}

static EFISimpleFileSystemProtocol *efi_rootfs(void)
{
    if (_rootfs)
    {
        return _rootfs;
    }

    EfiGuid guid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID;

    EfiStatus status = efi_st()->boot_services->open_protocol(
        efi_image_loader()->device_handle,
        &guid,
        (void **)&_rootfs,
        efi_handle(),
        nullptr,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

    assert_truth(status == EFI_SUCCESS);

    return _rootfs;
}

EfiFileProtocol *efi_rootdir(void)
{
    if (_rootdir)
    {
        return _rootdir;
    }

    EfiStatus status = efi_rootfs()->open_volume(efi_rootfs(), &_rootdir);

    assert_truth(status == EFI_SUCCESS);

    return _rootdir;
}

EmbedIoOpenFileResult embed_io_file_open(Str path)
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

    EfiFileProtocol *file = nullptr;

    EfiStatus status = efi_rootdir()->open(efi_rootdir(), &file, cstr, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);

    alloc_free(alloc_global(), cstr);

    if (status != EFI_SUCCESS)
    {
        return ERR(EmbedIoOpenFileResult, ERR_UNDEFINED);
    }
    else
    {
        return OK(EmbedIoOpenFileResult, file);
    }
}

EmbedIoOpenFileResult embed_io_file_create(Str path)
{
    UNUSED(path);
    panic$("embed_io_file_create() not implemented");
}

MaybeError embed_io_file_close(EmbedFile handle)
{
    _rootdir->close((EfiFileProtocol *)handle);
    return SUCCESS;
}

IoResult embed_io_read_file(EmbedFile handle, uint8_t *data, size_t size)
{
    EfiFileProtocol *file = handle;
    uint64_t read_write_size = size;
    EfiStatus status = file->read(file, &read_write_size, data);

    if (status != EFI_SUCCESS)
    {
        return ERR(IoResult, ERR_UNDEFINED);
    }
    else
    {
        return OK(IoResult, read_write_size);
    }
}

IoResult embed_io_write_file(EmbedFile handle, uint8_t const *data, size_t size)
{
    UNUSED(handle);
    UNUSED(data);
    UNUSED(size);
    panic$("embed_io_write_file() not implemented");
}

IoResult embed_io_read_std(IoStdChannel channel, uint8_t *data, size_t size)
{
    UNUSED(channel);
    UNUSED(data);
    UNUSED(size);
    panic$("embed_io_read_std() not implemented");
}

IoResult embed_io_write_std(IoStdChannel channel, uint8_t const *data, size_t size)
{
    assert_not_equal((int)channel, IO_STD_IN);

    uint16_t *cstr = str_to_cstr_utf16_dos(str_n$(size, (char *)data), alloc_global());

    efi_st()->console_out->output_string(efi_st()->console_out, cstr);

    alloc_free(alloc_global(), cstr);

    return OK(IoResult, size);
}
