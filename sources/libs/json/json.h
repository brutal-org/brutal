#pragma once
#include <brutal/base.h>
#include <brutal/ds.h>
#include <brutal/io.h>

enum json_type
{
    JSON_VALUE,
    JSON_ARRAY,
    JSON_CLASS,
};

enum json_error
{
    JSON_NOT_FOUNDED,
    JSON_NOT_VALID_TYPE,
    JSON_ARRAY_OVERFLOW,
};

enum json_value_type
{
    JSON_VALUE_STRING,
    JSON_VALUE_NUMBER,
    JSON_VALUE_CHAR,
};

struct json_value
{
    enum json_value_type type;
    union
    {
        Str string;
        long number;
        char character;
    };
};

struct json_data
{
    Str name;
    enum json_type type;

    union
    {
        struct
        {
            Vec(struct json_data) childs;
        } json_class;

        struct
        {
            Vec(struct json_value) values;
        } json_array;

        struct json_value value;
    };
};

struct json
{
    struct json_data *data;
    Alloc *alloc;
};

typedef Result(enum json_error, struct json_data) JsonDataResult;
typedef Result(enum json_error, struct json_value) JsonValueResult;
typedef Result(enum json_error, size_t) JsonArraySizeResult;

void json_destroy(struct json *entry);

struct json json_parse(Scan *scan, Alloc *alloc);

JsonDataResult json_get_child(struct json_data const *root, Str name);

JsonValueResult json_value(struct json_data const *root);

JsonValueResult json_array_entry(struct json_data const *root, int entry);

JsonArraySizeResult json_array_size(struct json_data const *root);