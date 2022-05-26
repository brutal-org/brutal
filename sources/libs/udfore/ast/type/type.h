#pragma once

#define FOREACH_UD_TYPE_AST(TACTION) \
    TACTION(TYPE_NONE)               \
    TACTION(TYPE_IDENT)              \
    TACTION(TYPE_FUNC)               \
    TACTION(TYPE_ARG_LIST)           \
    TACTION(TYPE_INT)                \
    TACTION(TYPE_STRING)             \
    TACTION(TYPE_FLOAT)              \
    TACTION(TYPE_ARRAY)              \
    TACTION(TYPE_CONSTANT)           \
    TACTION(TYPE_NAMED)
