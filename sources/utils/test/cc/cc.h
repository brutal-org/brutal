#pragma once

#include <brutal-io>
#include <cc/parse.h>

// FIXME: get ride of thoses ugly macros.

#define ctx_lex(lexer_name, str)                     \
    Scan _scan = {};                                 \
    scan_init(&_scan, str);                          \
    Lex lexer_name = clex(&_scan, test_use_alloc()); \
    scan_assert_no_error(&_scan)

#define ctx_lex_proc(lexer_name, str)                                            \
    Scan _scan = {};                                                             \
    scan_init(&_scan, str);                                                      \
    Lex _source_lex = clex(&_scan, test_use_alloc());                            \
    scan_assert_no_error(&_scan);                                                \
    Lex lexer_name = cproc_file(&_source_lex, str$("test.c"), test_use_alloc()); \
    lex_assert_no_error(&_source_lex)

bool assert_lex_eq(Lex *first, Str left);
