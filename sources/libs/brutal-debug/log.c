#include <brutal-fmt>
#include <embed/debug.h>
#include <embed/log.h>

#include "log.h"

void log_unlock_impl(LogLevel level, Loc loc, Str fmt, AnyVa args)
{
    embed_log_prefix(embed_log_writer());

    if (level == LOG_PANIC)
    {
        fmt_format$(embed_log_writer(), "panic: ");
    }

    fmt_format$(embed_log_writer(), "{}:{3d}: ", loc.file, loc.line);
    fmt_format(embed_log_writer(), fmt, args);
    fmt_format$(embed_log_writer(), "\n");
}

void log_impl(LogLevel level, Loc loc, Str fmt, AnyVa args)
{
    embed_log_lock();
    log_unlock_impl(level, loc, fmt, args);
    embed_log_unlock();
}

noreturn void panic_impl(LogLevel level, Loc loc, Str fmt, AnyVa args)
{
    log_unlock_impl(level, loc, fmt, args);
    embed_log_panic();
}
