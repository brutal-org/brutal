#include <brutal/debug.h>
#include <idl/ast/builder.h>
#include <idl/parse.h>

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

static IdlType parse_type(Scan *scan, Alloc *alloc);

static Str parse_ident(Scan *scan)
{
    Str name = scan_skip_until(scan, is_ident);

    if (is_nullstr(name))
    {
        scan_throw(scan, str$("invalid ident"), name);
    }

    return name;
}

static IdlType parse_primitive(Scan *scan)
{
    Str ident = parse_ident(scan);

    if (str_eq(ident, str$("Nil")))
    {
        return idl_nil();
    }

    return idl_primitive(ident);
}

static void parse_enum_constant(Scan *scan, IdlType *enum_)
{
    skip_separator(scan, '{');

    while (!skip_separator(scan, '}') && !scan_ended(scan))
    {
        idl_enum_constant(enum_, parse_ident(scan));
        skip_separator(scan, ',');
    }
}

static IdlType parse_enum(Scan *scan, Alloc *alloc)
{
    IdlType type = idl_enum(alloc);
    parse_enum_constant(scan, &type);
    return type;
}

static IdlType parse_struct(Scan *scan, Alloc *alloc)
{
    IdlType type = idl_struct(alloc);

    expect_separator(scan, '{');

    while (!skip_separator(scan, '}') && !scan_ended(scan))
    {
        Str member_name = parse_ident(scan);

        expect_separator(scan, ':');

        IdlType member_type = parse_type(scan, alloc);

        idl_struct_member(&type, member_name, member_type);

        skip_separator(scan, ',');
    }

    return type;
}

static IdlType parse_vec(IdlType subtype, Scan *scan, Alloc *alloc)
{
    skip_comment_and_space(scan);

    if (!skip_separator(scan, '['))
    {
        return subtype;
    }

    expect_separator(scan, ']');

    return idl_vec(subtype, alloc);
}

static IdlType parse_type(Scan *scan, Alloc *alloc)
{
    IdlType type = idl_nil();

    if (skip_keyword(scan, "struct") || scan_curr(scan) == '{')
    {
        type = parse_struct(scan, alloc);
    }
    else if (skip_keyword(scan, "enum"))
    {
        type = parse_enum(scan, alloc);
    }
    else if (is_ident(scan_curr(scan)))
    {
        type = parse_primitive(scan);
    }
    else
    {
        scan_throw(scan, str$("Un expected token"), str$(""));
    }

    return parse_vec(type, scan, alloc);
}

static void parse_alias(Scan *scan, IdlIface *iface, Alloc *alloc)
{
    Str name = parse_ident(scan);

    expect_separator(scan, ':');

    IdlType type = parse_type(scan, alloc);

    idl_alias(iface, name, type);
}

static void parse_method_body(Scan *scan, Str name, IdlIface *iface, Alloc *alloc)
{
    IdlType request = idl_nil();
    IdlType response = idl_nil();

    if (skip_keyword(scan, "->"))
    {
        response = parse_type(scan, alloc);
    }
    else
    {
        request = parse_type(scan, alloc);

        if (skip_keyword(scan, "->"))
        {
            response = parse_type(scan, alloc);
        }
    }

    idl_method(iface, name, request, response);
}

static void parse_method(Scan *scan, IdlIface *iface, Alloc *alloc)
{
    Str name = parse_ident(scan);
    return parse_method_body(scan, name, iface, alloc);
}

IdlIface idl_parse(Scan *scan, Alloc *alloc)
{
    skip_keyword(scan, "interface");

    bool is_methode = skip_keyword(scan, "method");

    Str name = parse_ident(scan);

    Str name_pascal = case_change_str(CASE_PASCAL, name, alloc);
    IdlIface iface = idl_iface(name_pascal, alloc);

    if (!is_methode)
    {
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
            else
            {
                scan_throw(scan, str$("expected errors/type/method"), parse_ident(scan));
            }

            expect_separator(scan, ';');
        }

        expect_separator(scan, '}');
    }
    else
    {
        parse_method_body(scan, name, &iface, alloc);
    }

    return iface;
}
