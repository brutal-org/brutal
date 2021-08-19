#include <ansi/ctype.h>
#include <json/json.h>

static int is_closing_string(int c)
{
    return c != '"';
}

static Str json_parse_str(Scan *scan)
{
    scan_skip_space(scan);
    scan_expect(scan, '"');

    Str name = scan_skip_until(scan, is_closing_string);

    scan_expect(scan, '"');
    scan_skip_space(scan);

    return name;
}

static JsonValue json_parse_array(Scan *scan, Alloc *alloc)
{
    JsonValue ret;
    ret.type = JSON_ARRAY;
    vec_init(&ret.array, alloc);

    while (scan_curr(scan) != ']' && !scan_end(scan))
    {
        JsonValue res;

        res = json_parse(scan, alloc);

        scan_skip_space(scan);

        vec_push(&ret.array, res);

        if (!scan_skip(scan, ','))
        {
            return ret;
        }

        scan_skip_space(scan);
    }

    return ret;
}

static JsonValue json_parse_object(Scan *scan, Alloc *alloc)
{
    JsonValue ret;
    ret.type = JSON_OBJECT;
    map_init(&ret.object, alloc);

    while (scan_curr(scan) != '}' && !scan_end(scan))
    {
        Str name = json_parse_str(scan);

        scan_expect(scan, ':');

        JsonValue value = json_parse(scan, alloc);

        map_put(&ret.object, name, value);

        scan_skip_space(scan);

        if (!scan_skip(scan, ','))
        {
            return ret;
        }

        scan_skip_space(scan);
    }
    return ret;
}

JsonValue json_parse(Scan *scan, Alloc *alloc)
{
    scan_skip_space(scan);

    if (scan_curr(scan) == '"') // string
    {
        return json_string(json_parse_str(scan));
    }
    else if (isdigit(scan_curr(scan)) || scan_curr(scan) == '-') // number
    {
        return json_number(scan_next_decimal(scan));
    }
    else if (scan_skip_word(scan, str$("true"))) // boolean (true)
    {
        return json_true();
    }
    else if (scan_skip_word(scan, str$("false"))) // boolean (false)
    {
        return json_false();
    }
    else if (scan_skip_word(scan, str$("null"))) // boolean (false)
    {
        return json_null();
    }
    else if (scan_skip(scan, '{')) // object
    {
        return json_parse_object(scan, alloc);
    }
    else if (scan_skip(scan, '[')) // array
    {
        return json_parse_array(scan, alloc);
    }
    else
    {
        scan_throw(scan, str$("expected a value"), str$(""));

        return json_error();
    }
}
