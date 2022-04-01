#include <abi/Syscalls.h>

static int _pid_cache = -1;

HjResult hj_process_this(int *pid)
{
    if (_pid_cache == -1)
    {
        __syscall(HJ_PROCESS_THIS, (uintptr_t)&_pid_cache);
    }

    *pid = _pid_cache;

    return SUCCESS;
}

HjResult hj_process_name(char *name, size_t size)
{
    return __syscall(HJ_PROCESS_NAME, (uintptr_t)name, size);
}

HjResult hj_process_launch(Launchpad *launchpad, int *pid)
{
    return __syscall(HJ_PROCESS_LAUNCH, (uintptr_t)launchpad, (uintptr_t)pid);
}

HjResult hj_process_clone(int *pid, TaskFlags flags)
{
    _pid_cache = -1;

    return __syscall(HJ_PROCESS_CLONE, (uintptr_t)pid, flags);
}

HjResult hj_process_exec(Launchpad *launchpad)
{
    return __syscall(HJ_PROCESS_EXEC, (uintptr_t)launchpad);
}

HjResult hj_process_exit(int exit_code)
{
    return __syscall(HJ_PROCESS_EXIT, (uintptr_t)exit_code);
}

HjResult hj_process_cancel(int pid)
{
    return __syscall(HJ_PROCESS_CANCEL, (uintptr_t)pid);
}

HjResult hj_process_sleep(int time)
{
    return __syscall(HJ_PROCESS_SLEEP, (uintptr_t)time);
}

HjResult hj_process_wait(int tid, int *user_exit_value)
{
    return __syscall(HJ_PROCESS_WAIT, (uintptr_t)tid, (uintptr_t)user_exit_value);
}

HjResult hj_memory_alloc(size_t size, uintptr_t *out_address)
{
    return __syscall(HJ_MEMORY_ALLOC, (uintptr_t)size, (uintptr_t)out_address);
}

HjResult hj_memory_map(uintptr_t address, size_t size, int flags)
{
    return __syscall(HJ_MEMORY_MAP, address, size, flags);
}

HjResult hj_memory_free(uintptr_t address)
{
    return __syscall(HJ_MEMORY_FREE, address);
}

HjResult hj_memory_include(int handle, uintptr_t *out_address, size_t *out_size)
{
    return __syscall(HJ_MEMORY_INCLUDE, (uintptr_t)handle, (uintptr_t)out_address, (uintptr_t)out_size);
}

HjResult hj_memory_get_handle(uintptr_t address, int *out_handle)
{
    return __syscall(HJ_MEMORY_GET_HANDLE, address, (uintptr_t)out_handle);
}

HjResult hj_filesystem_mkdir(const char *raw_path, size_t size)
{
    return __syscall(HJ_FILESYSTEM_MKDIR, (uintptr_t)raw_path, (uintptr_t)size);
}

HjResult hj_filesystem_mkpipe(const char *raw_path, size_t size)
{
    return __syscall(HJ_FILESYSTEM_MKPIPE, (uintptr_t)raw_path, (uintptr_t)size);
}

HjResult hj_filesystem_link(const char *raw_old_path, size_t old_size, const char *raw_new_path, size_t new_size)
{
    return __syscall(
        HJ_FILESYSTEM_LINK,
        (uintptr_t)raw_old_path,
        (uintptr_t)old_size,
        (uintptr_t)raw_new_path,
        (uintptr_t)new_size);
}

HjResult hj_filesystem_unlink(const char *raw_path, size_t size)
{
    return __syscall(
        HJ_FILESYSTEM_UNLINK,
        (uintptr_t)raw_path,
        (uintptr_t)size);
}

HjResult hj_filesystem_rename(const char *raw_old_path, size_t old_size, const char *raw_new_path, size_t new_size)
{
    return __syscall(
        HJ_FILESYSTEM_RENAME,
        (uintptr_t)raw_old_path,
        (uintptr_t)old_size,
        (uintptr_t)raw_new_path,
        (uintptr_t)new_size);
}

