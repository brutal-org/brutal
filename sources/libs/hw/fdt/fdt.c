#include <brutal/debug.h>
#include <hw/fdt/fdt.h>

static FdtTok *fdt_node_begin(FdtTokNode *fdt)
{
    return fdt_tok_next((FdtTok *)fdt);
}

static FdtTok *fdt_node_end(FdtTokNode *fdt, FdtHeader *header)
{
    uint32_t cur_type = 0;
    int depth = 0;

    for (FdtTok *cur = fdt_node_begin(fdt); cur < fdt_tok_end(header); cur = fdt_tok_next(cur))
    {
        cur_type = load_be(*cur);
        if (depth < 0 || cur_type == FDT_END)
        {
            return (cur);
        }
        if (cur_type == FDT_BEGIN_NODE)
        {
            depth++;
        }
        else if (cur_type == FDT_END_NODE)
        {
            depth--;
        }
    }
    return NULL;
}

static FdtNode fdt_tok2node(FdtTokNode *raw_node, FdtHeader *header)
{
    return (FdtNode){
        .fdt = header,
        .begin = fdt_node_begin(raw_node),
        .end = fdt_node_end(raw_node, header),
        .name = str$(raw_node->name),
    };
}

static FdtProp fdt_tok2prop(FdtTokProp *raw_prop, FdtHeader *header)
{
    Str node_name = str$((const char *)((uintptr_t)header + load_be(header->strings_offset) + load_be(raw_prop->name_offset)));

    return (FdtProp){
        .name = node_name,
        .value = slice$(VoidSlice, raw_prop->value, load_be(raw_prop->len)),
    };
}
FdtHeader *fdt_from_data(void *raw_data)
{
    FdtHeader *header = (FdtHeader *)raw_data;

    assert_equal(load_be(header->magic), FDT_MAGIC);
    return header;
}

FdtTok *fdt_tok_begin(FdtHeader *fdt)
{
    return (FdtTok *)((uintptr_t)fdt + load_be(fdt->structure_offset));
}

FdtTok *fdt_tok_next(FdtTok *tok)
{
    uint32_t cur_type = load_be(*tok);
    if (cur_type == FDT_BEGIN_NODE)
    {
        FdtTokNode *node = (FdtTokNode *)tok;
        tok += (ALIGN_UP(str$(node->name).len + 1, 4) / 4);
    }
    else if (cur_type == FDT_PROP)
    {
        FdtTokProp *node = (FdtTokProp *)tok;
        tok += (ALIGN_UP(load_be(node->len), 4) / 4);
        tok += 2;
    }
    tok++;

    return tok;
}

FdtTok *fdt_tok_end(FdtHeader *fdt)
{
    FdtTok *tok = fdt_tok_begin(fdt);
    tok += (load_be(fdt->structure_size) / 4) + 1;
    return tok;
}

FdtNode fdt_node_root(FdtHeader *fdt)
{
    return (FdtNode){
        .fdt = fdt,
        .begin = fdt_tok_begin(fdt),
        .end = fdt_tok_end(fdt),
        .name = str$(""),
    };
}

Iter fdt_node_childs(FdtNode node, IterFn fn, void *ctx)
{
    uint32_t cur_type = 0;
    int depth = 0;

    for (FdtTok *cur = node.begin; cur < node.end; cur = fdt_tok_next(cur))
    {
        cur_type = load_be(*cur);
        if (cur_type == FDT_BEGIN_NODE)
        {
            FdtTokNode *raw_node = (FdtTokNode *)cur;

            if (depth == 0)
            {
                FdtNode res = fdt_tok2node(raw_node, node.fdt);
                if (fn(&res, ctx) == ITER_STOP)
                {
                    return ITER_STOP;
                }
            }
            depth++;
        }
        else if (cur_type == FDT_END_NODE)
        {
            depth--;
        }
    }

    return ITER_CONTINUE;
}

