#include <brutal/alloc.h>
#include <brutal/debug/assert.h>
#include <brutal/text/utf16.h>
#include <efi/lib.h>
#include <efi/protos.h>
#include <embed/file.h>

static EFILoadedImage *_image_loader = nullptr;
static EFISimpleFileSystemProtocol *_rootfs = nullptr;
static EfiFileProtocol *_rootdir = nullptr;

static EFILoadedImage *efi_image_loader(void)
{
    if (_image_loader)
    {
        return _image_loader;
    }

    EfiGuid guid = EFI_LOADED_IMAGE_PROTOCOL_GUID;

    EfiStatus status = efi_st()->boot_services->open_protocol(
        efi_handle(),
        &guid,
        (void **)&_image_loader,
        efi_handle(),
        nullptr,
        EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

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

int embed_flags2efi_flags(FileOpenFlags flags)
{
    switch(flags)
    {
        case FILE_OPEN_READ_ONLY:
            return EFI_FILE_MODE_READ;
        case FILE_OPEN_WRITE_ONLY:
            return EFI_FILE_MODE_WRITE;
        case FILE_OPEN_READ_WRITE:
            return EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE;
        default:
            panic$("unkown embed flag: {}", flags);
    }
}

MaybeError embed_file_open(IoFile *self, Str path, FileOpenFlags flags)
{
    uint16_t *cstr = utf16_str_to_cstr(path, alloc_global());

    // HACK: EFI expect \ instead of /.
    for (size_t i = 0; cstr[i]; i++)
    {
        if (cstr[i] == u'/')
        {
            cstr[i] = u'\\';
        }
    }

    EfiStatus status = efi_rootdir()->open(
        efi_rootdir(),
        &self->embed.proto,
        cstr,
        embed_flags2efi_flags(flags),
        (flags ==FILE_OPEN_READ_ONLY ) ? EFI_FILE_READ_ONLY : 0);

    alloc_free(alloc_global(), cstr);

    if (status != EFI_SUCCESS)
    {
        return ERR(MaybeError, ERR_UNDEFINED);
    }
    else
    {
        return SUCCESS;
    }
}

MaybeError embed_file_create(IoFile *self, Str path)
{
    UNUSED(self);
    UNUSED(path);
    panic$("embed_file_create() not implemented");
}

MaybeError embed_file_close(IoFile *self)
{
    _rootdir->close((EfiFileProtocol *)self->embed.proto);
    return SUCCESS;
}

IoResult embed_file_read(IoFile *self, uint8_t *data, size_t size)
{
    uint64_t read_write_size = size;
    EfiStatus status = self->embed.proto->read(self->embed.proto, &read_write_size, data);

    if (status != EFI_SUCCESS)
    {
        return ERR(IoResult, ERR_UNDEFINED);
    }
    else
    {
        return OK(IoResult, read_write_size);
    }
}

IoResult embed_file_write(IoFile *self, uint8_t const *data, size_t size)
{
    UNUSED(self);
    UNUSED(data);
    UNUSED(size);
    panic$("embed_file_write() not implemented");
}

IoResult embed_file_seek(IoFile *self, IoSeek seek)
{
    UNUSED(self);
    UNUSED(seek);
    panic$("embed_file_seek() not implemented");
}
