#include <embed/file.h>
#include <embed/posix/err.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <brutal-alloc>
#include <brutal-debug>
#include <brutal-text/utf8.h>

int embed_flags2posix_flags(FileInitFlags flags)
{
    int result = 0;
    switch (flags & IO_FILE_READ_WRITE)
    {
    case IO_FILE_READ_WRITE:
        result = O_RDWR;
        break;
    case IO_FILE_WRITE:
        result = O_WRONLY;
        break;
    case IO_FILE_READ:
        result = O_RDONLY;
        break;
    default:
        panic$("unkown flag: {#x}", flags);
    }

    if (flags & IO_FILE_CREATE)
    {
        result |= O_CREAT | O_TRUNC;
    }

    return result;
}

MaybeError embed_file_open(IoFile *self, Str path, FileInitFlags flags)
{
    char *cstr = (char *)utf8_str_to_cstr(path, alloc_global());

    self->embed.fd = open(cstr, embed_flags2posix_flags(flags));

    alloc_free(alloc_global(), cstr);

    if (self->embed.fd == -1)
    {
        Error error = error_from_errno();
        return ERR(MaybeError, error);
    }

    return SUCCESS;
}

MaybeError embed_file_create(IoFile *self, Str path)
{
    char *cstr = (char *)utf8_str_to_cstr(path, alloc_global());

    self->embed.fd = open(cstr, O_CREAT | O_WRONLY | O_TRUNC, 0666);

    alloc_free(alloc_global(), cstr);

    if (self->embed.fd == -1)
    {
        Error error = error_from_errno();
        return ERR(MaybeError, error);
    }

    return SUCCESS;
}

MaybeError embed_file_close(IoFile *self)
{
    int result = close(self->embed.fd);

    if (result == -1)
    {
        Error error = error_from_errno();
        return ERR(MaybeError, error);
    }

    return SUCCESS;
}

int io_seek_to_whence(IoSeek seek)
{
    switch (seek.whence)
    {
    case IO_WHENCE_START:
        return SEEK_SET;
    case IO_WHENCE_CURRENT:
        return SEEK_CUR;
    case IO_WHENCE_END:
        return SEEK_END;
    default:
        panic$("Unknow whence {}", seek.whence);
    }
}

IoResult embed_file_seek(IoFile *self, IoSeek seek)
{
    ssize_t result = lseek(self->embed.fd, seek.position, io_seek_to_whence(seek));

    if (result == -1)
    {
        Error error = error_from_errno();
        return ERR(IoResult, error);
    }

    return OK(IoResult, result);
}

IoResult embed_file_read(IoFile *self, uint8_t *data, size_t size)
{
    ssize_t result = read(self->embed.fd, data, size);

    if (result == -1)
    {
        Error error = error_from_errno();
        return ERR(IoResult, error);
    }

    return OK(IoResult, result);
}

IoResult embed_file_write(IoFile *self, uint8_t const *data, size_t size)
{
    ssize_t result = write(self->embed.fd, data, size);

    if (result == -1)
    {
        Error error = error_from_errno();
        return ERR(IoResult, error);
    }

    return OK(IoResult, result);
}
