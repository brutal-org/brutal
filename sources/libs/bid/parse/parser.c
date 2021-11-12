#include <bid/ast/builder.h>
#include <bid/parse.h>
#include <brutal/debug.h>

static bool parser_skip_comment(Scan *scan)
{
    if (!scan_skip_word(scan, str$("/*")))
    {
        return false;
    }

    while (!scan_ended(scan))
    {
        if (scan_skip_word(scan, str$("*/")))
        {
            return true;
        }

        scan_next(scan);
    }

    return false;
}

static void skip_comment_and_space(Scan *scan)
{
    while (scan_eat(scan, isspace) ||
           parser_skip_comment(scan))
        ;
}

static bool skip_keyword(Scan *scan, char const *kw)
{
    skip_comment_and_space(scan);
    bool result = scan_skip_word(scan, str$(kw));
    skip_comment_and_space(scan);

    return result;
}

static bool skip_separator(Scan *scan, char sep)
{
    skip_comment_and_space(scan);
    bool result = scan_skip(scan, sep);
    skip_comment_and_space(scan);

    return result;
}

static bool expect_separator(Scan *scan, char sep)
{
    skip_comment_and_space(scan);
    bool result = scan_expect(scan, sep);
    skip_comment_and_space(scan);

    return result;
}

static int is_ident(int chr)
{
    return chr == '_' || isalnum(chr);
}

static BidType parse_type(Scan *scan, Alloc *alloc);

static Str parse_ident(Scan *scan)
{
    Str name = scan_skip_until(scan, is_ident);

    if (is_nullstr(name))
    {
        scan_throw(scan, str$("invalid ident"), name);
    }

    return name;
}

static BidType parse_primitive(Scan *scan, Alloc *alloc)
{
    BidType type = bid_primitive(parse_ident(scan), alloc);

    if (!skip_separator(scan, '('))
    {
        return type;
    }

    while (!skip_separator(scan, ')') && !scan_ended(scan))
    {
        bid_primitive_arg(&type, parse_ident(scan));
    }

    return type;
}

static void parse_enum_constant(Scan *scan, BidType *enum_)
{
    skip_separator(scan, '{');

    while (!skip_separator(scan, '}') && !scan_ended(scan))
    {
        bid_enum_constant(enum_, parse_ident(scan));
        skip_separator(scan, ',');
    }
}

static BidType parse_enum(Scan *scan, Alloc *alloc)
{
    BidType type = bid_enum(alloc);
    parse_enum_constant(scan, &type);
    return type;
}

static BidType parse_struct(Scan *scan, Alloc *alloc)
{
    BidType type = bid_struct(alloc);

    skip_separator(scan, '{');

    while (!skip_separator(scan, '}') && !scan_ended(scan))
    {
        Str member_name = parse_ident(scan);

        expect_separator(scan, ':');

        BidType member_type = parse_type(scan, alloc);

        bid_struct_member(&type, member_name, member_type);

        skip_separator(scan, ',');
    }

    return type;
}

static BidType parse_type(Scan *scan, Alloc *alloc)
{
    if (skip_keyword(scan, "struct") || skip_separator(scan, '{'))
    {
        return parse_struct(scan, alloc);
    }
    else if (skip_keyword(scan, "enum"))
    {
        return parse_enum(scan, alloc);
    }
    else if (is_ident(scan_curr(scan)))
    {
        return parse_primitive(scan, alloc);
    }
    else
    {
        scan_throw(scan, str$("Un expected token"), str$(""));
        return bid_none();
    }
}

static void parse_alias(Scan *scan, BidIface *iface, Alloc *alloc)
{
    Str name = parse_ident(scan);

    expect_separator(scan, ':');

    BidType type = parse_type(scan, alloc);

    bid_alias(iface, name, type);
}

static void parse_method(Scan *scan, BidIface *iface, Alloc *alloc)
{
    Str name = parse_ident(scan);

    skip_comment_and_space(scan);

    BidType request = parse_type(scan, alloc);

    skip_keyword(scan, "->");

    BidType response = parse_type(scan, alloc);

    bid_method(iface, name, request, response);
}

BidIface bid_parse(Scan *scan, Alloc *alloc)
{
    BidIface iface = bid_iface(alloc);

    skip_keyword(scan, "interface");

    bid_iface_name(&iface, parse_ident(scan));

    expect_separator(scan, '{');

    while (scan_curr(scan) != '}' && !scan_ended(scan))
    {
        if (skip_keyword(scan, "errors"))
        {
            parse_enum_constant(scan, &iface.errors);
        }
        else if (skip_keyword(scan, "type"))
        {
            parse_alias(scan, &iface, alloc);
        }
        else if (skip_keyword(scan, "method"))
        {
            parse_method(scan, &iface, alloc);
        }
        else if (skip_keyword(scan, "id"))
        {
            expect_separator(scan, '=');

            scan_expect_word(scan, str$("0x"));

            uint32_t id = 0;

            while (isxdigit(scan_curr(scan)))
            {
                id *= 0x10;
                id += scan_next_digit(scan);
            }

            iface.id = id;
        }
        else
        {
            scan_throw(scan, str$("expected errors/type/method"), parse_ident(scan));
        }

        expect_separator(scan, ';');
    }

    expect_separator(scan, '}');

    return iface;
}
