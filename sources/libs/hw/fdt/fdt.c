#include <brutal/debug.h>
#include <fdt/fdt.h>

static FdtTok *fdt_node_begin(FdtTokBegin *fdt)
{
    return fdt_tok_next((FdtTok *)fdt);
}

static FdtTok *fdt_node_end(FdtTokBegin *fdt, FdtHeader *header)
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

static FdtNode fdt_tok2node(FdtTokBegin *raw_node, FdtHeader *header)
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
        FdtTokBegin *node = (FdtTokBegin *)tok;
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
            FdtTokBegin *raw_node = (FdtTokBegin *)cur;

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

static bool node_name_eq(Str const searched, Str const node_name)
{
    if (str_last_chr(searched, '@') == (int)searched.len - 1 && searched.len != 0)
    {
        int node_name_end = str_first_chr(node_name, '@');
        Str compared = str_sub(node_name, 0, node_name_end);

        return str_eq(str_sub(searched, 0, searched.len - 1), compared);
    }

    return str_eq(searched, node_name);
}

static Iter fdt_lookup_node_iter(FdtNode *node, FdtNodeLookupCtx *ctx)
{
    int part_end = str_first_chr(ctx->name, '/');

    if (part_end < 0)
    {
        part_end = ctx->name.len;
    }

    Str node_name = str_sub(ctx->name, 0, part_end);

    if (node_name_eq(node_name, node->name))
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
    if (str_eq(node->name, res->name))
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

static void fdt_dump_props_value(FdtProp *prop, Emit *out)
{
    if (prop->value.len % sizeof(uint32_t) == 0)
    {
        be_uint32_t *val = ((be_uint32_t *)prop->value.buf);
        for (unsigned int i = 0; i < prop->value.len / sizeof(uint32_t); i++)
        {
            emit_fmt(out, "- as uint32_t: {#x} \n", load_be(val[i]));
        }
    }
    if (prop->value.len % sizeof(uint64_t) == 0)
    {
        be_uint64_t *val = ((be_uint64_t *)prop->value.buf);
        for (unsigned int i = 0; i < prop->value.len / sizeof(uint64_t); i++)
        {
            emit_fmt(out, "- as uint64_t: {#x}\n", load_be(val[i]));
        }
    }
    if (prop->value.len > 2)
    {
        Str as_string = str_n$(prop->value.len, (char *)prop->value.buf);
        emit_fmt(out, "- as string: {} \n", as_string);
    }

    emit_fmt(out, "- as data: ");
    for (unsigned int i = 0; i < prop->value.len; i++)
    {
        emit_fmt(out, "{#x} ", ((uint8_t *)prop->value.buf)[i]);
    }
    emit_fmt(out, "\n");
}

typedef struct
{
    Emit *out;
    bool dump_values;
} IterDumpCtx;

static Iter fdt_dump_props_iter(FdtProp *prop, IterDumpCtx *ctx)
{
    emit_fmt(ctx->out, "- prop: {}\n", prop->name);
    emit_ident(ctx->out);
    if (ctx->dump_values)
    {
        fdt_dump_props_value(prop, ctx->out);
    }
    emit_deident(ctx->out);
    return ITER_CONTINUE;
}

static Iter fdt_dump_node_iter(FdtNode *node, IterDumpCtx *ctx)
{
    emit_fmt(ctx->out, "node: {}\n", node->name);
    emit_ident(ctx->out);
    fdt_node_props(*node, (IterFn *)fdt_dump_props_iter, ctx);
    fdt_node_childs(*node, (IterFn *)fdt_dump_node_iter, ctx);
    emit_deident(ctx->out);
    return ITER_CONTINUE;
}

void fdt_dump(FdtHeader *fdt, Emit *out, bool dump_values)
{
    FdtNode root = fdt_node_root(fdt);
    IterDumpCtx ctx = {
        .out = out,
        .dump_values = dump_values,
    };

    emit_fmt(out, "fdt-dump:\n");
    emit_ident(out);
    fdt_node_childs(root, (IterFn *)fdt_dump_node_iter, &ctx);

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

USizeRange fdt_reg_get_range(FdtProp prop, MAYBE_UNUSED uintptr_t addr_size, MAYBE_UNUSED uintptr_t len_size)
{
    be_uint64_t *reg = (be_uint64_t *)prop.value.buf;
    assert_not_null(reg);
    return (USizeRange){.base = load_be(*reg), .size = load_be(*(reg + 1))};
}
