#include <brutal/alloc.h>
#include <brutal/host/io.h>
#include <brutal/log/assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

HostIoOpenFileResult host_io_file_open(Str path)
{
    char *cstr = (char *)alloc_malloc(alloc_global(), path.len + 1);

    mem_cpy(cstr, path.buffer, path.len);
    cstr[path.len] = '\0';

    HostIoFileHandle handle = open(cstr, O_RDONLY);

    alloc_free(alloc_global(), cstr);
    return OK(HostIoOpenFileResult, handle);
}

HostIoOpenFileResult host_io_file_create(Str path)
{
    char *cstr = (char *)alloc_malloc(alloc_global(), path.len + 1);

    mem_cpy(cstr, path.buffer, path.len);
    cstr[path.len] = '\0';

    HostIoFileHandle handle = open(cstr, O_CREAT | O_WRONLY | O_TRUNC, 0666);

    alloc_free(alloc_global(), cstr);
    return OK(HostIoOpenFileResult, handle);
}

MaybeError host_io_file_close(HostIoFileHandle handle)
{
    close(handle);
    return SUCCESS;
}

IoReadResult host_io_read_file(HostIoFileHandle handle, char *data, size_t size)
{
    return OK(IoReadResult, read(handle, data, size));
}

IoWriteResult host_io_write_file(HostIoFileHandle handle, char const *data, size_t size)
{
    return OK(IoWriteResult, write(handle, data, size));
}

IoReadResult host_io_read_std(IoStdChannel channel, char *data, size_t size)
{
    assert_equal((int)channel, IO_STD_IN);

    return OK(IoReadResult, write(0, data, size));
}

IoWriteResult host_io_write_std(IoStdChannel channel, char const *data, size_t size)
{
    assert_not_equal((int)channel, IO_STD_IN);

    return OK(IoWriteResult, write((int)channel, data, size));
}
