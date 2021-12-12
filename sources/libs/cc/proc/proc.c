#include <brutal/debug.h>
#include <cc/parse.h>
#include <cc/proc/proc.h>

void cproc_process_token(Lex *result, Lex *lex, MAYBE_UNUSED CProcContext *context, MAYBE_UNUSED Alloc *alloc);

void cproc_lex(Lex *result, Lex *from, CProcContext *ctx, Alloc *alloc);

void cproc_process_define(Lex *lex, CProcContext *ctx, Alloc *alloc)
{
    CProcMacro macro = {};
    vec_init(&macro.args, alloc);
    vec_init(&macro.lexemes, alloc);
    cparse_skip_separator(lex, CLEX_DEFINE);

    if (lex_curr(lex).type != CLEX_IDENT)
    {
        lex_throw(lex, str$("expected identifier"));
        return;
    }
    macro.name = lex_curr(lex).str;

    lex_skip_type(lex, CLEX_IDENT);

    // argument parsing, there should not have any space between the macro name and argument
    // A(a,b) != A (a,b)
    if (lex_curr_type(lex) == CLEX_LPARENT)
    {
        cparse_skip_separator(lex, CLEX_LPARENT);
        while (!cparse_is_separator(lex, CLEX_RPARENT) && !lex_ended(lex))
        {
            if (lex_curr(lex).type != CLEX_IDENT)
            {
                lex_throw(lex, str$("expected identifier"));

                return;
            }

            vec_push(&macro.args, lex_curr(lex).str);
            cparse_skip_separator(lex, CLEX_IDENT);

            if (cparse_is_separator(lex, CLEX_COMMA))
            {
                cparse_skip_separator(lex, CLEX_COMMA);
            }
        }
        cparse_skip_separator(lex, CLEX_RPARENT);
    }

    while (lex_curr_type(lex) != CLEX_NEWLINE && !lex_ended(lex))
    {
        vec_push(&macro.lexemes, lex_curr(lex));
        lex_next(lex);
    }
    vec_push(&ctx->macros, macro);
}

CProcMacro *cproc_macro_by_name(Str name, CProcContext *ctx)
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

Lexemes cproc_process_single_macro_arg(Lex *lex, Alloc *alloc)
{
    Lexemes arg;
    vec_init(&arg, alloc);
    int depth = 0;
    while (!lex_ended(lex))
    {
        if (lex_curr_type(lex) == CLEX_LPARENT)
        {
            depth++;
        }
        else if (lex_curr_type(lex) == CLEX_RPARENT)
        {
            if (depth == 0)
            {
                return arg;
            }

            depth--;
        }
        else if (lex_curr_type(lex) == CLEX_COMMA && depth == 0)
        {
            return arg;
        }

        vec_push(&arg, lex_curr(lex));
        lex_next(lex);
    }
    return arg;
}

bool cproc_is_already_in_macro(CProcContext *ctx, Str name)
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

void cproc_process_ident(Lex *result, Lex *lex, CProcContext *ctx, MAYBE_UNUSED Alloc *alloc)
{
    Str ident = lex_curr(lex).str;
    CProcMacro *macro = cproc_macro_by_name(ident, ctx);

    // if the identifier is not a macro or we are already in this macro
    if (macro == nullptr || cproc_is_already_in_macro(ctx, ident))
    {
        vec_push(&result->lexemes, lex_curr(lex));
        lex_next(lex);
        return;
    }

    lex_next(lex);
    Vec(Lexemes) macro_args_values;
    vec_init(&macro_args_values, alloc);

    if (lex_curr(lex).type == CLEX_LPARENT && macro->args.len > 0)
    {
        cparse_skip_separator(lex, CLEX_LPARENT);

        for (int i = 0; i < macro->args.len; i++)
        {
            Lexemes macro_arg = cproc_process_single_macro_arg(lex, alloc);
            vec_push(&macro_args_values, macro_arg);
            lex_next(lex);
        }
    }

    vec_push(&ctx->inside_macro, ident);

    // output all macro processed code in temp
    Lex temp = {};
    vec_init(&temp.lexemes, alloc);
    Lex macro_lex = lex_from_lexeme(&macro->lexemes);

    cproc_lex(&temp, &macro_lex, ctx, alloc);
    log$("macro {}", macro->name);
    vec_pop(&ctx->inside_macro);

    // replace all temp tokens with macro arguments if needed
    vec_foreach(entry, &temp.lexemes)
    {
        bool founded_arg = false;
        int arg_id = 0;
        vec_foreach(arg, &macro->args)
        {

            if (entry->type == CLEX_IDENT && str_eq(entry->str, *arg) && !founded_arg)
            {
                log$("replacing: {}", entry->str);

                Lexemes arg_lex = macro_args_values.data[arg_id];
                Lex v = lex_from_lexeme(&arg_lex);
                lex_dump(&v, clex_to_str);
                cproc_lex(result, &v, ctx, alloc);
                founded_arg = true;
            }
            arg_id++;
        }

        if (!founded_arg)
        {
            vec_push(&result->lexemes, *entry);
        }
    }

    log$("macro (end) {}", macro->name);
}

void cproc_process_token(Lex *result, Lex *lex, MAYBE_UNUSED CProcContext *context, MAYBE_UNUSED Alloc *alloc)
{
    Lexeme curr = lex_curr(lex);
    switch (curr.type)
    {
    case CLEX_COMMENT:
    case CLEX_NEWLINE:
        lex_next(lex);
        break;
    case CLEX_DEFINE:
        cproc_process_define(lex, context, alloc);
        lex_next(lex);
        break;
    case CLEX_IDENT:
        cproc_process_ident(result, lex, context, alloc);
        break;

    default:
        vec_push(&result->lexemes, curr);
        lex_next(lex);
        break;
    }
}

void cproc_lex(Lex *result, Lex *from, CProcContext *ctx, Alloc *alloc)
{
    while (!lex_ended(from))
    {
        cproc_process_token(result, from, ctx, alloc);
    }
}

Lex cproc_file(Lex *lex, Str filename, Alloc *alloc)
{
    CProcContext ctx = {};
    ctx.filename = filename;
    vec_init(&ctx.files_included, alloc);
    vec_init(&ctx.macros, alloc);
    vec_init(&ctx.include_path, alloc);
    vec_init(&ctx.lexemes, alloc);
    vec_init(&ctx.inside_macro, alloc);

    Lex result = {};
    vec_init(&result.lexemes, alloc);

    cproc_lex(&result, lex, &ctx, alloc);
    return result;
}
