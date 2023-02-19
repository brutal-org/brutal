#include <brutal-test>
#include <json/objects.h>
#include <json/parser.h>

test$(json_parse_test)
{
    Str base = str$("{ \"hello\": \"world\" }");
    Json json = json_parse_str(base, test_use_alloc());

    expect$(json_is(json, JSON_OBJECT));

    Json res = json_get(json, str$("hello"));

    expect_str_equal$(res.string, str$("world"));
}

test$(json_parse_int)
{
    Str base = str$("{ \"hello\": 10 }");
    Json json = json_parse_str(base, test_use_alloc());

    expect$(json_is(json, JSON_OBJECT));

    Json res = json_get(json, str$("hello"));

    expect_equal$(res.number, 10);
}

test$(json_parse_false)
{
    Str base = str$("{ \"hello\": false }");
    Json json = json_parse_str(base, test_use_alloc());

    expect$(json_is(json, JSON_OBJECT));

    json_get(json, str$("hello"));
}

test$(json_parse_true)
{
    Str base = str$("{ \"hello\": true }");
    Json json = json_parse_str(base, test_use_alloc());

    expect$(json_is(json, JSON_OBJECT));

    json_get(json, str$("hello"));
}

test$(json_parse_object_in_object)
{
    Str base = str$("{ \"hello\": { \"oh\": \"rust\", \"is\": \"based\" } }");
    Json json = json_parse_str(base, test_use_alloc());

    expect$(json_is(json, JSON_OBJECT));

    Json res = json_get(json, str$("hello"));
    Json oh = json_get(res, str$("oh"));
    Json is = json_get(res, str$("is"));

    expect_str_equal$(oh.string, str$("rust"));
    expect_str_equal$(is.string, str$("based"));
}

test$(json_parse_table)
{
    Str base = str$("{ \"hello\": [ \"oh\", \"rust\", { \"is\" : \"not\" }, \"cool\" ] }");
    Json json = json_parse_str(base, test_use_alloc());

    expect$(json_is(json, JSON_OBJECT));

    Json res = json_get(json, str$("hello"));

    expect_equal$(res.array.len, 4);

    expect_str_equal$(res.array.data[0].string, str$("oh"));
    expect_str_equal$(res.array.data[1].string, str$("rust"));
    expect$(json_is(res.array.data[2], JSON_OBJECT));
    expect_str_equal$(res.array.data[3].string, str$("cool"));
}

test$(json_parse_comment_test)
{
    Str base = str$("{ /* this is a beautiful comment */ \"hello\": /* am i here */ \"world\" } /*or here ???*/");
    Json json = json_parse_str(base, test_use_alloc());

    expect$(json_is(json, JSON_OBJECT));

    Json res = json_get(json, str$("hello"));

    expect_str_equal$(res.string, str$("world"));
}
