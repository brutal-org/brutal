#include <idl/builtin.h>

static IdlBuiltinType builtins[] = {
    {
        .name = str_const$("Enum"),
        .cname = str_const$("enum"),
        .pack_fn = str_const$("bal_pack_enum"),
        .unpack_fn = str_const$("bal_unpack_enum"),
    },

    {
        .name = str_const$("Size"),
        .cname = str_const$("size_t"),
        .pack_fn = str_const$("bal_pack_size"),
        .unpack_fn = str_const$("bal_unpack_size"),
    },

    {
        .name = str_const$("S8"),
        .cname = str_const$("int8_t"),
        .pack_fn = str_const$("bal_pack_s8"),
        .unpack_fn = str_const$("bal_unpack_s8"),
    },
    {
        .name = str_const$("U8"),
        .cname = str_const$("uint8_t"),
        .pack_fn = str_const$("bal_pack_u8"),
        .unpack_fn = str_const$("bal_unpack_u8"),
    },
    {
        .name = str_const$("S16"),
        .cname = str_const$("int16_t"),
        .pack_fn = str_const$("bal_pack_s16"),
        .unpack_fn = str_const$("bal_unpack_s16"),
    },
    {
        .name = str_const$("U16"),
        .cname = str_const$("uint16_t"),
        .pack_fn = str_const$("bal_pack_u16"),
        .unpack_fn = str_const$("bal_unpack_u16"),
    },
    {
        .name = str_const$("S32"),
        .cname = str_const$("int32_t"),
        .pack_fn = str_const$("bal_pack_s32"),
        .unpack_fn = str_const$("bal_unpack_s32"),
    },
    {
        .name = str_const$("U32"),
        .cname = str_const$("uint32_t"),
        .pack_fn = str_const$("bal_pack_u32"),
        .unpack_fn = str_const$("bal_unpack_u32"),
    },
    {
        .name = str_const$("S64"),
        .cname = str_const$("int64_t"),
        .pack_fn = str_const$("bal_pack_s64"),
        .unpack_fn = str_const$("bal_unpack_s64"),
    },
    {
        .name = str_const$("U64"),
        .cname = str_const$("uint64_t"),
        .pack_fn = str_const$("bal_pack_u64"),
        .unpack_fn = str_const$("bal_unpack_u64"),
    },

    {
        .name = str_const$("F32"),
        .cname = str_const$("float"),
        .pack_fn = str_const$("bal_pack_f32"),
        .unpack_fn = str_const$("bal_unpack_f32"),
    },
    {
        .name = str_const$("F64"),
        .cname = str_const$("double"),
        .pack_fn = str_const$("bal_pack_f64"),
        .unpack_fn = str_const$("bal_unpack_f64"),
    },

    {
        .name = str_const$("Str"),
        .cname = str_const$("Str"),
        .pack_fn = str_const$("bal_pack_str"),
        .unpack_fn = str_const$("bal_unpack_str"),
    },

    {
        .name = str_const$("Capability"),
        .cname = str_const$("BrAddr"),
        .pack_fn = str_const$("bal_pack_addr"),
        .unpack_fn = str_const$("bal_unpack_addr"),
    },
};

IdlBuiltinType *idl_lookup_builtin(Str name)
{
    for (size_t i = 0; i < array_len$(builtins); i++)
    {
        if (str_eq(name, builtins[i].name))
        {
            return &builtins[i];
        }
    }

    return nullptr;
}
