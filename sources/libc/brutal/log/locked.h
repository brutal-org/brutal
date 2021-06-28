#pragma once

#include <brutal/io/print.h>
#include <brutal/log/log.h>
#include <brutal/log/source_location.h>

/* --- Locked log functions ------------------------------------------------- */

void log_impl(enum log_level level, struct source_location location, str_t fmt, struct print_args args);
void panic_impl(enum log_level level, struct source_location location, str_t fmt, struct print_args args);

#define panic(fmt, ...) (                                                                       \
    {                                                                                           \
        panic_impl(LOG_PANIC, source_location_current, str_cast(fmt), PRINT_ARGS(__VA_ARGS__)); \
        __builtin_unreachable();                                                                \
    })

#define todo(fmt, ...) \
    log_impl(LOG_TODO, source_location_current, str_cast(fmt), PRINT_ARGS(__VA_ARGS__))

#define fixme(fmt, ...) \
    log_impl(LOG_FIXME, source_location_current, str_cast(fmt), PRINT_ARGS(__VA_ARGS__))

#define log(fmt, ...) \
    log_impl(LOG_DEFAULT, source_location_current, str_cast(fmt), PRINT_ARGS(__VA_ARGS__))

#define debug(fmt, ...) \
    log_impl(LOG_DEBUG, source_location_current, str_cast(fmt), PRINT_ARGS(__VA_ARGS__))
