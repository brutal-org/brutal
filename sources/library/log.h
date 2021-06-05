#pragma once

#include <library/io/print.h>

enum log_level
{
    LOG_PANIC,
    LOG_TODO,
    LOG_FIXME,
    LOG_DEFAULT,
    LOG_DEBUG,
};

void log_impl(enum log_level level, str_t fmt, struct print_args args);

#define panic(fmt, args) \
    log_impl(LOG_PANIC, (fmt), PRINT_ARGS(args))

#define todo(fmt, args) \
    log_impl(LOG_TODO, (fmt), PRINT_ARGS(args))

#define fixme(fmt, args) \
    log_impl(LOG_FIXME, (fmt), PRINT_ARGS(args))

#define log(fmt, args) \
    log_impl(LOG_DEFAULT, (fmt), PRINT_ARGS(args))

#define debug(fmt, args) \
    log_impl(LOG_DEBUG, (fmt), PRINT_ARGS(args))
