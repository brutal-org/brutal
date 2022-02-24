#pragma once

#include <brutal/ds.h>
#include <brutal/parse/lex.h>
#include <brutal/parse/lexeme.h>
typedef enum
{
    CREF_NONE,
    CREF_SINGLE,
    CREF_MULTIPLE,
} CRefSource;

typedef struct cref CRef;

struct cref
{
    CRefSource source;
    SrcRef ref;

    Vec(CRef) sub_ref;
};

CRef cref(Lex *lex, int begin, int end);

#define CREF CRef ref

/**
 * @brief Change element to have a cref with begin = begin and end = lex.head
 * It is a macro because it is more simple to do something like this:
 * ```c
 * return with_ref$(if_stmt, begin, lex);
 * ```
 * than this:
 * ```c
 * if_stmt.cref = cref(if_stmt, begin, lex);
 * return if_stmt;
 * ```
 * @param element The ast element to change.
 * @param begin Begin index of the ast element.
 * @param lex The lexer at the end of the element.
 */
#define with_cref$(element, begin, lex)                             \
    ({                                                              \
        typeof(element) _with_ref = (element);                     \
        if (begin == (lex)->head)                                   \
        {                                                           \
            _with_ref.ref = cref((lex), 0, 0);                     \
        }                                                           \
        else                                                        \
        {                                                           \
            _with_ref.ref = cref((lex), (begin), (lex)->head - 1); \
        }                                                           \
        _with_ref;                                                 \
    })
