#include "ast.h"
#include <brutal-debug>

void ud_ast_init(UdAst *self, Alloc *alloc)
{
    vec_init(&self->datas, alloc);
    vec_init(&self->infos, alloc);
    vec_init(&self->nodes, alloc);
}

void ud_ast_deinit(UdAst *self)
{
    vec_deinit(&self->datas);
    vec_deinit(&self->infos);
    vec_deinit(&self->nodes);
}

UdAstPtr ud_ast_push(UdAst *self, UdAstNode node, UdAstInfo info)
{
    int r = vec_len(&self->nodes);

    vec_push(&self->nodes, node);
    vec_push(&self->infos, info);

    return ud_ast_ptr_node$(r);
}

UdAstPtr ud_ast_push_no_info(UdAst *self, UdAstNode node)
{
    UdAstInfo info = {
        .end = UD_AST_INFO_USE_CHILD,
        .start = UD_AST_INFO_USE_CHILD,
        .sema_type = 0,
    };

    return ud_ast_push(self, node, info);
}

UdAstPtr ud_ast_push_data(UdAst *self, UdAstData data)
{
    int r = vec_len(&self->nodes);

    vec_push(&self->datas, data);

    return ud_ast_ptr_data$(r);
}

UdAstNode *ud_ast_node(UdAst *self, UdAstPtr ptr)
{
    assert_falsity((bool)ptr.is_data);
    return &self->nodes.data[ptr.index];
}

UdAstInfo *ud_ast_info(UdAst *self, UdAstPtr ptr)
{
    assert_falsity((bool)ptr.is_data);
    return &self->infos.data[ptr.index];
}

UdAstPtr ud_ast_push_list(UdAst *self, UdAstPtr list_node, int list_type, UdAstPtr pushed_index)
{
    UdAstNode *current = ud_ast_node(self, list_node);
    UdAstPtr current_id = list_node;

    assert_not_equal((int)pushed_index.index, 0);

    while (true)
    {
        assert_equal(current->type, list_type);

        if (current->left.index == 0)
        {
            current->left = pushed_index;
            return pushed_index;
        }
        else if (current->right.index == 0)
        {
            UdAstNode next = {
                .type = list_type,
                .right = ud_ast_ptr_null$(),
                .left = pushed_index,
            };

            UdAstPtr pushed_list_node = ud_ast_push_no_info(self, next);

            current->right = pushed_list_node;
            current = ud_ast_node(self, pushed_list_node);

            return pushed_index;
        }
        else
        {
            current_id = current->right;
            current = ud_ast_node(self, current_id);
        }
    }
}

UdAstData *ud_ast_data(UdAst *self, UdAstPtr ptr)
{
    assert_truth((bool)ptr.is_data);
    return &self->datas.data[ptr.index];
}
