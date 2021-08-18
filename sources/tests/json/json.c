#include <brutal/alloc.h>
#include <brutal/alloc/heap.h>
#include <brutal/gfx.h>
#include <brutal/io.h>
#include <json/json.h>
#include "brutal/log/assert.h"
#include "tests/test.h"

#define JSON_TEST_START()                              \
    struct alloc_heap heap;                            \
    alloc_heap_init(&heap);                            \
    Scan scanner = {};                                 \
    scan_init(&scanner, base);                         \
    JsonValue json = json_parse(&scanner, &heap.base); \
    assert_equal((int)json.type, (int)JSON_OBJECT)

#define JSON_TEST_END() \
    alloc_heap_deinit(&heap);

#define JSON_MEMBER_GET(_object, _type, _name)                      \
    (                                                               \
        {                                                           \
            JsonValue _res = {};                                    \
            assert_truth(map_get(&_object.object, _name, &(_res))); \
            assert_equal((int)_res.type, (int)_type);               \
            _res;                                                   \
        })

TEST(json_parse_test)
{
    Str base = str_cast("{ \"hello\": \"world\" }");

    JSON_TEST_START();

    JsonValue res = JSON_MEMBER_GET(json, JSON_STRING, str_cast("hello"));

    assert_str_equal(res.string, str_cast("world"));

    JSON_TEST_END();
}

TEST(json_parse_int)
{
    Str base = str_cast("{ \"hello\": 10 }");

    JSON_TEST_START();

    JsonValue res = JSON_MEMBER_GET(json, JSON_NUMBER, str_cast("hello"));

    assert_equal(res.number, 10);

    JSON_TEST_END();
}

TEST(json_parse_false)
{
    Str base = str_cast("{ \"hello\": false }");

    JSON_TEST_START();

    JSON_MEMBER_GET(json, JSON_FALSE, str_cast("hello"));

    JSON_TEST_END();
}

TEST(json_parse_true)
{
    Str base = str_cast("{ \"hello\": true }");

    JSON_TEST_START();

    JSON_MEMBER_GET(json, JSON_TRUE, str_cast("hello"));

    JSON_TEST_END();
}

TEST(json_parse_object_in_object)
{
    Str base = str_cast("{ \"hello\": { \"oh\": \"rust\", \"is\": \"bad\" } }");

    JSON_TEST_START();

    JsonValue res = JSON_MEMBER_GET(json, JSON_OBJECT, str_cast("hello"));

    JsonValue oh = JSON_MEMBER_GET(res, JSON_STRING, str_cast("oh"));

    assert_str_equal(oh.string, str_cast("rust"));

    JsonValue is = JSON_MEMBER_GET(res, JSON_STRING, str_cast("is"));

    assert_str_equal(is.string, str_cast("bad"));
    JSON_TEST_END();
}

TEST(json_parse_table)
{
    Str base = str_cast("{ \"hello\": [ \"oh\", \"rust\", \"is\", \"bad\" ] }");

    JSON_TEST_START();

    JsonValue res = JSON_MEMBER_GET(json, JSON_ARRAY, str_cast("hello"));

    assert_equal(res.array.length, 4);

    assert_str_equal(res.array.data[0].string, str_cast("oh"));
    assert_str_equal(res.array.data[1].string, str_cast("rust"));
    assert_str_equal(res.array.data[2].string, str_cast("is"));
    assert_str_equal(res.array.data[3].string, str_cast("bad"));

    JSON_TEST_END();
}
