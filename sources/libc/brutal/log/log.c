#include <brutal/host/log.h>
#include <brutal/log.h>

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
        return str_cast("\e[92m");

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
    print(host_log_writer(), "{}{}:\e[m ", log_color(level), log_prefix(level));
    print(host_log_writer(), "\e[37;2m{}:{3d}:\e[m\e[37m ", location.filename, location.line);
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
    host_log_lock();
    log_unlock_impl(level, location, fmt, args);
    host_log_unlock();

    host_log_panic();
}
