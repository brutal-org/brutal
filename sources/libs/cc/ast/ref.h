#pragma once

#include <brutal/ds.h>
#include <brutal/parse/lex.h>
#include <brutal/parse/lexeme.h>

/**
 * @brief Change element to have a SrcRef with begin = begin and end = lex.head
 * It is a macro because it is more simple to do something like this:
 * ```c
 * return with_ref$(if_stmt, begin, lex);
 * ```
 * than this:
 * ```c
 * if_stmt.ref = lex_src_ref(if_stmt, begin, lex);
 * return if_stmt;
 * ```
 * @param element The ast element to change.
 * @param begin Begin index of the ast element.
 * @param lex The lexer at the end of the element.
 */
#define with_cref$(element, begin, lex)                           \
    ({                                                            \
        typeof(element) __element = (element);                    \
        __element.ref = lex_src_ref((lex), (begin), (lex)->head); \
        __element;                                                \
    })
