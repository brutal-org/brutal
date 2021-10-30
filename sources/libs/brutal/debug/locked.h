#pragma once

#include <brutal/debug/log.h>
#include <brutal/debug/source_location.h>
#include <brutal/io/print.h>

/* --- Locked log functions ------------------------------------------------- */

void log_impl(LogLevel level, SourceLocation location, Str fmt, PrintArgs args);
noreturn void panic_impl(LogLevel level, SourceLocation location, Str fmt, PrintArgs args);

#define panic$(fmt, ...) \
    panic_impl(LOG_PANIC, source_location_current, str$(fmt), PRINT_ARGS(__VA_ARGS__));

#define log$(fmt, ...) \
    log_impl(LOG_DEFAULT, source_location_current, str$(fmt), PRINT_ARGS(__VA_ARGS__))
