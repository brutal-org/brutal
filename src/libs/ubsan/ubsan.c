#include <brutal-debug>
#include <brutal-text>

struct _UbsanSrc
{
    char const *file;
    uint32_t line;
    uint32_t column;
};

struct _UbsanType
{
    uint16_t kind;
    uint16_t info;
    char name[];
};

static void ubsan_print_location(char *message, struct _UbsanSrc loc)
{
    panic$("{}:{}:{}: {case-sentence}", str$(loc.file), loc.line, loc.column, message);
}

struct _UbsanOverflowData
{
    struct _UbsanSrc location;
    struct _UbsanType *type;
};

void __ubsan_handle_add_overflow(struct _UbsanOverflowData *data)
{
    ubsan_print_location("addition overflow", data->location);
}

void __ubsan_handle_sub_overflow(struct _UbsanOverflowData *data)
{
    ubsan_print_location("subtraction overflow", data->location);
}

void __ubsan_handle_mul_overflow(struct _UbsanOverflowData *data)
{
    ubsan_print_location("multiplication overflow", data->location);
}

void __ubsan_handle_divrem_overflow(struct _UbsanOverflowData *data)
{
    ubsan_print_location("division overflow", data->location);
}

void __ubsan_handle_negate_overflow(struct _UbsanOverflowData *data)
{
    ubsan_print_location("negation overflow", data->location);
}

void __ubsan_handle_pointer_overflow(struct _UbsanOverflowData *data)
{
    ubsan_print_location("pointer overflow", data->location);
}

struct _UbsanShiftOobData
{
    struct _UbsanSrc location;
    struct _UbsanType *left_type;
    struct _UbsanType *right_type;
};

void __ubsan_handle_shift_out_of_bounds(struct _UbsanShiftOobData *data)
{
    ubsan_print_location("shift out of bounds", data->location);
}

struct _UbsanInvalidData
{
    struct _UbsanSrc location;
    struct _UbsanType *type;
};

void __ubsan_handle_load_invalid_value(struct _UbsanInvalidData *data)
{
    ubsan_print_location("invalid load value", data->location);
}

struct _UbsanArrayOobData
{
    struct _UbsanSrc location;
    struct _UbsanType *array_type;
    struct _UbsanType *index_type;
};

void __ubsan_handle_out_of_bounds(struct _UbsanArrayOobData *data)
{
    log$(" for: {} at index: {}", (const char *)data->array_type->name, data->index_type->info);
    ubsan_print_location("array out of bounds", data->location);
}

struct _UbsanTypeMismatchV1Data
{
    struct _UbsanSrc location;
    struct _UbsanType *type;
    unsigned char log_alignment;
    unsigned char type_check_kind;
};

void __ubsan_handle_type_mismatch_v1(struct _UbsanTypeMismatchV1Data *data, uintptr_t ptr)
{
    if (!ptr)
    {
        ubsan_print_location("use of NULL pointer", data->location);
    }

    else if (ptr & ((1 << data->log_alignment) - 1))
    {
        ubsan_print_location("use of misaligned pointer", data->location);
    }
    else
    {
        ubsan_print_location("no space for object", data->location);
    }
}

struct _UbsanNegativeVlaData
{
    struct _UbsanSrc location;
    struct _UbsanType *type;
};

void __ubsan_handle_vla_bound_not_positive(struct _UbsanNegativeVlaData *data)
{
    ubsan_print_location("variable-length argument is negative", data->location);
}

struct _UbsanNonnullReturnData
{
    struct _UbsanSrc location;
};

void __ubsan_handle_nonnull_return(struct _UbsanNonnullReturnData *data)
{
    ubsan_print_location("non-null return is null", data->location);
}

struct ubsan_nonnull_arg_data
{
    struct _UbsanSrc location;
};

void __ubsan_handle_nonnull_arg(struct ubsan_nonnull_arg_data *data)
{
    ubsan_print_location("non-null argument is null", data->location);
}

struct _UbsanUnreachableData
{
    struct _UbsanSrc location;
};

void __ubsan_handle_builtin_unreachable(struct _UbsanUnreachableData *data)
{

    ubsan_print_location("unreachable code reached", data->location);
}

struct _UbsanInvalidBuiltinData
{
    struct _UbsanSrc location;
    unsigned char kind;
};

void __ubsan_handle_invalid_builtin(struct _UbsanInvalidBuiltinData *data)
{

    ubsan_print_location("invalid builtin", data->location);
}

struct _UbsanFloatCastOverflowData
{
    struct _UbsanSrc location;
};

void __ubsan_handle_float_cast_overflow(struct _UbsanFloatCastOverflowData *data)
{
    ubsan_print_location("float cast overflow", data->location);
}
