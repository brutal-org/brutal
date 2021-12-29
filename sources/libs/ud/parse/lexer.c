#include <ud/parse/lexer.h>

struct
{
    LexemeType type;
    char const *literal;
} keywords[] = {
    {UDLEX_LET, "let"},
    {UDLEX_FUNC, "func"},
    {UDLEX_COLON, ":"},
    {UDLEX_MINUS, "-"},
    {UDLEX_PLUS, "+"},
    {UDLEX_STAR, "*"},
    {UDLEX_SLASH, "/"},
    {UDLEX_BOOL, "bool"},
    {UDLEX_LPAREN, "("},
    {UDLEX_RPAREN, ")"},
    {UDLEX_LBRACKET, "["},
    {UDLEX_RBRACKET, "]"},
    {UDLEX_COMMA, ","},
    {UDLEX_LCBRACE, "{"},
    {UDLEX_RCBRACE, "}"},
    {UDLEX_THIN_ARROW, "->"},
    {UDLEX_DOUBLE_EQUAL, "=="},
    {UDLEX_EQUAL, "="},
    {UDLEX_SEMICOLON, ";"},
    {UDLEX_INT, "int"},
    {UDLEX_UINT, "uint"}};

static int isidentchar(char v)
{
    return isalpha(v) || v == '_' || v == '$' || isdigit(v);
}

static LexemeType ud_lex_impl(Scan *scan)
{
    if (isspace(scan_curr(scan)))
    {
        while (isspace(scan_curr(scan)) && !scan_ended(scan))
        {
            scan_next(scan);
        }

        return (UDLEX_WHITESPACE);
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

    else if (scan_skip_word(scan, str$("->")))
    {
        return (UDLEX_THIN_ARROW);
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

    for (size_t i = 0; i < ARRAY_LEN(keywords); i++)
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

Lex ud_lex(Scan *scan, Alloc *alloc)
{
    return lex(scan, ud_lex_impl, alloc);
}
