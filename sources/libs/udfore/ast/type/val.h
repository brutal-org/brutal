#pragma once
#define FOREACH_UD_CONST_AST(VACTION) \
    VACTION(CONST_STRING)             \
    VACTION(CONST_IDENT)              \
    VACTION(CONST_SIGNED)             \
    VACTION(CONST_UNSIGNED)           \
    VACTION(CONST_FLOAT)              \
    VACTION(CONST_BOOL)               \
    VACTION(CONST_NULL)
