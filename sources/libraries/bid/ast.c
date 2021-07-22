#include <bid/ast.h>
#include <brutal/alloc.h>

struct bid_ast_node *create_ast_node(enum bid_ast_node_type type)
{
    struct bid_ast_node *node = alloc_malloc(alloc_global(), sizeof(struct bid_ast_node));
    vec_init(&node->children, alloc_global());
    node->type = type;
    return node;
}

void destroy_ast_node_recursive(struct bid_ast_node *from)
{
    if (from != NULL)
    {
        for (int i = 0; i < from->children.length; i++)
        {
            destroy_ast_node_recursive(from->children.data[i]);
        }

        if (from->children.data != NULL)
        {
            vec_deinit(&from->children);
        }

        alloc_free(alloc_global(), from);
    }
}
