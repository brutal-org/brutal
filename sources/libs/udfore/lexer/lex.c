#include "lexeme.h"
#include "lexer.h"
struct
{
    LexemeType type;
    char const *literal;
} keywords[] = {
    {UDLEX_NEWLINE, "\n"},
    {UDLEX_BREAK, "break"},
    {UDLEX_CASE, "case"},
    {UDLEX_CHAR, "char"},
    {UDLEX_CONTINUE, "continue"},
    {UDLEX_DEFAULT, "default"},
    {UDLEX_DO, "do"},
    {UDLEX_DOUBLE, "double"},
    {UDLEX_ELSE, "else"},
    {UDLEX_ENUM, "enum"},
    {UDLEX_EXTERN, "extern"},
    {UDLEX_FLOAT, "float"},
    {UDLEX_FOR, "for"},
    {UDLEX_IF, "if"},
    {UDLEX_INLINE, "inline"},
    {UDLEX_INT, "int"},
    {UDLEX_LONG, "long"},
    {UDLEX_LET, "let"},
    {UDLEX_CONST, "const"},
    {UDLEX_FN, "fn"},
    {UDLEX_RETURN, "return"},
    {UDLEX_SHORT, "short"},
    {UDLEX_SIGNED, "signed"},
    {UDLEX_SIZEOF, "sizeof"},
    {UDLEX_STATIC, "static"},
    {UDLEX_STRUCT, "struct"},
    {UDLEX_SWITCH, "switch"},
    {UDLEX_USING, "using"},
    {UDLEX__NULL, "null"},
    {UDLEX_UNION, "union"},
    {UDLEX_UNSIGNED, "unsigned"},
    {UDLEX_VOLATILE, "volatile"},
    {UDLEX_WHILE, "while"},
    {UDLEX_ALIGNAS, "alignas"},
    {UDLEX_ALIGNOF, "alignof"},
    {UDLEX_BOOL, "bool"},
    {UDLEX_LPARENT, "("},
    {UDLEX_RPARENT, ")"},
    {UDLEX_LBRACKET, "["},
    {UDLEX_RBRACKET, "]"},
    {UDLEX_COMMA, ","},
    {UDLEX_LBRACE, "{"},
    {UDLEX_RBRACE, "}"},
    {UDLEX_LSHIFT_EQ, "<<="},
    {UDLEX_RSHIFT_EQ, ">>="},
    {UDLEX_LSHIFT, "<<"},
    {UDLEX_RSHIFT, ">>"},
    {UDLEX_LCHEVRON, "<"},
    {UDLEX_RCHEVRON, ">"},
    {UDLEX_DOUBLE_POUND, "##"},
    {UDLEX_POUND, "#"},
    {UDLEX_PLUS_EQ, "+="},
    {UDLEX_PLUSPLUS, "++"},
    {UDLEX_PLUS, "+"},
    {UDLEX_ARROW, "->"},
    {UDLEX_MINUS_EQ, "-="},
    {UDLEX_MINUSMINUS, "--"},
    {UDLEX_MINUS, "-"},
    {UDLEX_STAR_EQ, "*="},
    {UDLEX_STAR, "*"},
    {UDLEX_SLASH_EQ, "/="},
    {UDLEX_SLASH, "/"},
    {UDLEX_PERCENT_EQ, "%="},
    {UDLEX_PERCENT, "%"},
    {UDLEX_AMPERSANDAMPERSAND, "&&"},
    {UDLEX_AMPERSAND_EQ, "&="},
    {UDLEX_AMPERSAND, "&"},
    {UDLEX_BARBAR, "||"},
    {UDLEX_BAR_EQ, "|="},
    {UDLEX_BAR, "|"},
    {UDLEX_CIRCUMFLEX_EQ, "^="},
    {UDLEX_CIRCUMFLEX, "^"},
    {UDLEX_EXCLAMATIONEQUAL, "!="},
    {UDLEX_EXCLAMATION, "!"},
    {UDLEX_QUESTION_MARK, "?"},
    {UDLEX_EQUALEQUAL, "=="},
    {UDLEX_EQUAL, "="},
    {UDLEX_TILDE, "~"},
    {UDLEX_DOT, "."},
    {UDLEX_SEMICOLON, ";"},
    {UDLEX_COLON, ":"},
};

static int isidentchar(char v)
{
    return isalpha(v) || v == '_' || v == '$' || isdigit(v);
}

static LexemeType udlex_impl(Scan *scan)
{

    if (isspace(scan_curr(scan)) && scan_curr(scan) != '\n')
    {
        while (isspace(scan_curr(scan)) && scan_curr(scan) != '\n' && !scan_ended(scan))
        {
            scan_next(scan);
        }

        return (UDLEX_WHITESPACE);
    }
    else if (scan_skip_word(scan, str$("/*")))
    {
        while (!scan_skip_word(scan, str$("*/")) && !scan_ended(scan))
        {
            scan_next(scan);
        }

        return (UDLEX_COMMENT);
    }
    else if (scan_skip_word(scan, str$("//")))
    {
        while (!scan_skip(scan, '\n') && !scan_ended(scan))
        {
            scan_next(scan);
        }

        return (UDLEX_COMMENT);
    }
    else if (scan_skip_word(scan, str$("\"")))
    {

        while (!scan_skip(scan, '"') && !scan_ended(scan))
        {
            scan_next(scan);
        }

        return (UDLEX_STRING);
    }
    else if (scan_skip_word(scan, str$("'")))
    {
        if (scan_curr(scan) != '\'') // case for just ''
        {
            scan_next(scan);
        }

        scan_skip(scan, '\'');
        return UDLEX_CHARACTER;
    }

    for (size_t i = 0; i < array_len$(keywords); i++)
    {
        if (scan_skip_word(scan, str$(keywords[i].literal)))
        {
            return (keywords[i].type);
        }
    }

    if (isdigit(scan_curr(scan)))
    {
        while (isdigit(scan_curr(scan)) && !scan_ended(scan))
        {
            scan_next(scan);
        }

        return UDLEX_INTEGER;
    }

    if (isidentchar(scan_curr(scan)))
    {
        while ((isidentchar(scan_curr(scan))) && !scan_ended(scan))
        {
            scan_next(scan);
        }

        return (UDLEX_IDENT);
    }

    return (LEXEME_INVALID);
}

Lex udlex(Scan *scan, Alloc *alloc)
{
    return lex(scan, udlex_impl, alloc);
}
