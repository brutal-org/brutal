#include <brutal/alloc.h>
#include <brutal/log.h>
#include <udfore/udfore.h>

static UdExpr parse_expression(Scan *scan, Alloc *alloc);

static bool skip_keyword(Scan *scan, const char *expected)
{
    return scan_skip_word(scan, str$(expected));
}

static bool skip_space(Scan *scan)
{
    return scan_eat(scan, isspace);
}

static void skip_nested_comment(Scan *scan)
{
    while (!skip_keyword(scan, "*/") && !scan_end(scan))
    {
        scan_next(scan);
        if (skip_keyword(scan, "/*"))
        {
            skip_nested_comment(scan);
        }
    }
}

static bool skip_comment(Scan *scan)
{
    if (skip_keyword(scan, "//!"))
    {
        skip_space(scan);

        struct alloc_heap heap;
        alloc_heap_init(&heap);
        parse_expression(scan, base$(&heap));
        alloc_heap_deinit(&heap);

        return true;
    }
    else if (skip_keyword(scan, "//"))
    {
        while (scan_next(scan) != '\n' && !scan_end(scan))
        {
            if (skip_keyword(scan, "/*"))
            {
                skip_nested_comment(scan);
            }
        }

        return true;
    }
    else if (skip_keyword(scan, "/*"))
    {
        skip_nested_comment(scan);
        return true;
    }
    else
    {
        return false;
    }
}

static void skip_atmosphere(Scan *scan)
{
    while (skip_space(scan) || skip_comment(scan))
        ;
}

static int isany(int c, char const *what)
{
    for (size_t i = 0; what[i]; i++)
    {
        if (what[i] == c)
        {
            return true;
        }
    }

    return false;
}

static int is_num(int c)
{
    return isdigit(c) | isany(c, "+-");
}

static int is_atom_start(int c)
{
    return isalpha(c) | isany(c, "!$%&*/<=>?^_~");
}

static int is_atom(int c)
{
    return is_atom_start(c) || isdigit(c) || isany(c, "+-.@");
}

static UdExpr parse_atom(Scan *scan)
{
    scan_begin_token(scan);
    scan_next(scan);
    scan_skip_until(scan, is_atom);
    return ud_atom(scan_end_token(scan));
}

static UdExpr parse_rune(Scan *scan)
{
    if (skip_keyword(scan, "alarm"))
    {
        return ud_rune('\a');
    }
    else if (skip_keyword(scan, "backspace"))
    {
        return ud_rune('\xb');
    }
    else if (skip_keyword(scan, "delete"))
    {
        return ud_rune('\xff');
    }
    else if (skip_keyword(scan, "escape"))
    {
        return ud_rune('\e');
    }
    else if (skip_keyword(scan, "newline"))
    {
        return ud_rune('\n');
    }
    else if (skip_keyword(scan, "null"))
    {
        return ud_rune('\0');
    }
    else if (skip_keyword(scan, "return"))
    {
        return ud_rune('\r');
    }
    else if (skip_keyword(scan, "space"))
    {
        return ud_rune(' ');
    }
    else if (skip_keyword(scan, "tab"))
    {
        return ud_rune('\t');
    }

    if (scan_curr(scan) == 'x' && isxdigit(scan_peek(scan, 1)))
    {
        Rune rune = 0;

        while (isxdigit(scan_curr(scan)) && !scan_end(scan))
        {
            rune *= 16;
            rune += scan_next_digit(scan);
        }

        return ud_rune(rune);
    }

    return ud_rune(scan_next(scan));
}

static UdExpr parse_pair(Scan *scan, Alloc *alloc)
{
    if (scan_skip(scan, ')') || scan_end(scan))
    {
        return ud_nil();
    }

    UdExpr lhs = parse_expression(scan, alloc);
    skip_atmosphere(scan);
    UdExpr rhs = parse_pair(scan, alloc);

    return ud_pair(alloc, lhs, rhs);
}

static UdExpr parse_line(Scan *scan, Alloc *alloc)
{
    if (scan_curr(scan) == ';' || scan_end(scan))
    {
        return ud_nil();
    }

    UdExpr lhs = parse_expression(scan, alloc);
    skip_atmosphere(scan);
    UdExpr rhs = parse_line(scan, alloc);

    if (ud_is(rhs, UD_NIL))
    {
        return lhs;
    }

    return ud_pair(alloc, lhs, rhs);
}

static UdExpr parse_block(Scan *scan, Alloc *alloc)
{
    UdExpr body = ud_nil();

    skip_atmosphere(scan);

    while (scan_curr(scan) != '}' && !scan_end(scan))
    {
        UdExpr expr = ud_nil();

        expr = parse_line(scan, alloc);

        if (!scan_expect(scan, ';'))
        {
            return ud_nil();
        }

        body = ud_pair(alloc, expr, body);

        skip_atmosphere(scan);
    }

    scan_expect(scan, '}');

    return ud_pair(
        alloc,
        ud_atom(str$("bs-builtin-block")),
        body);
}

static UdExpr parse_expression(Scan *scan, Alloc *alloc)
{
    if (skip_keyword(scan, "#true") || skip_keyword(scan, "#t"))
    {
        return ud_bool(true);
    }
    else if (skip_keyword(scan, "#false") || skip_keyword(scan, "#f"))
    {
        return ud_bool(false);
    }
    else if (is_atom_start(scan_curr(scan)))
    {
        return parse_atom(scan);
    }
    else if (is_num(scan_curr(scan)))
    {
        return ud_num(scan_next_decimal(scan));
    }
    else if (skip_keyword(scan, "#\\"))
    {
        return parse_rune(scan);
    }
    else if (scan_skip(scan, '('))
    {
        skip_atmosphere(scan);
        return parse_pair(scan, alloc);
    }
    else if (scan_skip(scan, '{'))
    {
        skip_atmosphere(scan);
        return parse_block(scan, alloc);
    }
    else if (scan_skip(scan, '\''))
    {
        return ud_pair(
            alloc,
            ud_atom(str$("bs-builtin-quote")),
            parse_expression(scan, alloc));
    }
    else
    {
        char c = scan_curr(scan);
        scan_throw(scan, str$("unexpected-character"), str$_n(1, &c));
        return ud_nil();
    }
}

static UdExpr parse_module(Scan *scan, Alloc *alloc)
{
    UdExpr body = ud_nil();

    skip_atmosphere(scan);

    while (!scan_end(scan))
    {
        UdExpr expr = ud_nil();

        expr = parse_line(scan, alloc);

        if (!scan_expect(scan, ';'))
        {
            return ud_nil();
        }

        body = ud_pair(alloc, expr, body);

        skip_atmosphere(scan);
    }

    return ud_pair(
        alloc,
        ud_atom(str$("bs-builtin-block")),
        body);
}

UdExpr ud_parse(Scan *scan, Alloc *alloc)
{
    skip_atmosphere(scan);
    return parse_module(scan, alloc);
}
