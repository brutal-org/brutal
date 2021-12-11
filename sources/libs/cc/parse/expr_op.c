#include <cc/parse.h>

// this file will be refactored before the end of the pr

static COp lexeme_to_cop[CLEX_COUNT] = {

    [CLEX_PLUSPLUS] = COP_INC,                 // ++
    [CLEX_MINUSMINUS] = COP_DEC,               // --
    [CLEX_EQUAL] = COP_ASSIGN,                 // =
    [CLEX_PLUS_EQ] = COP_ASSIGN_ADD,           // +=
    [CLEX_MINUS_EQ] = COP_ASSIGN_SUB,          // -=
    [CLEX_STAR_EQ] = COP_ASSIGN_MULT,          // *=
    [CLEX_SLASH_EQ] = COP_ASSIGN_DIV,          // /=
    [CLEX_PERCENT_EQ] = COP_ASSIGN_MOD,        // %=
    [CLEX_AMPERSAND_EQ] = COP_ASSIGN_BIT_AND,  // &=
    [CLEX_BAR_EQ] = COP_ASSIGN_BIT_OR,         // |=
    [CLEX_CIRCUMFLEX_EQ] = COP_ASSIGN_BIT_XOR, // ^=
    [CLEX_LSHIFT_EQ] = COP_ASSIGN_LSHIFT,      // <<=
    [CLEX_RSHIFT_EQ] = COP_ASSIGN_RSHIFT,      // >>=
    [CLEX_PLUS] = COP_ADD,                     // +
    [CLEX_MINUS] = COP_SUB,                    // -
    [CLEX_STAR] = COP_MULT,                    // *
    [CLEX_SLASH] = COP_DIV,                    // /
    [CLEX_PERCENT] = COP_MOD,                  // mod
    [CLEX_TILDE] = COP_BIT_NOT,                // ~
    [CLEX_AMPERSAND] = COP_BIT_AND,            // &
    [CLEX_BAR] = COP_BIT_OR,                   // |
    [CLEX_CIRCUMFLEX] = COP_BIT_XOR,           // ^
    [CLEX_LSHIFT] = COP_LSHIFT,                // <<
    [CLEX_RSHIFT] = COP_RSHIFT,                // >>
    [CLEX_EXCLAMATION] = COP_NOT,              // !
    [CLEX_AMPERSANDAMPERSAND] = COP_AND,       // &&
    [CLEX_BARBAR] = COP_OR,                    // ||
    [CLEX_EQUALEQUAL] = COP_EQ,                // ==
    [CLEX_EXCLAMATIONEQUAL] = COP_NOT_EQ,      // !=
    [CLEX_LCHEVRON] = COP_LT,                  // <
    [CLEX_RCHEVRON] = COP_GT,                  // >
    [CLEX_LCHEVRON_EQ] = COP_LT_EQ,            // <=
    [CLEX_RCHEVRON_EQ] = COP_GT_EQ,            // >=
    [CLEX_LBRACKET] = COP_INDEX,               // []
                                               //   [CLEX_STAR] = COP_DEREF,             // *
                                               //  [CLEX_AMPERSAND] = COP_REF,          // &
    [CLEX_DOT] = COP_ACCESS,                   // .
    [CLEX_ARROW] = COP_PTR_ACCESS,             // ->

};
COp cparse_lexeme_to_cop(LexemeType type)
{
    return lexeme_to_cop[type];
}
