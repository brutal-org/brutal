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
        return str$("");

    case LOG_TODO:
        return str$("");

    case LOG_FIXME:
        return str$("");

    case LOG_DEFAULT:
        return str$("");

    default:
    case LOG_DEBUG:
        return str$("");
    }
}

static Str log_prefix(enum log_level level)
{
    switch (level)
    {
    case LOG_PANIC:
        return str$("panic");

    case LOG_TODO:
        return str$("todo");

    case LOG_FIXME:
        return str$("fixme");

    case LOG_DEFAULT:
        return str$("info");

    default:
    case LOG_DEBUG:
        return str$("debug");
    }
}

void log_unlock_impl(enum log_level level, SourceLocation location, Str fmt, struct print_args args)
{
#ifdef __kernel__
    print(host_log_writer(), "cpu{}: ", cpu_self_id());

    if (task_self())
    {
        print(host_log_writer(), "{}({}): ", str$(&task_self()->name), task_self()->handle);
    }
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
