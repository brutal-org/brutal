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

    cproc_gen_macro(out, source, ctx, macro);
}

void cproc_parse_token(Lex *result, Lex *source, CProcContext *context)
{
    Lexeme curr = lex_curr(source);
    switch (curr.type)
    {
    case CLEX_COMMENT:
    case CLEX_NEWLINE:
        lex_next(source);
        break;
    case CLEX_DEFINE:
        cproc_parse_define(source, context);
        lex_next(source);
        break;
    case CLEX_IDENT:
        cproc_parse_ident(result, source, context);
        break;

    default:
        vec_push(&result->lexemes, curr);
        lex_next(source);
        break;
    }
}

void cproc_lex(Lex *out, Lex *source, CProcContext *ctx)
{
    while (!lex_ended(source))
    {
        cproc_parse_token(out, source, ctx);
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
