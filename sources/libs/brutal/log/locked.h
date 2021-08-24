#pragma once

#include <brutal/io/print.h>
#include <brutal/log/log.h>
#include <brutal/log/source_location.h>

/* --- Locked log functions ------------------------------------------------- */

void log_impl(enum log_level level, SourceLocation location, Str fmt, struct print_args args);
noreturn void panic_impl(enum log_level level, SourceLocation location, Str fmt, struct print_args args);

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