Iter fdt_node_props(FdtNode node, IterFn fn, void *ctx)
{
    uint32_t cur_type = 0;
    int depth = 0;

    for (FdtTok *cur = node.begin; cur < node.end; cur = fdt_tok_next(cur))
    {
        cur_type = load_be(*cur);
        if (cur_type == FDT_BEGIN_NODE)
        {
            depth++;
        }
        else if (cur_type == FDT_END_NODE)
        {
            depth--;
        }
        else if (cur_type == FDT_PROP && depth == 0)
        {
            FdtTokProp *raw_prop = (FdtTokProp *)cur;
            FdtProp prop = fdt_tok2prop(raw_prop, node.fdt);
            if (fn(&prop, ctx) == ITER_STOP)
            {
                return ITER_STOP;
            }
        }
    }

    return ITER_CONTINUE;
}

typedef struct
{
    Str name;
    FdtNode res;
} FdtNodeLookupCtx;

static Iter fdt_lookup_node_iter(FdtNode *node, FdtNodeLookupCtx *ctx)
{
    int part_end = str_first_chr(ctx->name, '/');

    if (part_end < 0)
    {
        part_end = ctx->name.len;
    }

    Str node_name = str_sub(ctx->name, 0, part_end);

    if (str_eq(node_name, node->name))
    {
        if (part_end == (int)ctx->name.len)
        {
            ctx->res = *node;
            return ITER_STOP;
        }
        else
        {
            ctx->name = str_sub(ctx->name, part_end + 1, (int)ctx->name.len);

            if (fdt_node_childs(*node, (IterFn *)fdt_lookup_node_iter, ctx) == ITER_STOP)
            {
                return ITER_STOP;
            }
        }
    }

    return ITER_CONTINUE;
}

FdtNode fdt_lookup_node(FdtHeader *fdt, Str path)
{
    FdtNode node = fdt_node_root(fdt);
    FdtNodeLookupCtx ctx = {
        .name = path,
    };

    fdt_node_childs(node, (IterFn *)fdt_lookup_node_iter, &ctx);

    return ctx.res;
}

typedef struct
{
    Str name;
    FdtProp res;
} FdtPropCtx;

static Iter fdt_lookup_prop_iter(FdtProp *node, FdtPropCtx *res)
{
    if (str_eq(node->name, res->name) == 0)
    {
        res->res = *node;
        return ITER_STOP;
    }

    return ITER_CONTINUE;
}

FdtProp fdt_lookup_props(FdtNode node, Str name)
{
    FdtPropCtx ctx = {
        .name = name,
    };

    fdt_node_props(node, (IterFn *)fdt_lookup_prop_iter, &ctx);

    return ctx.res;
}

static Iter fdt_dump_props_iter(FdtProp *prop, Emit *out)
{
    emit_fmt(out, "- prop: {}\n", prop->name);
    return ITER_CONTINUE;
}

static Iter fdt_dump_node_iter(FdtNode *node, Emit *out)
{
    emit_fmt(out, "node: {}\n", node->name);
    emit_ident(out);
    fdt_node_props(*node, (IterFn *)fdt_dump_props_iter, out);
    fdt_node_childs(*node, (IterFn *)fdt_dump_node_iter, out);
    emit_deident(out);
    return ITER_CONTINUE;
}

void fdt_dump(FdtHeader *fdt, Emit *out)
{
    FdtNode root = fdt_node_root(fdt);
    emit_fmt(out, "fdt-dump:\n");
    emit_ident(out);

    fdt_node_childs(root, (IterFn *)fdt_dump_node_iter, out);

    emit_deident(out);

    emit_fmt(out, "fdt reserved memory:\n");
    FdtReservationEntry *entry = ((void *)fdt) + load_be(fdt->memory_reservation_offset);
    emit_ident(out);
    while (load_be(entry->size) != 0 || load_be(entry->address) != 0)
    {
        emit_fmt(out, "- [{#x}] - [{#x}] \n", load_be(entry->address), load_be(entry->address) + load_be(entry->size));
        entry++;
    }
    emit_deident(out);
}
