#include <json/objects.h>
#include <json/parser.h>
#include "test/test.h"

TEST(json_parse_test)
{
    Str base = str$("{ \"hello\": \"world\" }");
    Json json = json_parse_str(base, test_alloc());

    assert_truth(json_is(json, JSON_OBJECT));

    Json res = json_get(json, str$("hello"));

    assert_str_equal(res.string, str$("world"));
}

TEST(json_parse_int)
{
    Str base = str$("{ \"hello\": 10 }");
    Json json = json_parse_str(base, test_alloc());

    assert_truth(json_is(json, JSON_OBJECT));

    Json res = json_get(json, str$("hello"));

    assert_equal(res.number, 10);
}

TEST(json_parse_false)
{
    Str base = str$("{ \"hello\": false }");
    Json json = json_parse_str(base, test_alloc());

    assert_truth(json_is(json, JSON_OBJECT));

    json_get(json, str$("hello"));
}

TEST(json_parse_true)
{
    Str base = str$("{ \"hello\": true }");
    Json json = json_parse_str(base, test_alloc());

    assert_truth(json_is(json, JSON_OBJECT));

    json_get(json, str$("hello"));
}

TEST(json_parse_object_in_object)
{
    Str base = str$("{ \"hello\": { \"oh\": \"rust\", \"is\": \"based\" } }");
    Json json = json_parse_str(base, test_alloc());

    assert_truth(json_is(json, JSON_OBJECT));

    Json res = json_get(json, str$("hello"));
    Json oh = json_get(res, str$("oh"));
    Json is = json_get(res, str$("is"));

    assert_str_equal(oh.string, str$("rust"));
    assert_str_equal(is.string, str$("based"));
}

TEST(json_parse_table)
{
    Str base = str$("{ \"hello\": [ \"oh\", \"rust\", { \"is\" : \"not\" }, \"cool\" ] }");
    Json json = json_parse_str(base, test_alloc());

    assert_truth(json_is(json, JSON_OBJECT));

    Json res = json_get(json, str$("hello"));

    assert_equal(res.array.len, 4);

    assert_str_equal(res.array.data[0].string, str$("oh"));
    assert_str_equal(res.array.data[1].string, str$("rust"));
    assert_truth(json_is(res.array.data[2], JSON_OBJECT));
    assert_str_equal(res.array.data[3].string, str$("cool"));
}
