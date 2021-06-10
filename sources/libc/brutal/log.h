#pragma once

#include <brutal/io/print.h>
#include <brutal/log/source_location.h>

enum log_level
{
    LOG_PANIC,
    LOG_TODO,
    LOG_FIXME,
    LOG_DEFAULT,
    LOG_DEBUG,
};

void log_impl(enum log_level level, struct source_location location, str_t fmt, struct print_args args);

#define panic(fmt, ...) \
    log_impl(LOG_PANIC, source_location_current, make_str(fmt), PRINT_ARGS(__VA_ARGS__))

#define todo(fmt, ...) \
    log_impl(LOG_TODO, source_location_current, make_str(fmt), PRINT_ARGS(__VA_ARGS__))

#define fixme(fmt, ...) \
    log_impl(LOG_FIXME, source_location_current, make_str(fmt), PRINT_ARGS(__VA_ARGS__))

#define log(fmt, ...) \
    log_impl(LOG_DEFAULT, source_location_current, make_str(fmt), PRINT_ARGS(__VA_ARGS__))

#define debug(fmt, ...) \
    log_impl(LOG_DEBUG, source_location_current, make_str(fmt), PRINT_ARGS(__VA_ARGS__))
