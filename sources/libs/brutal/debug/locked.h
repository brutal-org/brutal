#pragma once

#include <brutal/debug/log.h>
#include <brutal/debug/source_location.h>
#include <brutal/io/print.h>

/* --- Locked log functions ------------------------------------------------- */

void log_impl(LogLevel level, SourceLocation location, Str fmt, PrintArgs args);
noreturn void panic_impl(LogLevel level, SourceLocation location, Str fmt, PrintArgs args);

#define panic$(fmt, ...) \
    panic_impl(LOG_PANIC, source_location_current, str$(fmt), PRINT_ARGS(__VA_ARGS__));

#define panic_todo$(fmt, ...) \
    panic_impl(LOG_TODO, source_location_current, str$(fmt), PRINT_ARGS(__VA_ARGS__));

#define panic_fixme$(fmt, ...) \
    panic_impl(LOG_FIXME, source_location_current, str$(fmt), PRINT_ARGS(__VA_ARGS__));

#define todo$(fmt, ...) \
    log_impl(LOG_TODO, source_location_current, str$(fmt), PRINT_ARGS(__VA_ARGS__))

#define fixme$(fmt, ...) \
    log_impl(LOG_FIXME, source_location_current, str$(fmt), PRINT_ARGS(__VA_ARGS__))

#define log$(fmt, ...) \
    log_impl(LOG_DEFAULT, source_location_current, str$(fmt), PRINT_ARGS(__VA_ARGS__))

#define debug$(fmt, ...) \
    log_impl(LOG_DEBUG, source_location_current, str$(fmt), PRINT_ARGS(__VA_ARGS__))
