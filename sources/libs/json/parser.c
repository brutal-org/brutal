#include <json/parser.h>
#include <brutal-alloc>
#include <brutal-io>

static bool json_skip_comment(Scan *scan)
{
    if (scan_curr(scan) == '/' && scan_peek(scan, 1) == '*')
    {
        while (!scan_ended(scan))
        {
            if (scan_curr(scan) == '*' && scan_peek(scan, 1) == '/')
            {
                scan_next_n(scan, 2);
                return true;
            }
            scan_next(scan);
        }

        scan_throw(scan, str$("not closed comment"), str$("/*"));
    }

    return false;
}

static void json_skip_space_and_comment(Scan *scan)
{
    while (scan_skip_space(scan) || json_skip_comment(scan))
        ;
}

static int is_closing_string(int c)
{
    return c != '"';
}

static Str parse_str(Scan *scan)
{
    json_skip_space_and_comment(scan);
    scan_expect(scan, '"');
    Str name = scan_eat_match(scan, is_closing_string);
    scan_expect(scan, '"');
    json_skip_space_and_comment(scan);

    return name;
}

Json json_parse_array(Scan *scan, Alloc *alloc)
{
    Json ret;
    ret.type = JSON_ARRAY;
    vec_init(&ret.array, alloc);

    while (scan_curr(scan) != ']' && !scan_ended(scan))
    {
        Json res;

        res = json_parse(scan, alloc);

        json_skip_space_and_comment(scan);

        vec_push(&ret.array, res);

        if (!scan_skip(scan, ','))
        {
            break;
        }

        json_skip_space_and_comment(scan);
    }

    scan_expect(scan, ']');

    return ret;
}

Json json_parse_object(Scan *scan, Alloc *alloc)
{
    Json ret;
    ret.type = JSON_OBJECT;
    map_init(&ret.object, alloc);

    while (scan_curr(scan) != '}' && !scan_ended(scan))
    {
        Str name = parse_str(scan);

        scan_expect(scan, ':');

        Json value = json_parse(scan, alloc);

        map_put(&ret.object, name, value);

        json_skip_space_and_comment(scan);

        if (!scan_skip(scan, ','))
        {
            break;
        }

        json_skip_space_and_comment(scan);
    }

    scan_expect(scan, '}');

    return ret;
}

Json json_parse(Scan *scan, Alloc *alloc)
{
    json_skip_space_and_comment(scan);

    if (scan_curr(scan) == '"') // string
    {
        return json_str(parse_str(scan));
    }
    else if (isdigit(scan_curr(scan)) || scan_curr(scan) == '-') // number
    {
        long value = 0;
        scan_next_int(scan, &value);
        return json_number(value);
    }
    else if (scan_skip_word(scan, str$("true"))) // boolean (true)
    {
        return json_true();
    }
    else if (scan_skip_word(scan, str$("false"))) // boolean (false)
    {
        return json_false();
    }
    else if (scan_skip_word(scan, str$("null"))) // null
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

Json json_parse_str(Str str, Alloc *alloc)
{
    Scan scan = {};
    scan_init(&scan, str);
    return json_parse(&scan, alloc);
}

Json json_parse_file(Str path, Alloc *alloc)
{
    IoFile file;
    io_file_view(&file, path);

    Buf buf;
    io_read_all(io_file_reader(&file), &buf, alloc);

    Scan scan = {};
    scan_init(&scan, buf_str(&buf));
    Json json = json_parse(&scan, alloc);

    io_file_close(&file);

    return json;
}
