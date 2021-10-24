#pragma once

#include <brutal/debug/log.h>
#include <brutal/debug/source_location.h>
#include <brutal/io/print.h>

/* --- Unlocked log functions ----------------------------------------------- */

void log_unlock_impl(LogLevel level, SourceLocation location, Str fmt, PrintArgs args);

#define log_unlock(fmt, ...) \
    log_unlock_impl(LOG_DEFAULT, source_location_current, str$(fmt), PRINT_ARGS(__VA_ARGS__))
