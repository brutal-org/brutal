#include <json/objects.h>
#include <brutal-debug>

bool json_is(Json const json, JsonType type)
{
    return json.type == type;
}

Json json_object(Alloc *alloc)
{
    Json json = {};
    json.type = JSON_OBJECT;
    map_init(&json.object, alloc);
    return json;
}

Json json_object_with_type(Str type, Alloc *alloc)
{
    Json object = json_object(alloc);
    json_put(&object, str$("$type"), json_str(type));
    return object;
}

void json_put(Json *json, Str key, Json value)
{
    assert_truth(json_is(*json, JSON_OBJECT));
    map_put(&json->object, key, value);
}

Json json_get(Json json, Str key)
{
    assert_truth(json_is(json, JSON_OBJECT));

    Json data;
    return map_get(&json.object, key, &data) ? data : json_null();
}

bool json_try_get(Json json, Str key, Json *result)
{
    assert_truth(json_is(json, JSON_OBJECT));
    return map_get(&json.object, key, result);
}

Json json_array(Alloc *alloc)
{
    Json json = {};
    json.type = JSON_ARRAY;
    vec_init(&json.array, alloc);

    return json;
}

void json_append(Json *json, Json value)
{
    assert_truth(json_is(*json, JSON_ARRAY));
    vec_push(&json->array, value);
}

Json json_at(Json const json, int index)
{
    assert_truth(json_is(json, JSON_ARRAY));

    return vec_at(&json.array, index);
}

int json_len(Json const json)
{
    assert_truth(json_is(json, JSON_ARRAY));
    return json.array.len;
}

Json json_null(void)
{
    return (Json){
        .type = JSON_NULL,
    };
}

Json json_true(void)
{
    return (Json){
        .type = JSON_TRUE,
    };
}

Json json_false(void)
{
    return (Json){
        .type = JSON_FALSE,
    };
}

Json json_error(void)
{
    return (Json){
        .type = JSON_ERROR,
    };
}

Json json_number(long number)
{
    return (Json){
        .type = JSON_NUMBER,
        .number = number,
    };
}

Json json_str(Str str)
{
    return (Json){
        .type = JSON_STRING,
        .string = str,
    };
}
