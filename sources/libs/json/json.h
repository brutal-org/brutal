#pragma once

#include <brutal/alloc.h>
#include <brutal/ds.h>
#include <brutal/io.h>

enum json_type
{
    JSON_NULL,
    JSON_TRUE,
    JSON_FALSE,
    JSON_ARRAY,
    JSON_OBJECT,
    JSON_STRING,
    JSON_NUMBER,
    JSON_ERROR
};

typedef struct json_value JsonValue;

typedef Map(JsonValue) json_object;
typedef Vec(JsonValue) json_array;

struct json_value
{
    enum json_type type;

    union
    {
        json_object object;
        json_array array;
        Str string;
        long number;
        char character;
    };
};

JsonValue json_parse(Scan *scan, Alloc *alloc);

static inline JsonValue json_null(void)
{
    return (JsonValue){
        .type = JSON_NULL,
    };
}

static inline JsonValue json_true(void)
{
    return (JsonValue){
        .type = JSON_TRUE,
    };
}

static inline JsonValue json_false(void)
{
    return (JsonValue){
        .type = JSON_FALSE,
    };
}

static inline JsonValue json_error(void)
{
    return (JsonValue){
        .type = JSON_ERROR,
    };
}

static inline JsonValue json_number(long number)
{
    return (JsonValue){
        .type = JSON_NUMBER,
        .number = number,
    };
}

static inline JsonValue json_string(Str str)
{
    return (JsonValue){
        .type = JSON_STRING,
        .string = str,
    };
}
