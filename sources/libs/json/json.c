#include <ansi/ctype.h>
#include <json/json.h>

static bool json_parse_value(Scan *scan, Alloc *alloc, struct json_object *from);

static int is_closing_string(int c)
{
    if (c == '"')
    {
        return false;
    }
    return true;
}

static Str json_parse_str(Scan *scan)
{
    scan_expect(scan, '"');

    Str name = scan_skip_until(scan, is_closing_string);

    scan_expect(scan, '"');

    return name;
}

static char json_parse_char(Scan *scan)
{
    scan_expect(scan, '\'');

    char res = scan_next(scan);

    scan_expect(scan, '\'');

    return res;
}

static void json_parse_array(Scan *scan, Alloc *alloc, struct json_object *from)
{
    vec_init(&from->array.elements, alloc);

    while (scan_curr(scan) != ']' && !scan_end(scan))
    {
        struct json_object res;

        if (!json_parse_value(scan, alloc, &res))
        {
            scan_throw(scan, str_cast("expected a value"), str_cast(""));
            return;
        }

        scan_skip_space(scan);

        vec_push(&from->array.elements, res);

        if (!scan_skip(scan, ','))
        {
            return;
        }
    }
}

static void json_parse_member(Scan *scan, Alloc *alloc, json_members *from)
{
    Str name = json_parse_str(scan);
    scan_skip_space(scan);
    scan_expect(scan, ':');

    struct json_object value;
    if (!json_parse_value(scan, alloc, &value))
    {
        scan_throw(scan, str_cast("expected a value"), str_cast(""));
        return;
    }

    map_put(from, name, value);
}

static void json_parse_object(Scan *scan, Alloc *alloc, struct json_object *from)
{
    map_init(&from->object.members, alloc);

    while (scan_curr(scan) != '}' && !scan_end(scan))
    {
        scan_skip_space(scan);

        json_parse_member(scan, alloc, &from->object.members);

        scan_skip_space(scan);

        if (!scan_skip(scan, ','))
        {
            return;
        }
    }
}

static bool json_parse_value(Scan *scan, Alloc *alloc, struct json_object *from)
{
    scan_skip_space(scan);

    if (scan_curr(scan) == '"') // string
    {
        from->type = JSON_VALUE_STRING;
        from->string = json_parse_str(scan);
    }
    else if (scan_curr(scan) == '\'') // char
    {
        from->type = JSON_VALUE_CHAR;
        from->character = json_parse_char(scan);
    }
    else if (isdigit(scan_curr(scan)) || scan_curr(scan) == '-') // number
    {
        from->type = JSON_VALUE_NUMBER;
        from->number = scan_next_decimal(scan);
    }
    else if (scan_skip_word(scan, str_cast("true"))) // boolean (true)
    {
        from->type = JSON_VALUE_BOOL;
        from->boolean = true;
    }
    else if (scan_skip_word(scan, str_cast("false"))) // boolean (false)
    {
        from->type = JSON_VALUE_BOOL;
        from->boolean = false;
    }
    else if (scan_skip_word(scan, str_cast("null"))) // boolean (false)
    {
        from->type = JSON_VALUE_NULL;
    }
    else if (scan_skip(scan, '{')) // object
    {
        from->type = JSON_OBJECT;
        json_parse_object(scan, alloc, from);
    }
    else if (scan_skip(scan, '[')) // array
    {
        from->type = JSON_ARRAY;
        json_parse_array(scan, alloc, from);
    }
    else
    {
        return false;
    }

    return true;
}

struct json_object json_parse(Scan *scan, Alloc *alloc)
{

    struct json_object root_object = {.type = JSON_OBJECT};
    scan_skip_space(scan);
    json_parse_value(scan, alloc, &root_object);

    return root_object;
}
