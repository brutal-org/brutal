#pragma once

#define FOREACH_UD_DECL_AST(DACTION) \
    DACTION(DECL_NONE)               \
    DACTION(DECL_VAR)                \
    DACTION(DECL_FUNC)               \
    DACTION(DECL_TYPE)               \
    DACTION(DECL_EXPR)               \
    DACTION(DECL_LIST)
