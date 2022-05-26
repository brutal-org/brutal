#include "dump.h"

#include "types.h"
Json ast_dump_val(UdAstData *data)
{

    switch (data->data_type)
    {
    case UD_AST_DATA_BOOL:
        return json_bool(data->boolean);
    case UD_AST_DATA_STR:
        return json_str(data->string);
    case UD_AST_DATA_CHAR:
        return json_str(data->string);
    case UD_AST_DATA_FLOATING:
        return json_str(str$("floating"));
    case UD_AST_DATA_INTEGER:
        return json_number(data->integer);
    case UD_AST_DATA_NULL:
    default:
        return json_str(str$("null"));
    }
}

Json ast_dump_impl(UdAst *ast, UdAstPtr ptr, Alloc *alloc)
{
    if (ptr.is_data)
    {
        return ast_dump_val(ud_ast_data(ast, ptr));
    }

    UdAstNode *node = ud_ast_node(ast, ptr);

    Json res = json_object(alloc);
    json_put(&res, str$("type"), json_str(udast_type_to_str(node->type)));

    if (!is_ud_ast_null$(node->left))
    {
        json_put(&res, str$("left"), ast_dump_impl(ast, node->left, alloc));
    }

    if (!is_ud_ast_null$(node->right))
    {
        json_put(&res, str$("right"), ast_dump_impl(ast, node->right, alloc));
    }

    return res;
}

Json ast_dump(UdAst *ast, Alloc *alloc)
{
    return ast_dump_impl(ast, ud_ast_root$(), alloc);
}
