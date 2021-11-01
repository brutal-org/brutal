#include <brutal/debug.h>
#include <json/emit.h>

void json_emit_string(Str str, Emit *emit)
{
    emit_fmt(emit, "\"");

    for (size_t i = 0; i < str.len; i++)
    {
        if (str.buffer[i] == '"')
        {
            emit_fmt(emit, "\\");
        }

        emit_fmt(emit, "{c}", str.buffer[i]);
    }

    emit_fmt(emit, "\"");
}

void json_emit(Json const json, Emit *emit)
{
    bool first = true;

    switch (json.type)
    {
    case JSON_NULL:
        emit_fmt(emit, "null");
        break;

    case JSON_TRUE:
        emit_fmt(emit, "true");
        break;

    case JSON_FALSE:
        emit_fmt(emit, "false");
        break;

    case JSON_ARRAY:
        emit_fmt(emit, "[");

        vec_foreach(el, &json.array)
        {
            if (!first)
            {
                emit_fmt(emit, ", ");
            }

            json_emit(el, emit);

            first = false;
        }

        emit_fmt(emit, "]");
        break;

    case JSON_OBJECT:
        emit_fmt(emit, "{{");

        bool first = true;

        map_foreach(k, v, &json.object)
        {
            if (!first)
            {
                emit_fmt(emit, ", ");
            }

            json_emit_string(k, emit);
            emit_fmt(emit, ": ");
            json_emit(v, emit);

            first = false;
        }

        emit_fmt(emit, "}}");
        break;

    case JSON_STRING:
        json_emit_string(json.string, emit);
        break;

    case JSON_NUMBER:
        emit_fmt(emit, "{}", json.number);
        break;

    default:
        panic$("Unknow json type {}", json.type);
    }
}
