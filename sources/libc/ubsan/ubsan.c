#include <brutal/log.h>
#include <brutal/text.h>
#include <brutal/base/std.h>

struct ubsan_source_location
{
    const char *file;
    uint32_t line;
    uint32_t column;
};

struct ubsan_type_descriptor
{
    uint16_t kind;
    uint16_t info;
    char name[];
};

struct ubsan_overflow_data
{
    struct ubsan_source_location location;
    struct ubsan_type_descriptor *type;
};

struct ubsan_shift_out_of_bounds_data
{
    struct ubsan_source_location location;
    struct ubsan_type_descriptor *left_type;
    struct ubsan_type_descriptor *right_type;
};

struct ubsan_invalid_value_data
{
    struct ubsan_source_location location;
    struct ubsan_type_descriptor *type;
};

struct ubsan_array_out_of_bounds_data
{
    struct ubsan_source_location location;
    struct ubsan_type_descriptor *array_type;
    struct ubsan_type_descriptor *index_type;
};

struct ubsan_type_mismatch_v1_data
{
    struct ubsan_source_location location;
    struct ubsan_type_descriptor *type;
    unsigned char log_alignment;
    unsigned char type_check_kind;
};

struct ubsan_negative_vla_data
{
    struct ubsan_source_location location;
    struct ubsan_type_descriptor *type;
};

struct ubsan_nonnull_return_data
{
    struct ubsan_source_location location;
};

struct ubsan_nonnull_arg_data
{
    struct ubsan_source_location location;
};

struct ubsan_unreachable_data
{
    struct ubsan_source_location location;
};

struct ubsan_invalid_builtin_data
{
    struct ubsan_source_location location;
    unsigned char kind;
};

static void ubsan_print_location(char *message, struct ubsan_source_location loc)
{
    panic("tinyubsan: {} at file {}, line {}, column {}", message, str_cast(loc.file), loc.line, loc.column);
}

void __ubsan_handle_add_overflow(struct ubsan_overflow_data *data)
{
    ubsan_print_location("addition overflow", data->location);
}

void __ubsan_handle_sub_overflow(struct ubsan_overflow_data *data)
{
    ubsan_print_location("subtraction overflow", data->location);
}

void __ubsan_handle_mul_overflow(struct ubsan_overflow_data *data)
{
    ubsan_print_location("multiplication overflow", data->location);
}

void __ubsan_handle_divrem_overflow(struct ubsan_overflow_data *data)
{
    ubsan_print_location("division overflow", data->location);
}

void __ubsan_handle_negate_overflow(struct ubsan_overflow_data *data)
{
    ubsan_print_location("negation overflow", data->location);
}

void __ubsan_handle_pointer_overflow(struct ubsan_overflow_data *data)
{
    ubsan_print_location("pointer overflow", data->location);
}

void __ubsan_handle_shift_out_of_bounds(struct ubsan_shift_out_of_bounds_data *data)
{
    ubsan_print_location("shift out of bounds", data->location);
}

void __ubsan_handle_load_invalid_value(struct ubsan_invalid_value_data *data)
{
    ubsan_print_location("invalid load value", data->location);
}

void __ubsan_handle_out_of_bounds(struct ubsan_array_out_of_bounds_data *data)
{
    ubsan_print_location("array out of bounds", data->location);
}

void __ubsan_handle_type_mismatch_v1(struct ubsan_type_mismatch_v1_data *data, uintptr_t ptr)
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

void __ubsan_handle_vla_bound_not_positive(struct ubsan_negative_vla_data *data)
{
    ubsan_print_location("variable-length argument is negative", data->location);
}

void __ubsan_handle_nonnull_reubsanrn(struct ubsan_nonnull_return_data *data)
{
    ubsan_print_location("non-null reubsanrn is null", data->location);
}

void __ubsan_handle_nonnull_arg(struct ubsan_nonnull_arg_data *data)
{
    ubsan_print_location("non-null argument is null", data->location);
}

void __ubsan_handle_builtin_unreachable(struct ubsan_unreachable_data *data)
{

    ubsan_print_location("unreachable code reached", data->location);
}

void __ubsan_handle_invalid_builtin(struct ubsan_invalid_builtin_data *data)
{

    ubsan_print_location("invalid builtin", data->location);
}