HjResult hj_system_info(SystemInfo *info)
{
    return __syscall(HJ_SYSTEM_INFO, (uintptr_t)info);
}

HjResult hj_system_status(SystemStatus *status)
{
    return __syscall(HJ_SYSTEM_STATUS, (uintptr_t)status);
}

HjResult hj_system_time(TimeStamp *timestamp)
{
    return __syscall(HJ_SYSTEM_TIME, (uintptr_t)timestamp);
}

HjResult hj_system_tick(uint32_t *tick)
{
    return __syscall(HJ_SYSTEM_TICKS, (uintptr_t)tick);
}

HjResult hj_system_reboot()
{
    return __syscall(HJ_SYSTEM_REBOOT);
}

HjResult hj_system_shutdown()
{
    return __syscall(HJ_SYSTEM_SHUTDOWN);
}

HjResult hj_create_pipe(int *reader_handle, int *writer_handle)
{
    return __syscall(HJ_CREATE_PIPE, (uintptr_t)reader_handle, (uintptr_t)writer_handle);
}

HjResult hj_create_term(int *server_handle, int *client_handle)
{
    return __syscall(HJ_CREATE_TERM, (uintptr_t)server_handle, (uintptr_t)client_handle);
}

HjResult hj_handle_open(int *handle, const char *raw_path, size_t size, HjOpenFlag flags)
{
    return __syscall(HJ_HANDLE_OPEN, (uintptr_t)handle, (uintptr_t)raw_path, (uintptr_t)size, flags);
}

HjResult hj_handle_close(int handle)
{
    return __syscall(HJ_HANDLE_CLOSE, (uintptr_t)handle);
}

HjResult hj_handle_reopen(int handle, int *reopened)
{
    return __syscall(HJ_HANDLE_REOPEN, (uintptr_t)handle, (uintptr_t)reopened);
}

HjResult hj_handle_copy(int source, int destination)
{
    return __syscall(HJ_HANDLE_COPY, (uintptr_t)source, (uintptr_t)destination);
}

HjResult hj_handle_poll(HandlePoll *handles, size_t count, Timeout timeout)
{
    return __syscall(HJ_HANDLE_POLL, (uintptr_t)handles, (uintptr_t)count, timeout);
}

HjResult hj_handle_read(int handle, void *buffer, size_t size, size_t *read)
{
    return __syscall(HJ_HANDLE_READ, (uintptr_t)handle, (uintptr_t)buffer, (uintptr_t)size, (uintptr_t)read);
}

HjResult hj_handle_write(int handle, const void *buffer, size_t size, size_t *written)
{
    return __syscall(HJ_HANDLE_WRITE, (uintptr_t)handle, (uintptr_t)buffer, (uintptr_t)size, (uintptr_t)written);
}

HjResult hj_handle_call(int handle, IOCall request, void *args)
{
    return __syscall(HJ_HANDLE_CALL, (uintptr_t)handle, (uintptr_t)request, (uintptr_t)args);
}

HjResult hj_handle_seek(int handle, ssize64_t *offset, HjWhence whence, ssize64_t *result_offset)
{
    return __syscall(HJ_HANDLE_SEEK, (uintptr_t)handle, (uintptr_t)offset, (uintptr_t)whence, (uintptr_t)result_offset);
}

HjResult hj_handle_stat(int handle, HjStat *state)
{
    return __syscall(HJ_HANDLE_STAT, (uintptr_t)handle, (uintptr_t)state);
}

HjResult hj_handle_connect(int *handle, const char *raw_path, size_t size)
{
    return __syscall(HJ_HANDLE_CONNECT, (uintptr_t)handle, (uintptr_t)raw_path, (uintptr_t)size);
}

HjResult hj_handle_accept(int handle, int *connection_handle)
{
    return __syscall(
        HJ_HANDLE_ACCEPT,
        (uintptr_t)handle,
        (uintptr_t)connection_handle);
}
