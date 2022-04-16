#include <brutal-debug>
#include <brutal-fmt>
#include <idl/ast/builder.h>
#include <idl/parse.h>

static bool parser_skip_comment(Scan *scan)
{
    if (!scan_skip_word(scan, str$("/*")))
    {
        return false;
    }

    while (!scan_ended(scan))
    {
        if (scan_skip_word(scan, str$("*/")))
        {
            return true;
        }

        scan_next(scan);
    }

    return false;
}

static void skip_comment_and_space(Scan *scan)
{
    while (scan_eat(scan, isspace) ||
           parser_skip_comment(scan))
        ;
}

static bool skip_keyword(Scan *scan, char const *kw)
{
    skip_comment_and_space(scan);
    bool result = scan_skip_word(scan, str$(kw));
    skip_comment_and_space(scan);

    return result;
}

static bool skip_separator(Scan *scan, char sep)
{
    skip_comment_and_space(scan);
    bool result = scan_skip(scan, sep);
    skip_comment_and_space(scan);

    return result;
}

static bool expect_separator(Scan *scan, char sep)
{
    skip_comment_and_space(scan);
    bool result = scan_expect(scan, sep);
    skip_comment_and_space(scan);

    return result;
}

static int is_ident(int chr)
{
    return chr == '-' || chr == '_' || isalnum(chr);
}

static IdlType idl_parse_type(Scan *scan, Alloc *alloc);

static Str parse_ident(Scan *scan)
{
    Str name = scan_eat_match(scan, is_ident);

    if (str_empty(name))
    {
        scan_throw(scan, str$("invalid ident"), name);
    }

    return name;
}

static IdlType idl_parse_primitive(Scan *scan)
{
    Str ident = parse_ident(scan);

    if (str_eq(ident, str$("Nil")))
    {
        return idl_nil();
    }

    return idl_primitive(ident);
}

static void idl_parse_enum_constant(Scan *scan, IdlType *enum_, int first)
{
    skip_separator(scan, '{');

    while (!skip_separator(scan, '}') && !scan_ended(scan))
    {
        idl_enum_constant(enum_, parse_ident(scan), first++);
        skip_separator(scan, ',');
    }
}

static IdlType idl_parse_enum(Scan *scan, Alloc *alloc)
{
    IdlType type = idl_enum(alloc);
    idl_parse_enum_constant(scan, &type, 0);
    return type;
}

static IdlType idl_parse_struct(Scan *scan, Alloc *alloc)
{
    IdlType type = idl_struct(alloc);

    expect_separator(scan, '{');

    while (!skip_separator(scan, '}') && !scan_ended(scan))
    {
        IdlType member_type = idl_parse_type(scan, alloc);

        skip_comment_and_space(scan);

        Str member_name = parse_ident(scan);

        idl_struct_member(&type, member_name, member_type);

        skip_separator(scan, ',');
    }

    return type;
}

static IdlType idl_parse_vec(IdlType subtype, Scan *scan, Alloc *alloc)
{
    skip_comment_and_space(scan);

    if (!skip_separator(scan, '['))
    {
        return subtype;
    }

    expect_separator(scan, ']');

    return idl_vec(subtype, alloc);
}

static IdlType idl_parse_type(Scan *scan, Alloc *alloc)
{
    IdlType type = idl_nil();

    if (skip_keyword(scan, "struct") || scan_curr(scan) == '{')
    {
        type = idl_parse_struct(scan, alloc);
    }
    else if (skip_keyword(scan, "enum"))
    {
        type = idl_parse_enum(scan, alloc);
    }
    else if (skip_keyword(scan, "__ctype"))
    {
        if (skip_separator(scan, '('))
        {
            type = idl_ctype(parse_ident(scan));
            expect_separator(scan, ')');
        }
        else
        {
            type = idl_ctype(nullstr);
        }
    }
    else if (is_ident(scan_curr(scan)))
    {
        type = idl_parse_primitive(scan);
    }
    else
    {
        scan_throw(scan, str$("Un expected token"), str$(""));
    }

    return idl_parse_vec(type, scan, alloc);
}

static IdlAlias idl_parse_alias(Scan *scan, IdlAttrs attrs, Alloc *alloc)
{
    Str name = parse_ident(scan);

    expect_separator(scan, '=');

    IdlType type = idl_parse_type(scan, alloc);

    if (type.type == IDL_TYPE_CTYPE && str_eq(nullstr, type.ctype_.name))
    {
        type.ctype_.name = name;
    }

    return idl_alias(name, attrs, type);
}

