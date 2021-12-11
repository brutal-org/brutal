#include <cc/parse.h>

// this file will be refactored before the end of the pr

static COp lexeme_to_cop[CLEX_COUNT] = {

    [CLEX_PLUSPLUS] = COP_INC,   // ++
    [CLEX_MINUSMINUS] = COP_DEC, // --
    [CLEX_EQUAL] = COP_ASSIGN,   // =
    //    [] = COP_ASSIGN_ADD,     // +=
    //    [] = COP_ASSIGN_SUB,     // -=
    //    [] = COP_ASSIGN_MULT,    // *=
    //    [] = COP_ASSIGN_DIV,     // /=
    //    [] = COP_ASSIGN_MOD,     // %=
    //    [] = COP_ASSIGN_BIT_AND, // &=
    //    [] = COP_ASSIGN_BIT_OR,  // |=
    //    [] = COP_ASSIGN_BIT_XOR, // ^=
    //    [] = COP_ASSIGN_LSHIFT,  // <<=
    //    [] = COP_ASSIGN_RSHIFT,  // >>=
    [CLEX_PLUS] = COP_ADD,                // +
    [CLEX_MINUS] = COP_SUB,               // -
    [CLEX_STAR] = COP_MULT,               // *
    [CLEX_SLASH] = COP_DIV,               // /
    [CLEX_PERCENT] = COP_MOD,             // mod
    [CLEX_TILDE] = COP_BIT_NOT,           // ~
    [CLEX_AMPERSAND] = COP_BIT_AND,       // &
    [CLEX_BAR] = COP_BIT_OR,              // |
    [CLEX_CIRCUMFLEX] = COP_BIT_XOR,      // ^
    [CLEX_LCHEVRONCHEVRON] = COP_LSHIFT,  // <<
    [CLEX_RCHEVRONCHEVRON] = COP_RSHIFT,  // >>
    [CLEX_EXCLAMATION] = COP_NOT,         // !
    [CLEX_AMPERSANDAMPERSAND] = COP_AND,  // &&
    [CLEX_BARBAR] = COP_OR,               // ||
    [CLEX_EQUALEQUAL] = COP_EQ,           // ==
    [CLEX_EXCLAMATIONEQUAL] = COP_NOT_EQ, // !=
    [CLEX_LCHEVRON] = COP_LT,             // <
    [CLEX_RCHEVRON] = COP_GT,             // >
                                          // [] = COP_LT_EQ,                 // <=
                                          // [] = COP_GT_EQ,                 // >=
    [CLEX_LBRACKET] = COP_INDEX,          // []
                                          //   [CLEX_STAR] = COP_DEREF,             // *
                                          //  [CLEX_AMPERSAND] = COP_REF,          // &
    [CLEX_DOT] = COP_ACCESS,              // .
    [CLEX_ARROW] = COP_PTR_ACCESS,        // ->

};
COp cparse_lexeme_to_cop(LexemeType type)
{
    return lexeme_to_cop[type];
}
