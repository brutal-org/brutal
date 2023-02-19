#pragma once

#include <cc/parse.h>
#include <brutal-io>

// FIXME: get ride of those ugly macros.

#define ctx_lex(lexer_name, str)                     \
    Scan _scan = {};                                 \
    scan_init(&_scan, str);                          \
    Lex lexer_name = clex(&_scan, test_use_alloc()); \
    scan_assert_no_error(&_scan)
