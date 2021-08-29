#include <bid/bid.h>
#include <brutal/log.h>

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

static int is_identifier(int chr)
{
    return chr == '_' || isalnum(chr);
}

static BidType parse_type(Scan *scan, Alloc *alloc);

static Str parse_identifier(Scan *scan)
{
    Str name = scan_skip_until(scan, is_identifier);

    if (name.len == 0)
    {
        scan_throw(scan, str$("invalid identifier"), name);
    }

    return name;
}

static BidGeneric parse_generic(Scan *scan, Alloc *alloc)
{
    BidGeneric type = {};
    type.name = parse_identifier(scan);

    skip_comment_and_space(scan);

    if (!scan_skip(scan, '<'))
    {
        return type;
    }

    skip_comment_and_space(scan);

    vec_init(&type.params, alloc);

    while (scan_curr(scan) != '>' && !scan_ended(scan))
    {
        vec_push(&type.params, parse_type(scan, alloc));

        skip_comment_and_space(scan);

        scan_expect(scan, ',');

        skip_comment_and_space(scan);
    }

    scan_expect(scan, '>');

    return type;
}

static BidEnum parse_enum(Scan *scan, Alloc *alloc)
{
    BidEnum type = {};
    vec_init(&type.members, alloc);

    scan_skip(scan, '{');
    skip_comment_and_space(scan);

    while (scan_curr(scan) != '}' && !scan_ended(scan))
    {
        vec_push(&type.members, parse_identifier(scan));

        skip_comment_and_space(scan);
        scan_skip(scan, ',');
        skip_comment_and_space(scan);
    }

    scan_skip(scan, '}');

    return type;
}

static BidStruct parse_struct(Scan *scan, Alloc *alloc)
{
    BidStruct type = {};
    vec_init(&type.members, alloc);

    scan_skip(scan, '{');
    skip_comment_and_space(scan);

    while (scan_curr(scan) != '}' && !scan_ended(scan))
    {
        BidVar var = {};
        var.name = parse_identifier(scan);

        skip_comment_and_space(scan);
        scan_expect(scan, ':');
        skip_comment_and_space(scan);

        var.type = parse_type(scan, alloc);

        vec_push(&type.members, var);

        skip_comment_and_space(scan);
        scan_skip(scan, ',');
        skip_comment_and_space(scan);
    }

    scan_skip(scan, '}');

    return type;
}

static BidType parse_type(Scan *scan, Alloc *alloc)
{
    BidType type = {};

    if (scan_skip_word(scan, str$("struct")) || scan_curr(scan) == '{')
    {
        skip_comment_and_space(scan);
        type.type = BID_TYPE_STRUCT;
        type.struct_ = parse_struct(scan, alloc);
    }
    else if (scan_skip_word(scan, str$("enum")))
    {
        skip_comment_and_space(scan);
        type.type = BID_TYPE_ENUM;
        type.enum_ = parse_enum(scan, alloc);
    }
    else
    {
        skip_comment_and_space(scan);
        type.type = BID_TYPE_GENERIC;
        type.generic_ = parse_generic(scan, alloc);
    }

    return type;
}

static BidAlias parse_alias(Scan *scan, Alloc *alloc)
{
    BidAlias alias = {};

    alias.name = parse_identifier(scan);

    skip_comment_and_space(scan);
    scan_skip(scan, ':');
    skip_comment_and_space(scan);

    alias.type = parse_type(scan, alloc);

    return alias;
}

static BidEvent parse_event(Scan *scan, Alloc *alloc)
{
    BidEvent event = {};

    event.name = parse_identifier(scan);

    skip_comment_and_space(scan);

    event.data = parse_type(scan, alloc);

    return event;
}

static BidMethod parse_method(Scan *scan, Alloc *alloc)
{
    BidMethod method = {};

    method.name = parse_identifier(scan);

    skip_comment_and_space(scan);

    method.request = parse_type(scan, alloc);

    skip_comment_and_space(scan);

    scan_skip_word(scan, str$("->"));

    skip_comment_and_space(scan);

    method.response = parse_type(scan, alloc);

    return method;
}

BidInterface bid_parse(Scan *scan, Alloc *alloc)
{
    BidInterface interface = {};

    vec_init(&interface.aliases, alloc);
    vec_init(&interface.events, alloc);
    vec_init(&interface.methods, alloc);

    skip_comment_and_space(scan);

    scan_expect_word(scan, str$("interface"));

    skip_comment_and_space(scan);

    interface.name = parse_identifier(scan);

    skip_comment_and_space(scan);

    scan_expect(scan, '{');

    skip_comment_and_space(scan);

    while (scan_curr(scan) != '}' && !scan_ended(scan))
    {
        if (scan_skip_word(scan, str$("errors")))
        {
            skip_comment_and_space(scan);
            interface.errors = parse_enum(scan, alloc);
            vec_push(&interface.errors.members, str$("UNEXPECTED_MESSAGE"));
            vec_push(&interface.errors.members, str$("BAD_COMMUNICATION"));
            vec_push(&interface.errors.members, str$("SUCCESS"));
        }
        else if (scan_skip_word(scan, str$("type")))
        {
            skip_comment_and_space(scan);
            vec_push(&interface.aliases, parse_alias(scan, alloc));
        }
        else if (scan_skip_word(scan, str$("event")))
        {
            skip_comment_and_space(scan);
            vec_push(&interface.events, parse_event(scan, alloc));
        }
        else if (scan_skip_word(scan, str$("method")))
        {
            skip_comment_and_space(scan);
            vec_push(&interface.methods, parse_method(scan, alloc));
        }
        else if (scan_skip_word(scan, str$("id")))
        {
            skip_comment_and_space(scan);
            scan_expect(scan, '=');
            skip_comment_and_space(scan);
            scan_expect_word(scan, str$("0x"));

            uint32_t id = 0;

            while (isxdigit(scan_curr(scan)))
            {
                id *= 0x10;
                id += scan_next_digit(scan);
            }

            interface.id = id;
        }
        else
        {
            scan_throw(scan, str$("expected errors/type/event/method"), parse_identifier(scan));
        }

        skip_comment_and_space(scan);

        scan_expect(scan, ';');

        skip_comment_and_space(scan);
    }

    scan_expect(scan, '}');

    return interface;
}
