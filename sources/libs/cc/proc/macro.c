#include <cc/parse.h>
#include <cc/proc/macro.h>
#include <cc/proc/proc.h>

void cproc_parse_define(Lex *source, CProcContext *ctx)
{
    CProcMacro macro = {};
    vec_init(&macro.args, ctx->alloc);
    vec_init(&macro.source, ctx->alloc);

    cparse_skip_separator(source, CLEX_DEFINE);

    if (lex_curr(source).type != CLEX_IDENT)
    {
        lex_throw(source, str$("expected identifier"));
        return;
    }

    macro.name = lex_curr(source).str;

    lex_skip_type(source, CLEX_IDENT);

    // argument parsing, there should not have any space between the macro name and argument
    // A(a,b) != A (a,b)
    if (cparse_skip_separator(source, CLEX_LPARENT))
    {
        while (!cparse_skip_separator(source, CLEX_RPARENT) && !lex_ended(source))
        {
            if (lex_curr(source).type != CLEX_IDENT)
            {
                lex_throw(source, str$("expected identifier"));

                return;
            }

            vec_push(&macro.args, lex_curr(source).str);
            cparse_skip_separator(source, CLEX_IDENT);

            cparse_skip_separator(source, CLEX_COMMA);
        }
    }

    while (lex_curr_type(source) != CLEX_NEWLINE && !lex_ended(source))
    {
        vec_push(&macro.source, lex_curr(source));
        lex_next(source);
    }

    vec_push(&ctx->macros, macro);
}

static Lexemes cproc_parse_a_call_macro_arg(Lex *source, Alloc *alloc)
{
    Lexemes arg;
    vec_init(&arg, alloc);
    int depth = 0;

    while (!lex_ended(source))
    {
        if (lex_curr_type(source) == CLEX_LPARENT)
        {
            depth++;
        }
        else if (lex_curr_type(source) == CLEX_RPARENT)
        {
            if (depth == 0)
            {
                lex_next(source);
                return arg;
            }

            depth--;
        }
        else if (lex_curr_type(source) == CLEX_COMMA && depth == 0)
        {
            lex_next(source);
            return arg;
        }

        vec_push(&arg, lex_curr(source));
        lex_next(source);
    }

    return arg;
}

static MacroArgsCodes cproc_parse_macro_call_args(CProcMacro *macro, Lex *source, Alloc *alloc)
{
    MacroArgsCodes macro_args_values;
    vec_init(&macro_args_values, alloc);

    if (lex_curr(source).type != CLEX_LPARENT || macro->args.len == 0)
    {
        return macro_args_values;
    }

    cparse_skip_separator(source, CLEX_LPARENT);

    for (int i = 0; i < macro->args.len; i++)
    {
        Lexemes macro_arg = cproc_parse_a_call_macro_arg(source, alloc);
        vec_push(&macro_args_values, macro_arg);
    }

    return macro_args_values;
}

static Lex cproc_gen_raw_extended_macro_code(CProcMacro *macro, CProcContext *ctx)
{
    vec_push(&ctx->inside_macro, macro->name);

    // output all macro processed code in temp
    Lex output = {};
    vec_init(&output.lexemes, ctx->alloc);

    Lex macro_lex = lex_from_lexeme(&macro->source); // take the macro source code

    cproc_lex(&output, &macro_lex, ctx); // process the macro source code and put it in macro_lex

    vec_pop(&ctx->inside_macro);

    return output;
}

static void cproc_gen_macro_args_replacement(CProcMacro *macro, MacroArgsCodes *macro_args_values, Lex *out, Lex *macro_source, CProcContext *ctx)
{
    // we loop for each token and we want to find a token equal to an argument name
    vec_foreach(entry, &macro_source->lexemes)
    {
        bool founded_arg = false;
        int arg_id = 0;
        vec_foreach(arg, &macro->args)
        {
            if (entry->type == CLEX_IDENT && str_eq(entry->str, *arg))
            {
                Lexemes arg_lex = macro_args_values->data[arg_id];
                Lex v = lex_from_lexeme(&arg_lex);
                // process argument
                cproc_lex(out, &v, ctx);
                founded_arg = true;
                break;
            }
            arg_id++;
        }

        if (!founded_arg)
        {
            vec_push(&out->lexemes, *entry);
        }
    }
}

void cproc_gen_macro(Lex *out, Lex *source, CProcContext *ctx, CProcMacro *macro)
{
    // first we parse the argument
    MacroArgsCodes macro_args_values = cproc_parse_macro_call_args(macro, source, ctx->alloc);

    // we generate raw macro code
    Lex temp = cproc_gen_raw_extended_macro_code(macro, ctx);

    // we apply the arguments
    cproc_gen_macro_args_replacement(macro, &macro_args_values, out, &temp, ctx);
}
