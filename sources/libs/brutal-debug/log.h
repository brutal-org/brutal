#pragma once

#include <brutal-base>

typedef enum
{
    LOG_PANIC,
    LOG_DEFAULT,
} LogLevel;

/* --- Locked log functions ------------------------------------------------- */

void log_impl(LogLevel level, Loc loc, Str fmt, AnyVa args);
noreturn void panic_impl(LogLevel level, Loc loc, Str fmt, AnyVa args);

#define panic$(fmt, ...) \
    panic_impl(LOG_PANIC, loc$(), str$(fmt), any_va$(__VA_ARGS__));

#define log$(fmt, ...) \
    log_impl(LOG_DEFAULT, loc$(), str$(fmt), any_va$(__VA_ARGS__))

/* --- Unlocked log functions ----------------------------------------------- */

void log_unlock_impl(LogLevel level, Loc loc, Str fmt, AnyVa args);

#define log_unlock(fmt, ...) \
    log_unlock_impl(LOG_DEFAULT, loc$(), str$(fmt), any_va$(__VA_ARGS__))
