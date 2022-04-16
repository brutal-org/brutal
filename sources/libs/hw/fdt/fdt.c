#include <brutal-debug>
#include <brutal-fmt>
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
        cur_type = be_load$(*cur);
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
    Str node_name = str$((char const *)((uintptr_t)header + be_load$(header->strings_offset) + be_load$(raw_prop->name_offset)));

    return (FdtProp){
        .name = node_name,
        .value = slice$(VoidSlice, raw_prop->value, be_load$(raw_prop->len)),
    };
}
FdtHeader *fdt_from_data(void *raw_data)
{
    FdtHeader *header = (FdtHeader *)raw_data;

    assert_equal(be_load$(header->magic), FDT_MAGIC);
    return header;
}

FdtTok *fdt_tok_begin(FdtHeader *fdt)
{
    return (FdtTok *)((uintptr_t)fdt + be_load$(fdt->structure_offset));
}

FdtTok *fdt_tok_next(FdtTok *tok)
{
    uint32_t cur_type = be_load$(*tok);
    if (cur_type == FDT_BEGIN_NODE)
    {
        FdtTokBegin *node = (FdtTokBegin *)tok;
        tok += (align_up$(str$(node->name).len + 1, 4) / 4);
    }
    else if (cur_type == FDT_PROP)
    {
        FdtTokProp *node = (FdtTokProp *)tok;
        tok += (align_up$(be_load$(node->len), 4) / 4);
        tok += 2;
    }
    tok++;

    return tok;
}

FdtTok *fdt_tok_end(FdtHeader *fdt)
{
    FdtTok *tok = fdt_tok_begin(fdt);
    tok += (be_load$(fdt->structure_size) / 4) + 1;
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
        cur_type = be_load$(*cur);
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
        cur_type = be_load$(*cur);
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

static void fdt_dump_props_value(FdtProp *prop, Emit *emit)
{
    if (prop->value.len % sizeof(uint32_t) == 0)
    {
        be_uint32_t *val = ((be_uint32_t *)prop->value.buf);
        for (unsigned int i = 0; i < prop->value.len / sizeof(uint32_t); i++)
        {
            emit_fmt$(emit, "- as uint32_t: {#x} \n", be_load$(val[i]));
        }
    }
    if (prop->value.len % sizeof(uint64_t) == 0)
    {
        be_uint64_t *val = ((be_uint64_t *)prop->value.buf);
        for (unsigned int i = 0; i < prop->value.len / sizeof(uint64_t); i++)
        {
            emit_fmt$(emit, "- as uint64_t: {#x}\n", be_load$(val[i]));
        }
    }
    if (prop->value.len > 2)
    {
        Str as_string = str_n$(prop->value.len, (char *)prop->value.buf);
        emit_fmt$(emit, "- as string: {} \n", as_string);
    }

    emit_fmt$(emit, "- as data: ");
    for (unsigned int i = 0; i < prop->value.len; i++)
    {
        emit_fmt$(emit, "{#x} ", ((uint8_t *)prop->value.buf)[i]);
    }
    emit_fmt$(emit, "\n");
}

typedef struct
{
    Emit *emit;
    bool dump_values;
} IterDumpCtx;

static Iter fdt_dump_props_iter(FdtProp *prop, IterDumpCtx *ctx)
{
    emit_fmt$(ctx->emit, "- prop: {}\n", prop->name);
    emit_ident(ctx->emit);
    if (ctx->dump_values)
    {
        fdt_dump_props_value(prop, ctx->emit);
    }
    emit_deident(ctx->emit);
    return ITER_CONTINUE;
}

static Iter fdt_dump_node_iter(FdtNode *node, IterDumpCtx *ctx)
{
    emit_fmt$(ctx->emit, "node: {}\n", node->name);
    emit_ident(ctx->emit);
    fdt_node_props(*node, (IterFn *)fdt_dump_props_iter, ctx);
    fdt_node_childs(*node, (IterFn *)fdt_dump_node_iter, ctx);
    emit_deident(ctx->emit);
    return ITER_CONTINUE;
}

void fdt_dump(FdtHeader *fdt, Emit *emit, bool dump_values)
{
    FdtNode root = fdt_node_root(fdt);
    IterDumpCtx ctx = {
        .emit = emit,
        .dump_values = dump_values,
    };

    emit_fmt$(emit, "fdt-dump:\n");
    emit_ident(emit);
    fdt_node_childs(root, (IterFn *)fdt_dump_node_iter, &ctx);

    emit_deident(emit);

    emit_fmt$(emit, "fdt reserved memory:\n");
    FdtReservationEntry *entry = ((void *)fdt) + be_load$(fdt->memory_reservation_offset);
    emit_ident(emit);
    while (be_load$(entry->size) != 0 || be_load$(entry->address) != 0)
    {
        emit_fmt$(emit, "- [{#x}] - [{#x}] \n", be_load$(entry->address), be_load$(entry->address) + be_load$(entry->size));
        entry++;
    }
    emit_deident(emit);
}

USizeRange fdt_reg_get_range(FdtProp prop, MAYBE_UNUSED uintptr_t addr_size, MAYBE_UNUSED uintptr_t len_size)
{
    be_uint64_t *reg = (be_uint64_t *)prop.value.buf;
    assert_not_null(reg);
    return (USizeRange){.base = be_load$(*reg), .size = be_load$(*(reg + 1))};
}
