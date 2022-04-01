#pragma once

#include <bruwutal/base/any.h>
#include <bruwutal/debug/log.h>
#include <bruwutal/debug/source_location.h>

/* --- Locked log functions ------------------------------------------------- */

void log_impl(LogLevel level, SourceLocation location, Str fmt, AnyVa args);
noreturn void panic_impl(LogLevel level, SourceLocation location, Str fmt, AnyVa args);

#define panic$(fmt, ...) \
    panic_impl(LOG_PANIC, source_location_current, str$(fmt), any_va$(__VA_ARGS__));

#define log$(fmt, ...) \
    log_impl(LOG_DEFAULT, source_location_current, str$(fmt), any_va$(__VA_ARGS__))
