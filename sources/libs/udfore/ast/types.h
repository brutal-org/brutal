#pragma once
#include <brutal-base>

#include "type/decl.h"
#include "type/expr.h"
#include "type/type.h"
#include "type/val.h"

#define FOREACH_UD_AST(CALL)   \
    FOREACH_UD_EXPR_AST(CALL)  \
    FOREACH_UD_DECL_AST(CALL)  \
    FOREACH_UD_CONST_AST(CALL) \
    FOREACH_UD_TYPE_AST(CALL)  \
    CALL(COUNT)

#define UDAST_ELEMENT_ENUM_ADD(v) UDAST_##v,

typedef enum
{
    UDAST_NONE,
    FOREACH_UD_AST(UDAST_ELEMENT_ENUM_ADD)
} UDAstTypes;

Str udast_type_to_str(int type);
