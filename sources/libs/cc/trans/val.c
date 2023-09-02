#include <brutal-debug>
#include <cc/trans.h>

void ctrans_value(Emit *emit, CVal value)
{
    switch (value.type)
    {
    case CVAL_INT:
        emit_fmt$(emit, "{}", value.signed_);
        break;

    case CVAL_UINT:
        emit_fmt$(emit, "{}", value.unsigned_);
        break;

    case CVAL_FLOAT:
        emit_fmt$(emit, "{}", value.float_);
        break;

    case CVAL_STRING:
        emit_fmt$(emit, "\"{}\"", value.str_);
        break;

    default:
        panic$("Unknown CVal type {}", value.type);
        break;
    }
}
