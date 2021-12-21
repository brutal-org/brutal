#include <brutal/debug.h>
#include <cc/parse.h>
#include <cc/proc/macro.h>
#include <cc/proc/proc.h>

static CProcMacro *cproc_macro_by_name(Str name, CProcContext *ctx)
{
    vec_foreach(v, &ctx->macros)
    {
        if (str_eq(v->name, name))
        {
            return v;
        }
    }
    return nullptr;
}

static bool cproc_is_already_in_macro(CProcContext *ctx, Str name)
{
    vec_foreach(macro_name, &ctx->inside_macro)
    {
        if (str_eq(*macro_name, name))
        {
            return true;
        }
    }
    return false;
}

void cproc_parse_ident(Lex *out, Lex *source, CProcContext *ctx)
{
    CProcMacro *macro = cproc_macro_by_name(lex_curr(source).str, ctx);

    // if the identifier is not a macro or we are already in this macro
    if (macro == nullptr || cproc_is_already_in_macro(ctx, macro->name))
    {
        vec_push(&out->lexemes, lex_curr(source));
        lex_next(source);
        return;
    }

    lex_next(source);
    if (cparse_peek_separator(source, 1).type != CLEX_LPARENT && macro->args.len != 0)
    {
        vec_push(&out->lexemes, lex_curr(source));
        return;
    }

    cproc_gen_macro(out, source, ctx, macro);
}

void cproc_parse_directive(MAYBE_UNUSED Lex *result, Lex *source, CProcContext *context)
{
    lex_skip_type(source, CLEX_WHITESPACE);
    if (lex_curr(source).type == CLEX_NEWLINE) // null directive
    {
        lex_next(source);
        return;
    }
    if (lex_curr(source).type != CLEX_IDENT)
    {
        lex_throw(source, str$("expected identifier"));
        return;
    }

    if (str_eq(lex_curr(source).str, str$("define")))
    {
        cproc_parse_define(source, context);
    }
    else
    {
        lex_throw(source, str$("unknown directive"));
    }
}

void cproc_lex(Lex *result, Lex *source, CProcContext *ctx)
{
    bool can_be_directive = true;
    while (!lex_ended(source))
    {
        Lexeme curr = lex_curr(source);

        switch (curr.type)
        {
        case CLEX_COMMENT:
            lex_next(source);
            break;

        case CLEX_NEWLINE:
            lex_next(source);
            can_be_directive = true;
            break;

        case CLEX_POUND:
            if (can_be_directive)
            {
                lex_next(source);
                cproc_parse_directive(result, source, ctx);
                can_be_directive = false;
            }
            else
            {
                lex_throw(source, str$("'#' can be placed only at the start of a line"));
            }
            break;

        default:
            cproc_parse_ident(result, source, ctx);
            can_be_directive = false;
            break;
        }
    }
}

Lex cproc_file(Lex *source, Str filename, Alloc *alloc)
{
    CProcContext ctx = {};
    ctx.filename = filename;
    ctx.alloc = alloc;
    vec_init(&ctx.files_included, alloc);
    vec_init(&ctx.macros, alloc);
    vec_init(&ctx.include_path, alloc);
    vec_init(&ctx.source, alloc);
    vec_init(&ctx.inside_macro, alloc);

    Lex result = {};
    vec_init(&result.lexemes, alloc);

    cproc_lex(&result, source, &ctx);
    return result;
}
