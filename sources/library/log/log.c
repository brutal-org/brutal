#include <host/log.h>
#include <library/log.h>

str_t log_prefix(enum log_level level)
{
    switch (level)
    {
    case LOG_PANIC:
        return make_str("panic");

    case LOG_TODO:
        return make_str("todo");

    case LOG_FIXME:
        return make_str("fixme");

    case LOG_DEFAULT:
        return make_str("info");

    default:
    case LOG_DEBUG:
        return make_str("debug");
    }
}

void log_impl(enum log_level level, str_t fmt, struct print_args args)
{
    host_log_lock();
    print(host_log_writer(), "{}: ", log_prefix(level));
    print_impl(host_log_writer(), fmt, args);
    io_put(host_log_writer(), '\n');
    host_log_unlock();
}