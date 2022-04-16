#include <brutal-debug>
#include <json/emit.h>

void json_emit_string(Str str, Emit *emit)
{
    emit_fmt$(emit, "\"");

    for (size_t i = 0; i < str.len; i++)
    {
        if (str.buf[i] == '"')
        {
            emit_fmt$(emit, "\\");
        }

        emit_fmt$(emit, "{c}", str.buf[i]);
    }

    emit_fmt$(emit, "\"");
}

void json_emit(Json const json, Emit *emit)
{
    bool first = true;
    bool any = false;

    switch (json.type)
    {
    case JSON_NULL:
        emit_fmt$(emit, "null");
        break;

    case JSON_TRUE:
        emit_fmt$(emit, "true");
        break;

    case JSON_FALSE:
        emit_fmt$(emit, "false");
        break;

    case JSON_ARRAY:
        emit_fmt$(emit, "[");
        emit_ident(emit);
        vec_foreach_v(el, &json.array)
        {
            if (!first)
            {
                emit_fmt$(emit, ",");
            }
            emit_fmt$(emit, "\n");

            json_emit(el, emit);

            first = false;
            any = true;
        }

        if (any)
        {
            emit_fmt$(emit, "\n");
        }

        emit_deident(emit);
        emit_fmt$(emit, "]");
        break;

    case JSON_OBJECT:
        emit_fmt$(emit, "{{");
        emit_ident(emit);

        map_foreach(k, v, &json.object)
        {
            if (!first)
            {
                emit_fmt$(emit, ",");
            }
            emit_fmt$(emit, "\n");

            json_emit_string(k, emit);
            emit_fmt$(emit, ": ");
            json_emit(v, emit);

            first = false;
            any = true;
        }

        if (any)
        {
            emit_fmt$(emit, "\n");
        }

        emit_deident(emit);
        emit_fmt$(emit, "}}");
        break;

    case JSON_STRING:
        json_emit_string(json.string, emit);
        break;

    case JSON_NUMBER:
        emit_fmt$(emit, "{}", json.number);
        break;

    default:
        panic$("Unknow json type {}", json.type);
    }
}
