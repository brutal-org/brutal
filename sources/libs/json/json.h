#pragma once
#include <brutal/alloc/heap.h>
#include <brutal/base.h>
#include <brutal/ds.h>
#include <brutal/io.h>

enum json_type
{
    JSON_ARRAY,
    JSON_OBJECT,
    JSON_STRING,
    JSON_NUMBER,
    JSON_BOOL,
    JSON_NULL,
    JSON_ERROR
};

struct json_value;

typedef Map(struct json_value) json_object;
typedef Vec(struct json_value) json_array;

typedef struct json_value
{
    enum json_type type;

    union
    {
        json_object object;

        json_array array;

        Str string;

        long number;

        char character;

        bool boolean;
    };
} JsonValue;

JsonValue json_parse(Scan *scan, Alloc *alloc);
