#include <brutal/alloc.h>
#include <brutal/text/utf8.h>
#include <embed/file.h>
#include <embed/posix/err.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

MaybeError embed_file_open(EmbedFile *self, Str path)
{
    char *cstr = (char *)str_to_cstr_utf8(path, alloc_global());

    *self = open(cstr, O_RDONLY);

    alloc_free(alloc_global(), cstr);

    if (*self == -1)
    {
        Error error = error_from_errno();
        return ERR(MaybeError, error);
    }

    return SUCCESS;
}

MaybeError embed_file_create(EmbedFile *self, Str path)
{
    char *cstr = (char *)str_to_cstr_utf8(path, alloc_global());

    *self = open(cstr, O_CREAT | O_WRONLY | O_TRUNC, 0666);

    alloc_free(alloc_global(), cstr);

    if (*self == -1)
    {
        Error error = error_from_errno();
        return ERR(MaybeError, error);
    }

    return SUCCESS;
}

MaybeError embed_file_close(EmbedFile *self)
{
    int result = close(*self);

    if (result == -1)
    {
        Error error = error_from_errno();
        return ERR(MaybeError, error);
    }

    return SUCCESS;
}

IoResult embed_file_read(EmbedFile *self, uint8_t *data, size_t size)
{
    ssize_t result = read(*self, data, size);

    if (result == -1)
    {
        Error error = error_from_errno();
        return ERR(IoResult, error);
    }

    return OK(IoResult, result);
}

IoResult embed_file_write(EmbedFile *self, uint8_t const *data, size_t size)
{
    ssize_t result = write(*self, data, size);

    if (result == -1)
    {
        Error error = error_from_errno();
        return ERR(IoResult, error);
    }

    return OK(IoResult, result);
}