static IdlMethod idl_parse_method_body(Scan *scan, Str name, IdlAttrs attrs, Alloc *alloc)
{
    IdlType request = idl_nil();
    IdlType response = idl_nil();

    if (skip_keyword(scan, "->"))
    {
        response = idl_parse_type(scan, alloc);
    }
    else
    {
        request = idl_parse_type(scan, alloc);

        if (skip_keyword(scan, "->"))
        {
            response = idl_parse_type(scan, alloc);
        }
    }

    return idl_method(name, attrs, request, response);
}

static IdlMethod idl_parse_method(Scan *scan, IdlAttrs attrs, Alloc *alloc)
{
    Str name = parse_ident(scan);
    return idl_parse_method_body(scan, name, attrs, alloc);
}

static IdlAttrs idl_parse_attrs(Scan *scan, Alloc *alloc)
{
    IdlAttrs attrs = idl_attrs(alloc);

    if (skip_separator(scan, '['))
    {
        do
        {
            IdlAttr attr = idl_attr(parse_ident(scan), alloc);

            if (skip_separator(scan, '('))
            {
                do
                {
                    idl_attr_append(&attr, parse_ident(scan));
                } while (!scan_ended(scan) && skip_separator(scan, ','));

                expect_separator(scan, ')');
            }

            idl_attrs_append(&attrs, attr);
        } while (!scan_ended(scan) && skip_separator(scan, ','));

        expect_separator(scan, ']');
    }

    return attrs;
}

static IdlIface idl_parse_iface(Scan *scan, IdlAttrs attrs, Alloc *alloc)
{
    skip_keyword(scan, "interface");

    bool is_methode = skip_keyword(scan, "method");

    Str name = parse_ident(scan);

    Str name_pascal = case_change_str(CASE_PASCAL, name, alloc);
    IdlIface iface = idl_iface(name_pascal, attrs, alloc);

    if (!is_methode)
    {
        expect_separator(scan, '{');

        while (scan_curr(scan) != '}' && !scan_ended(scan))
        {
            IdlAttrs child_attrs = idl_parse_attrs(scan, alloc);

            if (skip_keyword(scan, "method"))
            {
                IdlMethod method = idl_parse_method(scan, child_attrs, alloc);
                idl_iface_methode(&iface, method);
            }
            else
            {
                scan_throw(scan, str$("expected errors/type/method"), parse_ident(scan));
            }

            expect_separator(scan, ';');
        }

        expect_separator(scan, '}');
    }
    else
    {
        idl_parse_method_body(scan, name, attrs, alloc);
    }

    return iface;
}

static int is_path(int chr)
{
    return is_ident(chr) || chr == '/' || chr == '.';
}

Str idl_parse_cinclude(Scan *scan)
{
    return scan_eat_match(scan, is_path);
}

IdlModule idl_parse_module(Scan *scan, Alloc *alloc)
{
    skip_keyword(scan, "module");
    Str name = parse_ident(scan);
    IdlModule module = idl_module(name, alloc);
    expect_separator(scan, ';');

    while (!scan_ended(scan))
    {
        IdlAttrs attrs = idl_parse_attrs(scan, alloc);

        if (skip_keyword(scan, "import"))
        {
            skip_comment_and_space(scan);

            do
            {
                idl_module_import(&module, parse_ident(scan));
            } while (!scan_ended(scan) && skip_separator(scan, ','));
        }
        else if (skip_keyword(scan, "include"))
        {
            skip_comment_and_space(scan);

            do
            {
                idl_module_include(&module, idl_parse_cinclude(scan));
            } while (!scan_ended(scan) && skip_separator(scan, ','));
        }
        else if (skip_keyword(scan, "errors"))
        {
            // Skip zero because it's IPC_SUCCESS
            idl_parse_enum_constant(scan, &module.errors, 1);
        }
        else if (skip_keyword(scan, "type"))
        {
            IdlAlias alias = idl_parse_alias(scan, attrs, alloc);
            idl_module_alias(&module, alias);
        }
        else if (skip_keyword(scan, "interface"))
        {
            IdlIface iface = idl_parse_iface(scan, attrs, alloc);
            idl_module_iface(&module, iface);
        }
        else
        {
            scan_throw(scan, str$("expected import/include/type/interface"), parse_ident(scan));
        }

        expect_separator(scan, ';');
    }

    return module;
}

IoResult idl_import_module(Str name, IdlModule *module, Alloc *alloc)
{
    IoFile source_file;
    TRY(IoResult, io_file_view(&source_file, fmt_str$(alloc, "sources/protos/{}.idl", name)));

    Buf source_buf;
    buf_init(&source_buf, 512, alloc);

    size_t read = TRY(IoResult, io_copy(io_file_reader(&source_file), buf_writer(&source_buf)));

    Scan scan;
    scan_init(&scan, buf_str(&source_buf));
    *module = idl_parse_module(&scan, alloc);

    return OK(IoResult, read);
}
