#include <bid/builtin.h>

static BidBuiltinType builtins[] = {
    {
        .name = str_const$("Enum"),
        .pack_fn = str_const$("bal_pack_enum"),
        .unpack_fn = str_const$("bal_unpack_enum"),
    },

    {
        .name = str_const$("Size"),
        .pack_fn = str_const$("bal_pack_size"),
        .unpack_fn = str_const$("bal_unpack_size"),
    },

    {
        .name = str_const$("S8"),
        .pack_fn = str_const$("bal_pack_s8"),
        .unpack_fn = str_const$("bal_unpack_s8"),
    },
    {
        .name = str_const$("U8"),
        .pack_fn = str_const$("bal_pack_u8"),
        .unpack_fn = str_const$("bal_unpack_u8"),
    },
    {
        .name = str_const$("S16"),
        .pack_fn = str_const$("bal_pack_s16"),
        .unpack_fn = str_const$("bal_unpack_s16"),
    },
    {
        .name = str_const$("U16"),
        .pack_fn = str_const$("bal_pack_s16"),
        .unpack_fn = str_const$("bal_unpack_s16"),
    },
    {
        .name = str_const$("S32"),
        .pack_fn = str_const$("bal_pack_s32"),
        .unpack_fn = str_const$("bal_unpack_s32"),
    },
    {
        .name = str_const$("U32"),
        .pack_fn = str_const$("bal_pack_u32"),
        .unpack_fn = str_const$("bal_unpack_u32"),
    },
    {
        .name = str_const$("S64"),
        .pack_fn = str_const$("bal_pack_s64"),
        .unpack_fn = str_const$("bal_unpack_s64"),
    },
    {
        .name = str_const$("U64"),
        .pack_fn = str_const$("bal_pack_u64"),
        .unpack_fn = str_const$("bal_unpack_u64"),
    },

    {
        .name = str_const$("F32"),
        .pack_fn = str_const$("bal_pack_f32"),
        .unpack_fn = str_const$("bal_unpack_f32"),
    },
    {
        .name = str_const$("F64"),
        .pack_fn = str_const$("bal_pack_f64"),
        .unpack_fn = str_const$("bal_unpack_f64"),
    },

    {
        .name = str_const$("Str"),
        .pack_fn = str_const$("bal_pack_str"),
        .unpack_fn = str_const$("bal_unpack_str"),
    },
};

BidBuiltinType *bid_lookup_builtin(Str name)
{
    for (size_t i = 0; i < ARRAY_LEN(builtins); i++)
    {
        if (str_eq(name, builtins[i].name))
        {
            return &builtins[i];
        }
    }

    return nullptr;
}
