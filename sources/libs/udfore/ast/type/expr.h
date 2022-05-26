#pragma once

#define FOREACH_UD_EXPR_AST(EACTION)       \
    EACTION(EXPR_NONE)                     \
    EACTION(EXPR_NEG)                      \
    EACTION(EXPR_RETURN)                   \
    EACTION(EXPR_VAL)                      \
    EACTION(EXPR_IF)                       \
    EACTION(EXPR_ELSE)                     \
    EACTION(EXPR_CONSTANT)                 \
    EACTION(EXPR_DECL_BLOCK)               \
    EACTION(EXPR_IDENT)                    \
    EACTION(EXPR_INC)            /* ++ */  \
    EACTION(EXPR_DEC)            /* -- */  \
    EACTION(EXPR_ASSIGN)         /* = */   \
    EACTION(EXPR_ASSIGN_ADD)     /* += */  \
    EACTION(EXPR_ASSIGN_SUB)     /* -= */  \
    EACTION(EXPR_ASSIGN_MULT)    /* *= */  \
    EACTION(EXPR_ASSIGN_DIV)     /* /= */  \
    EACTION(EXPR_ASSIGN_MOD)     /* %= */  \
    EACTION(EXPR_ASSIGN_BIT_AND) /* &= */  \
    EACTION(EXPR_ASSIGN_BIT_OR)  /* |= */  \
    EACTION(EXPR_ASSIGN_BIT_XOR) /* ^= */  \
    EACTION(EXPR_ASSIGN_LSHIFT)  /* <<= */ \
    EACTION(EXPR_ASSIGN_RSHIFT)  /* >>= */ \
    EACTION(EXPR_ADD)            /* + */   \
    EACTION(EXPR_SUB)            /* - */   \
    EACTION(EXPR_MULT)           /* * */   \
    EACTION(EXPR_DIV)            /* / */   \
    EACTION(EXPR_MOD)            /* mod */ \
    EACTION(EXPR_BIT_NOT)        /* ~ */   \
    EACTION(EXPR_BIT_AND)        /* & */   \
    EACTION(EXPR_BIT_OR)         /* | */   \
    EACTION(EXPR_BIT_XOR)        /* ^ */   \
    EACTION(EXPR_LSHIFT)         /* << */  \
    EACTION(EXPR_RSHIFT)         /* >> */  \
    EACTION(EXPR_NOT)            /* ! */   \
    EACTION(EXPR_AND)            /* && */  \
    EACTION(EXPR_OR)             /* || */  \
    EACTION(EXPR_EQ)             /* == */  \
    EACTION(EXPR_NOT_EQ)         /* != */  \
    EACTION(EXPR_LT)             /* < */   \
    EACTION(EXPR_GT)             /* > */   \
    EACTION(EXPR_LT_EQ)          /* <= */  \
    EACTION(EXPR_GT_EQ)          /* >= */  \
    EACTION(EXPR_INDEX)          /* [] */  \
    EACTION(EXPR_DEREF)          /* * */   \
    EACTION(EXPR_REF)            /* & */   \
    EACTION(EXPR_ACCESS)         /* . */   \
    EACTION(EXPR_PTR_ACCESS)     /* -> */  \
    EACTION(EXPR_CALL)           /* ( */
