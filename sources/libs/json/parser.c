#include <brutal/alloc.h>
#include <brutal/io.h>
#include <json/parser.h>

static int is_closing_string(int c)
{
    return c != '"';
}

static Str parse_str(Scan *scan)
{
    scan_skip_space(scan);
    scan_expect(scan, '"');

    Str name = scan_skip_until(scan, is_closing_string);

    scan_expect(scan, '"');
    scan_skip_space(scan);

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

        scan_skip_space(scan);

        if (!scan_skip(scan, ','))
        {
            return ret;
        }

        scan_skip_space(scan);
    }
    return ret;
}

Json json_parse(Scan *scan, Alloc *alloc)
{
    scan_skip_space(scan);

    if (scan_curr(scan) == '"') // string
    {
        return json_string(parse_str(scan), alloc);
    }
    else if (isdigit(scan_curr(scan)) || scan_curr(scan) == '-') // number
    {
        return json_number(scan_next_number(scan));
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

Json json_parse_str(Str str, Alloc *alloc)
{
    Scan scan = {};
    scan_init(&scan, str);
    return json_parse(&scan, alloc);
}

Json json_parse_file(Str path, Alloc *alloc)
{
    HeapAlloc heap;
    heap_alloc_init(&heap, NODE_DEFAULT);

    IoFile file;
    io_file_open(&file, path);

    IoReader reader = io_file_reader(&file);
    Buffer buffer = io_readall((&reader), alloc_global());

    Scan scan = {};
    scan_init(&scan, buffer_str(&buffer));
    Json json = json_parse(&scan, alloc);

    io_file_close(&file);

    heap_alloc_deinit(&heap);

    return json;
}
