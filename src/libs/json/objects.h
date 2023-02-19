#pragma once

#include <brutal-ds>

typedef enum
{
    JSON_NULL,
    JSON_TRUE,
    JSON_FALSE,
    JSON_ARRAY,
    JSON_OBJECT,
    JSON_STRING,
    JSON_NUMBER,
    JSON_ERROR
} JsonType;

typedef struct _Json Json;

typedef Map(Json) JsonObj;
typedef Vec(Json) JsonArr;

struct _Json
{
    JsonType type;

    union
    {
        JsonObj object;
        JsonArr array;
        Str string;
        long number;
    };
};

bool json_is(Json const json, JsonType type);

Json json_object(Alloc *alloc);

Json json_object_with_type(Str type, Alloc *alloc);

void json_put(Json *json, Str key, Json value);

Json json_get(Json const json, Str key);

bool json_try_get(Json const json, Str key, Json *result);

Json json_array(Alloc *alloc);

void json_append(Json *json, Json value);

Json json_at(Json json, int index);

int json_len(Json json);

Json json_null(void);

Json json_true(void);

Json json_false(void);

#define json_bool(val) ((val) ? json_true() : json_false())

Json json_error(void);

Json json_number(long number);

Json json_str(Str str);
