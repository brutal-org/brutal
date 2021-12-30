#pragma once

#include <brutal/io/std.h>
#include <cc/parse.h>
#include <cc/proc/proc.h>

#define lex_assert_no_error(LEX)                                                                                 \
    ({                                                                                                           \
        if ((LEX)->has_error)                                                                                    \
        {                                                                                                        \
            panic$("lex contains error: '{}' with lexeme: '{}'", (LEX)->error.message, (LEX)->error.lexeme.str); \
        }                                                                                                        \
    })

#define scan_assert_no_error(SCAN)                    \
    ({                                                \
        if ((SCAN)->has_error)                        \
        {                                             \
            scan_dump_error(SCAN, io_chan_out());     \
            panic$("scan: {} contains error", #SCAN); \
        }                                             \
    })

#define ctx_lex(lexer_name, str)                 \
    Scan _scan = {};                             \
    scan_init(&_scan, str);                      \
    Lex lexer_name = clex(&_scan, test_alloc()); \
    scan_assert_no_error(&_scan)

#define ctx_lex_proc(lexer_name, str)                                        \
    Scan _scan = {};                                                         \
    scan_init(&_scan, str);                                                  \
    Lex _source_lex = clex(&_scan, test_alloc());                            \
    scan_assert_no_error(&_scan);                                            \
    Lex lexer_name = cproc_file(&_source_lex, str$("test.c"), test_alloc()); \
    lex_assert_no_error(&_source_lex)

bool assert_lex_eq(Lex *first, Str left);
