#pragma once

#include <brutal/io/print.h>
#include <brutal/log/log.h>
#include <brutal/log/source_location.h>

/* --- Unlocked log functions ----------------------------------------------- */

void log_unlock_impl(LogLevel level, SourceLocation location, Str fmt, PrintArgs args);

#define log_unlock(fmt, ...) \
    log_unlock_impl(LOG_DEFAULT, source_location_current, str$(fmt), PRINT_ARGS(__VA_ARGS__))
