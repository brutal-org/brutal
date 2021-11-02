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

static BidPrimitive parse_primitive(Scan *scan, Alloc *alloc)
{
    BidPrimitive type = {};
    type.name = parse_ident(scan);

    if (!skip_separator(scan, '('))
    {
        return type;
    }

    vec_init(&type.args, alloc);

    while (!skip_separator(scan, ')') && !scan_ended(scan))
    {
        vec_push(&type.args, parse_ident(scan));
        skip_separator(scan, ',');
    }

    return type;
}

static BidEnum parse_enum(Scan *scan, Alloc *alloc)
{
    BidEnum type = {};

    vec_init(&type.members, alloc);

    skip_separator(scan, '{');

    while (!skip_separator(scan, '}') && !scan_ended(scan))
    {
        vec_push(&type.members, parse_ident(scan));
        skip_separator(scan, ',');
    }

    return type;
}

static BidStruct parse_struct(Scan *scan, Alloc *alloc)
{
    BidStruct type = {};
    vec_init(&type.members, alloc);

    skip_separator(scan, '{');

    while (!skip_separator(scan, '}') && !scan_ended(scan))
    {
        BidMember member = {};

        member.name = parse_ident(scan);

        expect_separator(scan, ':');

        member.type = parse_type(scan, alloc);

        vec_push(&type.members, member);

        skip_separator(scan, ',');
    }

    return type;
}

static BidType parse_type(Scan *scan, Alloc *alloc)
{
    BidType type = {};

    type.type = BID_TYPE_NONE;

    if (skip_keyword(scan, "struct") || skip_separator(scan, '{'))
    {
        type.type = BID_TYPE_STRUCT;
        type.struct_ = parse_struct(scan, alloc);
    }
    else if (skip_keyword(scan, "enum"))
    {
        type.type = BID_TYPE_ENUM;
        type.enum_ = parse_enum(scan, alloc);
    }
    else if (is_ident(scan_curr(scan)))
    {
        skip_comment_and_space(scan);
        type.type = BID_TYPE_PRIMITIVE;
        type.primitive_ = parse_primitive(scan, alloc);
    }

    skip_comment_and_space(scan);

    return type;
}

static BidAlias parse_alias(Scan *scan, Alloc *alloc)
{
    BidAlias alias = {};

    alias.name = parse_ident(scan);

    expect_separator(scan, ':');

    alias.type = parse_type(scan, alloc);

    return alias;
}

static BidEvent parse_event(Scan *scan, Alloc *alloc)
{
    BidEvent event = {};

    event.name = parse_ident(scan);

    skip_comment_and_space(scan);

    event.data = parse_type(scan, alloc);

    return event;
}

static BidMethod parse_method(Scan *scan, Alloc *alloc)
{
    BidMethod method = {};

    method.name = parse_ident(scan);

    skip_comment_and_space(scan);

    method.request = parse_type(scan, alloc);

    skip_keyword(scan, "->");

    method.response = parse_type(scan, alloc);

    return method;
}

BidIface bid_parse(Scan *scan, Alloc *alloc)
{
    BidIface iface = {};

    vec_init(&iface.aliases, alloc);
    vec_init(&iface.events, alloc);
    vec_init(&iface.methods, alloc);

    skip_keyword(scan, "interface");

    iface.name = parse_ident(scan);

    expect_separator(scan, '{');

    while (scan_curr(scan) != '}' && !scan_ended(scan))
    {
        if (skip_keyword(scan, "errors"))
        {
            iface.errors = parse_enum(scan, alloc);
        }
        else if (skip_keyword(scan, "type"))
        {
            vec_push(&iface.aliases, parse_alias(scan, alloc));
        }
        else if (skip_keyword(scan, "event"))
        {
            vec_push(&iface.events, parse_event(scan, alloc));
        }
        else if (skip_keyword(scan, "method"))
        {
            vec_push(&iface.methods, parse_method(scan, alloc));
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
            scan_throw(scan, str$("expected errors/type/event/method"), parse_ident(scan));
        }

        expect_separator(scan, ';');
    }

    expect_separator(scan, '}');

    vec_push(&iface.errors.members, str$("UNEXPECTED_MESSAGE"));
    vec_push(&iface.errors.members, str$("BAD_COMMUNICATION"));
    vec_push(&iface.errors.members, str$("SUCCESS"));

    return iface;
}
