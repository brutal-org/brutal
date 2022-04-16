#include <cc/lex/pproc.h>
#include <cc/parse/parser.h>
#include <brutal-debug>

CProcMacro cproc_macro_init(CProc *ctx, Str name)
{
    CProcMacro macro = {};
    vec_init(&macro.args, ctx->alloc);
    vec_init(&macro.source, ctx->alloc);
    macro.name = name;
    return macro;
}

void cproc_macro_code(CProcMacro *target, Lex *source)
{
    while (lex_curr_type(source) != CLEX_NEWLINE && !lex_ended(source))
    {
        vec_push(&target->source, lex_curr(source));
        lex_next(source);
    }
}

CProcMacro *cproc_define(CProc *ctx, Str name, Str value)
{
    CProcMacro macro = cproc_macro_init(ctx, name);

    Scan scan = {};

    scan_init(&scan, value);
    Lex source = clex(&scan, ctx->alloc);

    cproc_macro_code(&macro, &source);

    vec_push(&ctx->macros, macro);

    return vec_end(&ctx->macros);
}

void cproc_define_arg(CProcMacro *targ, Str name)
{
    vec_push(&targ->args, name);
}

void cproc_parse_define(Lex *source, CProc *ctx)
{
    cparse_skip_separator(source, CLEX_IDENT); // # {define}

    CProcMacro macro = cproc_macro_init(ctx, lex_curr(source).str);
    macro.name = lex_curr(source).str;

    lex_next(source);

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

    cproc_macro_code(&macro, source);
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
    cparse_whitespace(source);

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

static Lex cproc_gen_macro_code(CProcMacro *macro)
{
    return lex_from_lexeme(&macro->source); // take the macro source code
}

static Lex cproc_gen_extended_macro_code(CProcMacro *macro, Lex *macro_code, CProc *ctx)
{
    vec_push(&ctx->inside_macro, macro->name);

    // output all macro processed code in temp
    Lex output = {};
    vec_init(&output.lexemes, ctx->alloc);

    cproc_lex(&output, macro_code, ctx); // process the macro source code and put it in macro_lex

    (void)vec_pop(&ctx->inside_macro);

    return output;
}

static void cproc_gen_macro_args_replacement(CProcMacro *macro, MacroArgsCodes *macro_args_values, Lex *out, Lex *macro_source, CProc *ctx)
{
    // we loop for each token and we want to find a token equal to an argument name

    vec_foreach(entry, &macro_source->lexemes)
    {
        bool found_arg = false;
        int arg_id = 0;
        vec_foreach(arg, &macro->args)
        {
            if (entry->type == CLEX_IDENT && str_eq(entry->str, *arg))
            {
                Lexemes arg_lex = macro_args_values->data[arg_id];
                Lex v = lex_from_lexeme(&arg_lex);
                // process argument
                cproc_lex(out, &v, ctx);
                found_arg = true;
                break;
            }
            arg_id++;
        }

        if (!found_arg)
        {
            vec_push(&out->lexemes, *entry);
        }
    }
}

static void cproc_remove_last_non_space_keyword(Lex *lex)
{
    while (vec_last(&lex->lexemes).type == CLEX_WHITESPACE)
    {
        (void)vec_pop(&lex->lexemes);
    }

    (void)vec_pop(&lex->lexemes); // we pop the last token to fusion it with the next one
}

static void cproc_gen_concatenation(Lex *out, Lex *macro_source, CProc *ctx)
{
    Lexeme lhs = {.type = CLEX_NONE};
    Lexeme rhs = {.type = CLEX_NONE};
    bool concatenating = false;

    while (!lex_ended(macro_source))
    {
        if (lex_curr_type(macro_source) != CLEX_WHITESPACE)
        {
            if (lex_curr_type(macro_source) == CLEX_DOUBLE_POUND) // if we detect '##' then the next token should handle it
            {
                concatenating = true;
                lex_next(macro_source);
                continue;
            }
            else if (concatenating)
            {
                if (lhs.type == CLEX_NONE)
                {
                    panic$("## concatenation is not supported");
                    return;
                }

                cproc_remove_last_non_space_keyword(out); // we pop the last token to fusion it with the next one

                rhs = lex_curr(macro_source);
                concatenating = false;
                Lexeme final = {
                    .str = str_concat(lhs.str, rhs.str, ctx->alloc),
                };

                Scan scanner;
                scan_init(&scanner, final.str);
                Lex concatenated = clex(&scanner, ctx->alloc);
                log$(final.str);
                if (concatenated.lexemes.len > 1)
                {
                    panic$("## concatenation can't be done with multiple tokens");
                    return;
                }

                vec_push(&out->lexemes, concatenated.lexemes.data[0]);
                lex_next(macro_source);
                continue;
            }
            else
            {
                lhs = lex_curr(macro_source);
            }
        }
        vec_push(&out->lexemes, lex_curr(macro_source));
        lex_next(macro_source);
    }
}

void cproc_gen_macro(Lex *out, Lex *source, CProc *ctx, CProcMacro *macro)
{
    // first we parse the argument
    MacroArgsCodes macro_args_values = cproc_parse_macro_call_args(macro, source, ctx->alloc);

    // we generate raw macro code
    Lex raw_macro = cproc_gen_macro_code(macro); // raw macro code
    Lex macro_with_argument = {};
    vec_init(&macro_with_argument.lexemes, ctx->alloc);
    cproc_gen_macro_args_replacement(macro, &macro_args_values, &macro_with_argument, &raw_macro, ctx);
    Lex extended_macro = cproc_gen_extended_macro_code(macro, &macro_with_argument, ctx);

    // we apply the concatenation (after argument see 6.10.3)
    cproc_gen_concatenation(out, &extended_macro, ctx);
}

static CProcMacro *cproc_macro_by_name(Str name, CProc *ctx)
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

static bool cproc_is_already_in_macro(CProc *ctx, Str name)
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

void cproc_parse_ident(Lex *out, Lex *source, CProc *ctx)
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

void cproc_parse_directive(MAYBE_UNUSED Lex *result, Lex *source, CProc *context)
{
    lex_skip_type(source, CLEX_WHITESPACE);

    if (lex_curr(source).type == CLEX_NEWLINE)
    {
        return;
    }

    if (lex_curr(source).type != CLEX_IDENT)
    {
        lex_throw(source, str$("expected identifier"));
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

void cproc_lex(Lex *result, Lex *source, CProc *ctx)
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
    CProc ctx = {};
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
