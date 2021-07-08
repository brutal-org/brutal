#pragma once

#include <brutal/base/error.h>
#include <brutal/base/result.h>
#include <brutal/io/read.h>
#include <brutal/io/write.h>
#include <brutal/text/str.h>

#if defined(__linux__)
#    include <brutal/host/linux/io.h>
#elif defined(__brutal__)
#    include <brutal/host/brutal/io.h>
#else
#    error "Unsupported platform!"
#endif

typedef Result(Error, HostIoFileHandle) HostIoOpenFileResult;

HostIoOpenFileResult host_io_file_open(Str path);

HostIoOpenFileResult host_io_file_create(Str path);

MaybeError host_io_file_close(HostIoFileHandle handle);

IoReadResult host_io_read_file(HostIoFileHandle handle, char *data, size_t size);

IoWriteResult host_io_write_file(HostIoFileHandle handle, char const *data, size_t size);
