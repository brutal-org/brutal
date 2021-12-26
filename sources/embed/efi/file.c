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

MaybeError embed_file_open(EmbedFile *self, Str path)
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

    EfiStatus status = efi_rootdir()->open(efi_rootdir(), &self->proto, cstr, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);

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

MaybeError embed_file_create(EmbedFile *self, Str path)
{
    UNUSED(self);
    UNUSED(path);
    panic$("embed_file_create() not implemented");
}

MaybeError embed_file_close(EmbedFile *self)
{
    _rootdir->close((EfiFileProtocol *)self->proto);
    return SUCCESS;
}

IoResult embed_file_read(EmbedFile *self, uint8_t *data, size_t size)
{
    uint64_t read_write_size = size;
    EfiStatus status = self->proto->read(self->proto, &read_write_size, data);

    if (status != EFI_SUCCESS)
    {
        return ERR(IoResult, ERR_UNDEFINED);
    }
    else
    {
        return OK(IoResult, read_write_size);
    }
}

IoResult embed_file_write(EmbedFile *self, uint8_t const *data, size_t size)
{
    UNUSED(self);
    UNUSED(data);
    UNUSED(size);
    panic$("embed_file_write() not implemented");
}
