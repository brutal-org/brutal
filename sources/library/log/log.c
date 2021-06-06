#include <host/log.h>
#include <library/log.h>

static str_t log_color(enum log_level level)
{
    switch (level)
    {
    case LOG_PANIC:
        return make_str("");

    case LOG_TODO:
        return make_str("");

    case LOG_FIXME:
        return make_str("");

    case LOG_DEFAULT:
        return make_str("\e[92m");

    default:
    case LOG_DEBUG:
        return make_str("");
    }
}

static str_t log_prefix(enum log_level level)
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

void log_impl(enum log_level level, struct source_location location, str_t fmt, struct print_args args)
{
    UNUSED(location);

    host_log_lock();
    print(host_log_writer(), "{}{}:\e[m ", log_color(level), log_prefix(level));
    print(host_log_writer(), "\e[37;2m{}:{}:\e[m\e[37m ", location.filename, location.line);
    print_impl(host_log_writer(), fmt, args);
    print(host_log_writer(), "\n");
    host_log_unlock();
}