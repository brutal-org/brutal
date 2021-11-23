#include "kernel/riscv64/fdt.h"
#include <brutal/debug.h>
#include <brutal/io/emit.h>
#include <ctype.h>
#include "kernel/arch.h"

FdtBeginNode *fdt_root(FdtHeader *fdt)
{
    FdtHeader *header = (FdtHeader *)fdt;

    assert_equal(load_be(header->magic), FDT_MAGIC);

    return (FdtBeginNode *)((uintptr_t)fdt + load_be(header->structure_offset));
}

be_uint32_t *skip_fdt(be_uint32_t *cur)
{
    uint32_t cur_type = load_be(*cur);
    if (cur_type == FDT_BEGIN_NODE)
    {
        FdtBeginNode *node = (FdtBeginNode *)cur;
        cur += (ALIGN_UP(str$(node->name).len + 1, 4) / 4);
    }
    else if (cur_type == FDT_PROP)
    {
        FdtPropertiesNode *node = (FdtPropertiesNode *)cur;
        cur += (ALIGN_UP(load_be(node->len), 4) / 4);
        cur += 2;
    }
    else if (cur_type == FDT_END)
    {
        return cur;
    }
    cur++;

    return cur;
}

void dump_fdt(FdtHeader *header, FdtBeginNode *current_fdt)
{
    Emit emit;
    emit_init(&emit, arch_debug());

    be_uint32_t *cur = (be_uint32_t *)current_fdt;
    uint32_t cur_type = load_be(*cur);

    while (cur_type != FDT_END)
    {
        if (cur_type == FDT_BEGIN_NODE)
        {
            FdtBeginNode *node = (FdtBeginNode *)cur;
            emit_fmt(&emit, " - \"{}\" \n", str$(node->name));
            emit_ident(&emit);
        }
        else if (cur_type == FDT_PROP)
        {
            FdtPropertiesNode *cnode = (FdtPropertiesNode *)cur;
            Str node_name = str$((const char *)((uintptr_t)header + load_be(header->strings_offset) + load_be(cnode->name_offset)));
            emit_fmt(&emit, " - property: \"{}\" \n", node_name);
        }
        else if (cur_type == FDT_END_NODE)
        {
            emit_deident(&emit);
        }

        cur = skip_fdt(cur);
        cur_type = load_be(*cur);
    }

    emit_deinit(&emit);
}

FdtBeginNode *get_fdt_node(FdtBeginNode *current_fdt, Str entry)
{
    be_uint32_t *cur = (be_uint32_t *)current_fdt;
    uint32_t cur_type = 0;
    int depth = 0;

    while (cur_type != FDT_END && depth >= 0)
    {
        cur = skip_fdt(cur);
        cur_type = load_be(*cur);

        if (cur_type == FDT_BEGIN_NODE)
        {
            FdtBeginNode *node = (FdtBeginNode *)cur;
            if (str_eq(str$(node->name), entry) && depth == 0)
            {
                return node;
            }
            else
            {
                depth++;
            }
        }
        else if (cur_type == FDT_END_NODE)
        {
            depth--;
        }
        else if (cur_type == FDT_END)
        {
            return NULL;
        }
    }

    return NULL;
}

NodeProperty get_fdt_property(FdtHeader *header, FdtBeginNode *node, Str name)
{
    NodeProperty result;

    be_uint32_t *cur = (be_uint32_t *)node;
    uint32_t cur_type = 0;
    int depth = 0;

    while (cur_type != FDT_END && depth >= 0)
    {
        cur = skip_fdt(cur);
        cur_type = load_be(*cur);

        if (cur_type == FDT_BEGIN_NODE)
        {
            depth++;
        }
        else if (cur_type == FDT_PROP)
        {
            FdtPropertiesNode *cnode = (FdtPropertiesNode *)cur;

            Str node_name = str$((const char *)((uintptr_t)header + load_be(header->strings_offset) + load_be(cnode->name_offset)));

            if (str_eq(name, node_name) && depth == 0)
            {
                result.property_name = node_name;
                result.property_value_size = load_be(cnode->len);
                result.property_value = cnode->value;
                return result;
            }
        }
        else if (cur_type == FDT_END_NODE)
        {
            depth--;
        }
    }

    return (NodeProperty){};
}
