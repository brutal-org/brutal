#include <cc/lex.h>

static LexemeType c_lex_impl(Scan *scan)
{
    char c = scan_curr(scan);

    if (isalpha(c) || c == '_')
    {
        while (isalnum(scan_curr(scan)) || scan_curr(scan) == '_')
        {
            scan_next(scan);
        }

        return C_LEXEME_IDENTIFIER;
    }
    else if (isspace(c))
    {
        while (isspace(scan_curr(scan)))
        {
            scan_next(scan);
        }

        return C_LEXEME_WHITESPACE;
    }
    else if (scan_skip_word(scan, str$("/*")))
    {
        while (!scan_skip_word(scan, str$("*/")) && !scan_ended(scan))
        {
            scan_next(scan);
        }

        return C_LEXEME_COMMENT;
    }
    else if (scan_skip_word(scan, str$("//")))
    {
        while (!scan_skip(scan, '\n') && !scan_ended(scan))
        {
            scan_next(scan);
        }

        return C_LEXEME_COMMENT;
    }
    else
    {
        scan_next(scan);
        return LEXEME_INVALID;
    }
}

Lex c_lex(Scan *scan, Alloc *alloc)
{
    return lex(scan, c_lex_impl, alloc);
}
