#include <brutal/debug.h>
#include <cc/parse/lexer.h>

struct
{
    LexemeType type;
    char const *literal;
} keywords[] = {
    {CLEX_AUTO, "auto"},
    {CLEX_BREAK, "break"},
    {CLEX_CASE, "case"},
    {CLEX_CHAR, "char"},
    {CLEX_CONST, "const"},
    {CLEX_CONTINUE, "continue"},
    {CLEX_DEFAULT, "default"},
    {CLEX_DO, "do"},
    {CLEX_DOUBLE, "double"},
    {CLEX_ELSE, "else"},
    {CLEX_ENUM, "enum"},
    {CLEX_EXTERN, "extern"},
    {CLEX_FLOAT, "float"},
    {CLEX_FOR, "for"},
    {CLEX_GOTO, "goto"},
    {CLEX_IF, "if"},
    {CLEX_INLINE, "inline"},
    {CLEX_INT, "int"},
    {CLEX_LONG, "long"},
    {CLEX_REGISTER, "register"},
    {CLEX_RESTRICT, "restrict"},
    {CLEX_RETURN, "return"},
    {CLEX_SHORT, "short"},
    {CLEX_SIGNED, "signed"},
    {CLEX_SIZEOF, "sizeof"},
    {CLEX_STATIC, "static"},
    {CLEX_STRUCT, "struct"},
    {CLEX_SWITCH, "switch"},
    {CLEX_TYPEDEF, "typedef"},
    {CLEX_UNION, "union"},
    {CLEX_UNSIGNED, "unsigned"},
    {CLEX_VOID, "void"},
    {CLEX_VOLATILE, "volatile"},
    {CLEX_WHILE, "while"},
    {CLEX_ALIGNAS, "_Alignas"},
    {CLEX_ALIGNOF, "_Alignof"},
    {CLEX_ATOMIC, "_Atomic"},
    {CLEX_BOOL, "_Bool"},
    {CLEX_GENERIC, "_Generic"},
    {CLEX_NORETURN, "_Noreturn"},
    {CLEX_STATIC_ASSERT, "_Static_assert"},
    {CLEX_THREAD_LOCAL, "_Thread_local"},
    {CLEX_LPARENT, "("},
    {CLEX_RPARENT, ")"},
    {CLEX_LBRACKET, "["},
    {CLEX_RBRACKET, "]"},
    {CLEX_COMMA, ","},
    {CLEX_LBRACE, "{"},
    {CLEX_RBRACE, "}"},
    {CLEX_LCHEVRON, "<"},
    {CLEX_RCHEVRON, ">"},
    {CLEX_POUND, "#"},
    {CLEX_PLUSPLUS, "++"},
    {CLEX_PLUS, "+"},
    {CLEX_ARROW, "->"},
    {CLEX_MINUSMINUS, "--"},
    {CLEX_MINUS, "-"},
    {CLEX_EQUALEQUAL, "=="},
    {CLEX_EQUAL, "="},
    {CLEX_STAR, "*"},
    {CLEX_SLASH, "/"},
    {CLEX_PERCENT, "%"},
    {CLEX_AMPERSANDAMPERSAND, "&&"},
    {CLEX_AMPERSAND, "&"},
    {CLEX_BAR, "|"},
    {CLEX_CIRCUMFLEX, "^"},
    {CLEX_TILDE, "~"},
    {CLEX_EXCLAMATION, "!"},
    {CLEX_DOT, "."},
    {CLEX_SEMICOLON, ";"},
};

static LexemeType clex_impl(Scan *scan)
{
    if (isspace(scan_curr(scan)))
    {
        while (isspace(scan_curr(scan)))
        {
            scan_next(scan);
        }

        return (CLEX_WHITESPACE);
    }
    else if (scan_skip_word(scan, str$("/*")))
    {
        while (!scan_skip_word(scan, str$("*/")) && !scan_ended(scan))
        {
            scan_next(scan);
        }

        return (CLEX_COMMENT);
    }
    else if (scan_skip_word(scan, str$("//")))
    {
        while (!scan_skip(scan, '\n') && !scan_ended(scan))
        {
            scan_next(scan);
        }

        return (CLEX_COMMENT);
    }
    else if (scan_skip_word(scan, str$("\"")))
    {

        while (!scan_skip(scan, '"') && !scan_ended(scan))
        {
            scan_next(scan);
        }

        return (CLEX_STRING);
    }
    else if (scan_skip_word(scan, str$("'")))
    {
        if (scan_curr(scan) != '\'') // case for just ''
        {
            scan_next(scan);
        }

        scan_skip(scan, '\'');
        return CLEX_CHARACTER;
    }

    for (size_t i = 0; i < ARRAY_LENGTH(keywords); i++)
    {
        if (scan_skip_word(scan, str$(keywords[i].literal)))
        {
            return (keywords[i].type);
        }
    }

    if (isalpha(scan_curr(scan)) || scan_curr(scan) == '_')
    {
        while (isalnum(scan_curr(scan)) || scan_curr(scan) == '_')
        {
            scan_next(scan);
        }

        return (CLEX_ATOM);
    }

    if (isdigit(scan_curr(scan)))
    {
        while (isdigit(scan_curr(scan)) && !scan_ended(scan))
        {
            scan_next(scan);
        }

        return CLEX_INTEGER;
    }

    return (LEXEME_INVALID);
}

Lex clex(Scan *scan, Alloc *alloc)
{
    return lex(scan, clex_impl, alloc);
}
