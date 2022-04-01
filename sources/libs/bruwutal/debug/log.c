#include <bal/abi.h>
#include <bruwutal/debug.h>
#include <embed/debug.h>
#include <embed/log.h>

void log_unlock_impl(LogLevel level, SourceLocation location, Str fmt, AnyVa args)
{
    embed_log_prefix(embed_log_writer());

    if (level == LOG_PANIC)
    {
        io_fmt$(embed_log_writer(), "panic: ");
    }

    io_fmt$(embed_log_writer(), "{}:{3d}: ", location.filename, location.line);
    io_fmt(embed_log_writer(), fmt, args);
    io_fmt$(embed_log_writer(), "\n");
}

void log_impl(LogLevel level, SourceLocation location, Str fmt, AnyVa args)
{
    embed_log_lock();
    log_unlock_impl(level, location, fmt, args);
    embed_log_unlock();
}

noreturn void panic_impl(LogLevel level, SourceLocation location, Str fmt, AnyVa args)
{
    log_unlock_impl(level, location, fmt, args);
    embed_log_panic();
}
