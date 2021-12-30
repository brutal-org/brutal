#include <embed/log.h>
#include <ud/ast.h>
#include <ud/parse/lexer.h>

void ud_parse_whitespace(Lex *lex)
{
    while (lex_curr_type(lex) == UDLEX_COMMENT ||
           lex_curr_type(lex) == UDLEX_WHITESPACE)
    {
        lex_next(lex);
    }
}

bool ud_parse_expect_separator(Lex *lex, LexemeType type)
{
    ud_parse_whitespace(lex);
    bool result = lex_expect(lex, type);
    ud_parse_whitespace(lex);
    return result;
}

bool ud_parse_skip_separator(Lex *lex, LexemeType type)
{
    ud_parse_whitespace(lex);

    bool result = lex_skip_type(lex, type);

    ud_parse_whitespace(lex);

    return result;
}

bool ud_parse_is_separator(Lex *lex, LexemeType type)
{
    ud_parse_whitespace(lex);
    return lex_curr_type(lex) == type;
}

bool ud_expect(Lex *lex, LexemeType type)
{
    if (lex_expect(lex, type))
    {
        return true;
    }

    print(embed_log_writer(), "Error while parsing: expected '{}' at {}:{}\n", udlex_to_str(type), lex_curr(lex).line, lex_curr(lex).col);

    return false;
}
