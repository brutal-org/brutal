#include <brutal/alloc.h>
#include <brutal/debug/assert.h>
#include <brutal/text/utf8.h>
#include <embed/io.h>
#include <embed/posix/err.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

EmbedIoOpenFileResult embed_io_file_open(Str path)
{
    char *cstr = (char *)str_to_cstr_utf8(path, alloc_global());

    EmbedFile handle = open(cstr, O_RDONLY);

    alloc_free(alloc_global(), cstr);

    if (handle == -1)
    {
        Error error = error_from_errno();
        return ERR(EmbedIoOpenFileResult, error);
    }

    return OK(EmbedIoOpenFileResult, handle);
}

EmbedIoOpenFileResult embed_io_file_create(Str path)
{
    char *cstr = (char *)str_to_cstr_utf8(path, alloc_global());

    EmbedFile handle = open(cstr, O_CREAT | O_WRONLY | O_TRUNC, 0666);

    alloc_free(alloc_global(), cstr);

    if (handle == -1)
    {
        Error error = error_from_errno();
        return ERR(EmbedIoOpenFileResult, error);
    }

    return OK(EmbedIoOpenFileResult, handle);
}

MaybeError embed_io_file_close(EmbedFile handle)
{
    int result = close(handle);

    if (result == -1)
    {
        Error error = error_from_errno();
        return ERR(MaybeError, error);
    }

    return SUCCESS;
}

IoResult embed_io_read_file(EmbedFile handle, uint8_t *data, size_t size)
{
    ssize_t result = read(handle, data, size);

    if (result == -1)
    {
        Error error = error_from_errno();
        return ERR(IoResult, error);
    }

    return OK(IoResult, result);
}

IoResult embed_io_write_file(EmbedFile handle, uint8_t const *data, size_t size)
{
    ssize_t result = write(handle, data, size);

    if (result == -1)
    {
        Error error = error_from_errno();
        return ERR(IoResult, error);
    }

    return OK(IoResult, result);
}

IoResult embed_io_read_std(IoStdChannel channel, uint8_t *data, size_t size)
{
    assert_equal((int)channel, IO_STD_IN);

    ssize_t result = read(0, data, size);

    if (result == -1)
    {
        Error error = error_from_errno();
        return ERR(IoResult, error);
    }

    return OK(IoResult, result);
}

IoResult embed_io_write_std(IoStdChannel channel, uint8_t const *data, size_t size)
{
    ssize_t result = write((int)channel, data, size);

    if (result == -1)
    {
        Error error = error_from_errno();
        return ERR(IoResult, error);
    }

    return OK(IoResult, result);
}
