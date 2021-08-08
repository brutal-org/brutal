#include <brutal/host/log.h>
#include <brutal/log.h>

#ifdef __kernel__
#    include "kernel/cpu.h"
#endif

static Str log_color(enum log_level level)
{
    switch (level)
    {
    case LOG_PANIC:
        return str_cast("");

    case LOG_TODO:
        return str_cast("");

    case LOG_FIXME:
        return str_cast("");

    case LOG_DEFAULT:
        return str_cast("");

    default:
    case LOG_DEBUG:
        return str_cast("");
    }
}

static Str log_prefix(enum log_level level)
{
    switch (level)
    {
    case LOG_PANIC:
        return str_cast("panic");

    case LOG_TODO:
        return str_cast("todo");

    case LOG_FIXME:
        return str_cast("fixme");

    case LOG_DEFAULT:
        return str_cast("info");

    default:
    case LOG_DEBUG:
        return str_cast("debug");
    }
}

void log_unlock_impl(enum log_level level, SourceLocation location, Str fmt, struct print_args args)
{
#ifdef __kernel__
    print(host_log_writer(), "cpu{}: ", cpu_self_id());
#endif

    print(host_log_writer(), "{}{}: ", log_color(level), log_prefix(level));
    print(host_log_writer(), "{}:{3d}: ", location.filename, location.line);
    print_impl(host_log_writer(), fmt, args);
    print(host_log_writer(), "\n");
}

void log_impl(enum log_level level, SourceLocation location, Str fmt, struct print_args args)
{
    host_log_lock();
    log_unlock_impl(level, location, fmt, args);
    host_log_unlock();
}

noreturn void panic_impl(enum log_level level, SourceLocation location, Str fmt, struct print_args args)
{
    log_unlock_impl(level, location, fmt, args);

    host_log_panic();
}
