#include <brutal/alloc.h>
#include <brutal/debug.h>
#include <brutal/io.h>
#include <brutal/text.h>
#include <json/json.h>
#include "loader/config.h"

LoaderEntry config_get_entry(Str name, Str path, Buffer *buffer)
{
    IoFile file;
    IoReader reader;
    LoaderEntry ret;

    io_file_open(&file, path);

    reader = io_file_reader(&file);

    *buffer = io_readall((&reader), alloc_global());

    Str base = str$((char *)buffer->data);

    Scan scanner = {};

    scan_init(&scanner, base);

    JsonValue json = json_parse(&scanner, alloc_global());

    JsonValue res = JSON_MEMBER_GET(json, JSON_ARRAY, str$("entries"));

    int i = 0;
    JsonValue entry_name = JSON_MEMBER_GET(res.array.data[i], JSON_STRING, str$("name"));

    while (str_eq(entry_name.string, name) != true)
    {
        entry_name = JSON_MEMBER_GET(res.array.data[i], JSON_STRING, str$("name"));
        i++;
    }

    ret.kernel = JSON_MEMBER_GET(res.array.data[i], JSON_STRING, str$("kernel")).string;

    return ret;
}
