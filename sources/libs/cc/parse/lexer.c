#include <cc/parse/lexer.h>

static LexemeType c_lex_impl(Scan *scan)
{
    if (isalpha(scan_curr(scan)) || scan_curr(scan) == '_')
    {
        while (isalnum(scan_curr(scan)) || scan_curr(scan) == '_')
        {
            scan_next(scan);
        }

        return CLEX_ATOM;
    }
    else if (isspace(scan_curr(scan)))
    {
        while (isspace(scan_curr(scan)))
        {
            scan_next(scan);
        }

        return CLEX_WHITESPACE;
    }
    else if (scan_skip_word(scan, str$("/*")))
    {
        while (!scan_skip_word(scan, str$("*/")) && !scan_ended(scan))
        {
            scan_next(scan);
        }

        return CLEX_COMMENT;
    }
    else if (scan_skip_word(scan, str$("//")))
    {
        while (!scan_skip(scan, '\n') && !scan_ended(scan))
        {
            scan_next(scan);
        }

        return CLEX_COMMENT;
    }
    else
    {
        switch (scan_next(scan))
        {
        case '(':
            return CLEX_LPARENT;

        case ')':
            return CLEX_RPARENT;

        case '[':
            return CLEX_LBRACKET;

        case ']':
            return CLEX_RBRACKET;

        case '{':
            return CLEX_LBRACE;

        case '}':
            return CLEX_RBRACE;

        case '<':
            return CLEX_LCHEVRON;

        case '>':
            return CLEX_RCHEVRON;

        case '#':
            return CLEX_POUND;

        case '+':
            return CLEX_PLUS;

        case '-':
            return CLEX_MINUS;

        case '=':
            return CLEX_EQUAL;

        case '*':
            return CLEX_STAR;

        case '/':
            return CLEX_SLASH;

        case '%':
            return CLEX_PERCENT;

        case '&':
            return CLEX_AMPERSAND;

        case '|':
            return CLEX_BAR;

        case '^':
            return CLEX_CIRCUMFLEX;

        case '~':
            return CLEX_TILDE;

        case '!':
            return CLEX_EXCLAMATION;

        case '.':
            return CLEX_DOT;

        default:
            return LEXEME_INVALID;
        };
    }
}

Lex c_lex(Scan *scan, Alloc *alloc)
{
    return lex(scan, c_lex_impl, alloc);
}
