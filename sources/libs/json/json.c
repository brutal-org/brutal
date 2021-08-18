#include <ansi/ctype.h>
#include <json/json.h>

void json_parse_entries(Scan *scan, Alloc *alloc, struct json_data *from);

int is_closing_string(int c)
{
    if (c == '"')
    {
        return true;
    }
    return 0;
}

int json_is_value_definition_start(int s)
{
    if (s == '"' || isdigit(s) || s == '\'')
    {
        return 1;
    }
    return 0;
}

static Str json_parse_str(Scan *scan)
{
    scan_expect(scan, '"');
    Str name = scan_skip_until(scan, is_closing_string);
    scan_expect(scan, '"');

    return name;
}

static void json_destroy_recursive(struct json_data *data, Alloc *alloc)
{
    if (data->type == JSON_CLASS && data->json_class.childs.length != 0)
    {
        for (size_t i = 0; i < data->json_class.childs.length; i++)
        {
            json_destroy_recursive(&data->json_class.childs.data[i], alloc);
        }
        vec_deinit(&data->json_class.childs);
    }
    else if (data->type == JSON_ARRAY)
    {
        vec_deinit(&data->json_array.values);
    }
}

void json_destroy(struct json *entry)
{
    json_destroy_recursive(entry->data, entry->alloc);
}

bool json_parse_value(Scan *scan, Alloc *alloc, struct json_value *from)
{
    scan_skip_space(scan);

    if (scan_curr(scan) == '"') // string
    {
        from->type = JSON_VALUE_STRING;
        from->string = json_parse_str(scan);
    }
    else if (scan_curr(scan) == '\'') // char
    {
        from->type = JSON_VALUE_CHAR;
        scan_expect(scan, '\'');
        from->character = scan_curr(scan);
        scan_next(scan);
        scan_expect(scan, '\'');
    }
    else if (isalnum(scan_curr(scan)) || scan_curr(scan) == '-') // number
    {
        from->type = JSON_VALUE_NUMBER;
        from->number = scan_next_decimal(scan);
    }
    else
    {
        return false;
    }

    return true;
}

void json_parse_array(Scan *scan, Alloc *alloc, struct json_data *from)
{
    vec_init(&from->json_array.values, alloc);

    scan_skip_space(scan);

    while (scan_curr(scan) != ']' && !scan_end(scan))
    {
        struct json_value res;

        scan_skip_space(scan);

        if (!json_parse_value(scan, alloc, &res))
        {
            scan_throw(scan, str_cast("expected a value"), str_cast(""));
            return;
        }

        scan_skip_space(scan);

        vec_push(&from->json_array.values, res);

        if (scan_curr(scan) != ',')
        {
            return;
        }
        scan_skip_space(scan);

        scan_skip(scan, ',');
    }
}

void json_parse_entry(Scan *scan, Alloc *alloc, struct json_data *from)
{
    from->name = json_parse_str(scan);

    scan_skip_space(scan);
    scan_expect(scan, ':');
    scan_skip_space(scan);

    if (scan_skip(scan, '{'))
    {
        from->type = JSON_CLASS;
        json_parse_entries(scan, alloc, from);
        scan_expect(scan, '}');
    }
    else if (scan_skip(scan, '['))
    {

        from->type = JSON_ARRAY;
        json_parse_array(scan, alloc, from);
        scan_expect(scan, ']');
    }
    else
    {
        from->type = JSON_VALUE;
        if (json_parse_value(scan, alloc, &from->value) == false)
        {
            scan_throw(scan, str_cast("expected a value, an array or a class"), str_cast(""));
        }
    }
}

void json_parse_entries(Scan *scan, Alloc *alloc, struct json_data *from)
{
    vec_init(&from->json_class.childs, alloc);

    scan_skip_space(scan);

    while (scan_curr(scan) != '}' && !scan_end(scan))
    {
        struct json_data res;

        scan_skip_space(scan);

        json_parse_entry(scan, alloc, &res);

        scan_skip_space(scan);

        vec_push(&from->json_class.childs, res);

        if (scan_curr(scan) != ',')
        {
            return;
        }

        scan_skip(scan, ',');
    }
}

struct json json_parse(Scan *scan, Alloc *alloc)
{
    struct json ret = {.alloc = alloc, .data = NULL};

    ret.data = alloc_malloc(alloc, sizeof(struct json_data));
    ret.data->name = str_cast("root");
    ret.data->type = JSON_CLASS;

    scan_skip_space(scan);
    scan_expect(scan, '{');

    json_parse_entries(scan, alloc, ret.data);
    scan_skip_space(scan);
    scan_expect(scan, '}');

    return ret;
}

JsonDataResult json_get_child(struct json_data const *root, Str name)
{
    if (root->type != JSON_CLASS)
    {
        return ERR(JsonDataResult, JSON_NOT_VALID_TYPE);
    }

    for (size_t i = 0; i < root->json_class.childs.length; i++)
    {
        if (str_eq(root->json_class.childs.data[i].name, name))
        {
            return OK(JsonDataResult, root->json_class.childs.data[i]);
        }
    }

    return ERR(JsonDataResult, JSON_NOT_FOUNDED);
}

JsonValueResult json_value(struct json_data const *root)
{

    if (root->type != JSON_VALUE)
    {
        return ERR(JsonValueResult, JSON_NOT_VALID_TYPE);
    }
    return OK(JsonValueResult, root->value);
}

JsonValueResult json_array_entry(struct json_data const *root, int entry)
{

    if (root->type != JSON_ARRAY)
    {
        return ERR(JsonValueResult, JSON_NOT_VALID_TYPE);
    }

    if (root->json_array.values.length <= entry)
    {
        return ERR(JsonValueResult, JSON_ARRAY_OVERFLOW);
    }

    return OK(JsonValueResult, root->json_array.values.data[entry]);
}

JsonArraySizeResult json_array_size(struct json_data const *root)
{
    if (root->type != JSON_ARRAY)
    {
        return ERR(JsonArraySizeResult, JSON_NOT_VALID_TYPE);
    }

    return OK(JsonArraySizeResult, root->json_array.values.length);
}