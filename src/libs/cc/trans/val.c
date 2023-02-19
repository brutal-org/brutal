#include <brutal-debug>
#include <cc/trans.h>

void cc_trans_value(Emit *emit, CVal value)
{
    switch (value.type)
    {
    case CVAL_SIGNED:
        emit_fmt$(emit, "{}", value.signed_);
        break;

    case CVAL_UNSIGNED:
        emit_fmt$(emit, "{}", value.unsigned_);
        break;

    case CVAL_FLOAT:
        emit_fmt$(emit, "{}", value.float_);
        break;

    case CVAL_STRING:
        emit_fmt$(emit, "\"{}\"", value.string_);
        break;

    default:
        panic$("Unknown CVal type {}", value.type);
        break;
    }
}
