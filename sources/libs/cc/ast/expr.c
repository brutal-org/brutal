#include <cc/ast/expr.h>

const char *cop_type_to_str[] = {
    [COP_INC] = "++",
    [COP_DEC] = "--",
    [COP_ASSIGN] = "=",
    [COP_ASSIGN_ADD] = "+=",
    [COP_ASSIGN_SUB] = "-=",
    [COP_ASSIGN_MULT] = "*=",
    [COP_ASSIGN_DIV] = "/=",
    [COP_ASSIGN_MOD] = "%=",
    [COP_ASSIGN_BIT_AND] = "&=",
    [COP_ASSIGN_BIT_OR] = "|=",
    [COP_ASSIGN_BIT_XOR] = "^=",
    [COP_ASSIGN_LSHIFT] = "<<=",
    [COP_ASSIGN_RSHIFT] = ">>=",
    [COP_ADD] = "+",
    [COP_SUB] = "-",
    [COP_MULT] = "*",
    [COP_DIV] = "/",
    [COP_MOD] = "%",
    [COP_BIT_NOT] = "~",
    [COP_BIT_AND] = "&",
    [COP_BIT_OR] = "|",
    [COP_BIT_XOR] = "^",
    [COP_LSHIFT] = "<<",
    [COP_RSHIFT] = ">>",
    [COP_NOT] = "!",
    [COP_AND] = "&&",
    [COP_OR] = "||",
    [COP_EQ] = "==",
    [COP_NOT_EQ] = "!=",
    [COP_LT] = "<",
    [COP_GT] = ">",
    [COP_LT_EQ] = "<=",
    [COP_GT_EQ] = ">=",
    [COP_INDEX] = "[]",
    [COP_DEREF] = "*",
    [COP_REF] = "&",
    [COP_ACCESS] = ".",
    [COP_PTR_ACCESS] = "->",
};

Str cop_to_str(COp type)
{
    return str$(cop_type_to_str[type]);
}
