#pragma once
#include <brutal/alloc/heap.h>
#include <brutal/base.h>
#include <brutal/ds.h>
#include <brutal/io.h>

enum json_type
{
    JSON_ARRAY,
    JSON_OBJECT,
    JSON_VALUE_STRING,
    JSON_VALUE_NUMBER,
    JSON_VALUE_CHAR,
    JSON_VALUE_BOOL,
    JSON_VALUE_NULL,
};

struct json_object;

typedef Map(struct json_object) json_members;

struct json_object
{
    enum json_type type;

    union
    {
        struct
        {
            json_members members;
        } object;

        struct
        {
            Vec(struct json_object) elements;
        } array;

        Str string;

        long number;

        char character;

        bool boolean;
    };
};

struct json_object json_parse(Scan *scan, Alloc *alloc);
