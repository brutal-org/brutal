#include <json/json.h>
#include "test/test.h"

#define JSON_TEST_START()                              \
    HeapAlloc heap;                                    \
    heap_alloc_init(&heap);                            \
    Scan scanner = {};                                 \
    scan_init(&scanner, base);                         \
    JsonValue json = json_parse(&scanner, &heap.base); \
    assert_equal((int)json.type, (int)JSON_OBJECT)

#define JSON_TEST_END() \
    heap_alloc_deinit(&heap);

TEST(json_parse_test)
{
    Str base = str$("{ \"hello\": \"world\" }");

    JSON_TEST_START();

    JsonValue res = JSON_MEMBER_GET(json, JSON_STRING, str$("hello"));

    assert_str_equal(res.string, str$("world"));

    JSON_TEST_END();
}

TEST(json_parse_int)
{
    Str base = str$("{ \"hello\": 10 }");

    JSON_TEST_START();

    JsonValue res = JSON_MEMBER_GET(json, JSON_NUMBER, str$("hello"));

    assert_equal(res.number, 10);

    JSON_TEST_END();
}

TEST(json_parse_false)
{
    Str base = str$("{ \"hello\": false }");

    JSON_TEST_START();

    JSON_MEMBER_GET(json, JSON_FALSE, str$("hello"));

    JSON_TEST_END();
}

TEST(json_parse_true)
{
    Str base = str$("{ \"hello\": true }");

    JSON_TEST_START();

    JSON_MEMBER_GET(json, JSON_TRUE, str$("hello"));

    JSON_TEST_END();
}

TEST(json_parse_object_in_object)
{
    Str base = str$("{ \"hello\": { \"oh\": \"rust\", \"is\": \"bad\" } }");

    JSON_TEST_START();

    JsonValue res = JSON_MEMBER_GET(json, JSON_OBJECT, str$("hello"));

    JsonValue oh = JSON_MEMBER_GET(res, JSON_STRING, str$("oh"));

    assert_str_equal(oh.string, str$("rust"));

    JsonValue is = JSON_MEMBER_GET(res, JSON_STRING, str$("is"));

    assert_str_equal(is.string, str$("bad"));
    JSON_TEST_END();
}

TEST(json_parse_table)
{
    Str base = str$("{ \"hello\": [ \"oh\", \"rust\", \"is\", \"bad\" ] }");

    JSON_TEST_START();

    JsonValue res = JSON_MEMBER_GET(json, JSON_ARRAY, str$("hello"));

    assert_equal(res.array.length, 4);

    assert_str_equal(res.array.data[0].string, str$("oh"));
    assert_str_equal(res.array.data[1].string, str$("rust"));
    assert_str_equal(res.array.data[2].string, str$("is"));
    assert_str_equal(res.array.data[3].string, str$("bad"));

    JSON_TEST_END();
}
