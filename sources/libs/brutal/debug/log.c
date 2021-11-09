#include <brutal/debug.h>
#include <embed/arch.h>
#include <embed/log.h>

#ifdef __kernel__
#    include "kernel/cpu.h"
#endif

void log_unlock_impl(LogLevel level, SourceLocation location, Str fmt, PrintArgs args)
{
#ifdef __kernel__
    print(host_log_writer(), "cpu{}: ", cpu_self_id());

    if (task_self())
    {
        print(host_log_writer(), "{}({}): ", str$(&task_self()->name), task_self()->id);
    }
#endif

    if (level == LOG_PANIC)
    {
        print(host_log_writer(), "panic: ");
    }

    print(host_log_writer(), "{}:{3d}: ", location.filename, location.line);
    print_impl(host_log_writer(), fmt, args);
    print(host_log_writer(), "\n");
}

void log_impl(LogLevel level, SourceLocation location, Str fmt, PrintArgs args)
{
    host_log_lock();
    log_unlock_impl(level, location, fmt, args);
    host_log_unlock();
}

noreturn void panic_impl(LogLevel level, SourceLocation location, Str fmt, PrintArgs args)
{
    log_unlock_impl(level, location, fmt, args);

    print(host_log_writer(), "Backtrace:\n");

    uintptr_t buf[128];
    size_t len = arch_backtrace(buf, 128);

    for (size_t i = 0; i < len; i++)
    {
        print(host_log_writer(), "\t{#p}\n", buf[i]);
    }

    host_log_panic();
}
