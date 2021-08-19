#include <brutal/alloc.h>
#include <brutal/io/buffer.h>
#include <brutal/log.h>
#include "brutal/alloc/global.h"
#include "brutal/io/write.h"
#include "brutal/text/str.h"
#include "config.h"

#define JSON_MEMBER_GET(_object, _type, _name)                      \
    (                                                               \
        {                                                           \
            JsonValue _res = {};                                    \
            assert_truth(map_get(&_object.object, _name, &(_res))); \
            assert_equal((int)_res.type, (int)_type);               \
            _res;                                                   \
        })

LoaderEntry config_get_entry(Str name, Str path, Buffer *buffer)
{
    IoFile file;
    IoFileReader reader;
    LoaderEntry ret;

    io_file_open(&file, path);

    reader = io_file_read(&file);

    *buffer = io_readall(base_cast(&reader), alloc_global());

    Str base = str_cast((char *)buffer->data);

    Scan scanner = {};

    scan_init(&scanner, base);

    JsonValue json = json_parse(&scanner, alloc_global());

    JsonValue res = JSON_MEMBER_GET(json, JSON_ARRAY, str_cast("entries"));

    int i = 0;
    JsonValue entry_name = JSON_MEMBER_GET(res.array.data[i], JSON_STRING, str_cast("name"));

    while (str_eq(entry_name.string, name) != true)
    {
        entry_name = JSON_MEMBER_GET(res.array.data[i], JSON_STRING, str_cast("name"));
        i++;
    }

    ret.kernel = JSON_MEMBER_GET(res.array.data[i], JSON_STRING, str_cast("kernel")).string;

    return ret;
}
