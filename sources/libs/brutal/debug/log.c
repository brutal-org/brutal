#include <bal/abi.h>
#include <brutal/debug.h>
#include <embed/debug.h>
#include <embed/log.h>

void log_unlock_impl(LogLevel level, SourceLocation location, Str fmt, PrintArgs args)
{
    embed_log_prefix(embed_log_writer());

    if (level == LOG_PANIC)
    {
        print(embed_log_writer(), "panic: ");
    }

    print(embed_log_writer(), "{}:{3d}: ", location.filename, location.line);
    print_impl(embed_log_writer(), fmt, args);
    print(embed_log_writer(), "\n");
}

void log_impl(LogLevel level, SourceLocation location, Str fmt, PrintArgs args)
{
    embed_log_lock();
    log_unlock_impl(level, location, fmt, args);
    embed_log_unlock();
}

noreturn void panic_impl(LogLevel level, SourceLocation location, Str fmt, PrintArgs args)
{
    log_unlock_impl(level, location, fmt, args);
    embed_log_panic();
}
