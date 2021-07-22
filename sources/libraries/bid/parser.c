#include <bid/ast.h>
#include <bid/parser.h>

static int is_identifier(int chr)
{
    return chr == '_' || isalnum(chr);
}

// ----------------- parsing utils -------

static bool parser_skip_comment(Scan *scan)
{
    if (!scan_skip_word(scan, str_cast("/*")))
    {
        return false;
    }

    while (!scan_end(scan))
    {
        if (scan_skip_word(scan, str_cast("*/")))
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

// ----------------- type-----------------

static BidParseResult scan_type(struct bid *idl_in, struct bid_ast_node *type_node)
{
    // [type]
    skip_comment_and_space(&idl_in->scan);
    Str name = scan_skip_until((&idl_in->scan), is_identifier);

    if (name.len == 0)
    {
        return ERR(BidParseResult, bid_error(BID_ALNUM_STR, idl_in));
    }

    type_node->ntype.name = name;
    skip_comment_and_space(&idl_in->scan);

    // type[<]
    if (scan_curr(&idl_in->scan) == '<')
    {
        scan_next(&idl_in->scan);

        // type<...>
        while (scan_curr(&idl_in->scan) != '>' && !scan_end(&idl_in->scan))
        {

            auto ast = create_ast_node(BID_AST_NODE_TYPE);
            vec_push(&type_node->children, ast);

            // type<[type]>
            scan_type(idl_in, ast);

            if (scan_curr(&idl_in->scan) == ',')
            {

                // type<type[,]type>
                scan_next(&idl_in->scan);
            }
            else if (scan_curr(&idl_in->scan) != '>')
            {
                // type<type[>]
                return ERR(BidParseResult, bid_error(BID_END_TYPE, idl_in));
            }
        }

        if (scan_end(&idl_in->scan))
        {
            return ERR(BidParseResult, bid_error(BID_GREATER_THAN, idl_in));
        }

        scan_next(&idl_in->scan); // skip '>'
    }

    return BID_SUCCESS;
}

// ----------------- var -----------------

static BidParseResult scan_var(struct bid *idl_in, struct bid_ast_node *arg_node)
{
    // [name] : type

    skip_comment_and_space(&idl_in->scan);

    Str name = scan_skip_until((&idl_in->scan), is_identifier);

    if (name.len == 0)
    {
        return ERR(BidParseResult, bid_error(BID_ALNUM_STR, idl_in));
    }

    arg_node->argument.name = name;

    skip_comment_and_space(&idl_in->scan);

    // name [:] type

    if (scan_curr(&idl_in->scan) != ':')
    {
        return ERR(BidParseResult, bid_error(BID_DOUBLE_DOT, idl_in));
    }

    scan_next(&idl_in->scan);
    skip_comment_and_space(&idl_in->scan);

    // name : [type]

    auto ast = create_ast_node(BID_AST_NODE_TYPE);
    vec_push(&arg_node->children, ast);

    TRY(BidParseResult, scan_type(idl_in, ast));

    return BID_SUCCESS;
}

// ----------------- typedef -----------------

static BidParseResult scan_typedef(struct bid *idl_in, struct bid_ast_node *typedef_node)
{
    // type type1 : type2;

    skip_comment_and_space(&idl_in->scan);

    // type [type1] : type2;

    auto ast = create_ast_node(BID_AST_NODE_TYPE);
    vec_push(&typedef_node->children, ast);
    TRY(BidParseResult, scan_type(idl_in, ast));

    // type type1 [:] type2;

    skip_comment_and_space(&idl_in->scan);

    if (scan_curr(&idl_in->scan) != ':')
    {
        return ERR(BidParseResult, bid_error(BID_DOUBLE_DOT, idl_in));
    }

    scan_next(&idl_in->scan);

    // type type1 : [type2];

    skip_comment_and_space(&idl_in->scan);

    auto ast2 = create_ast_node(BID_AST_NODE_TYPE);
    vec_push(&typedef_node->children, ast2);

    TRY(BidParseResult, scan_type(idl_in, ast2));

    // type type1 : [type2][;]

    skip_comment_and_space(&idl_in->scan);

    if (scan_curr(&idl_in->scan) != ';')
    {
        return ERR(BidParseResult, bid_error(BID_END_LINE, idl_in));
    }

    scan_next(&idl_in->scan);

    return BID_SUCCESS;
}

// ----------------- method -----------------

static BidParseResult scan_method(struct bid *idl_in, struct bid_ast_node *method_node)
{
    // method [name](..)->...;
    skip_comment_and_space(&idl_in->scan);

    Str name = scan_skip_until((&idl_in->scan), is_identifier);

    if (name.len == 0)
    {
        return ERR(BidParseResult, bid_error(BID_ALNUM_STR, idl_in));
    }

    method_node->method.name = name;

    // method name[(]...)->...;

    skip_comment_and_space(&idl_in->scan);

    if (scan_curr(&idl_in->scan) != '(')
    {
        return ERR(BidParseResult, bid_error(BID_OPENNING_PARENTHESIS, idl_in));
    }

    // method name([...])->...;

    scan_next(&idl_in->scan);

    while (scan_curr(&idl_in->scan) != ')' && !scan_end(&idl_in->scan))
    {

        // method name(arg)->...;

        skip_comment_and_space(&idl_in->scan);

        auto ast = create_ast_node(BID_AST_NODE_TYPE_VAR);
        vec_push(&method_node->children, ast);

        TRY(BidParseResult, scan_var(idl_in, ast));

        // method name(arg[,] )->...;

        skip_comment_and_space(&idl_in->scan);

        if (scan_curr(&idl_in->scan) == ',')
        {

            scan_next(&idl_in->scan);
            skip_comment_and_space(&idl_in->scan);
        }

        // method name(arg[)]->...;

        else if (scan_curr(&idl_in->scan) != ')')
        {
            return ERR(BidParseResult, bid_error(BID_END_ARGUMENT, idl_in));
        }
    }

    if (scan_end(&idl_in->scan))
    {

        return ERR(BidParseResult, bid_error(BID_CLOSING_PARENTHESIS, idl_in));
    }

    scan_next(&idl_in->scan);
    skip_comment_and_space(&idl_in->scan);

    // method name()[;]

    if (scan_curr(&idl_in->scan) == ';')
    {
        scan_next(&idl_in->scan); // ;
        return BID_SUCCESS;
    }

    // method name() [->] type;

    if (scan_curr(&idl_in->scan) != '-' || scan_peek(&idl_in->scan, 1) != '>')
    {
        return ERR(BidParseResult, bid_error(BID_ARROW, idl_in));
    }

    scan_next(&idl_in->scan); // skip [-]>
    scan_next(&idl_in->scan); // skip -[>]

    skip_comment_and_space(&idl_in->scan);

    auto ast = create_ast_node(BID_AST_NODE_TYPE_METHOD_RETURN_TYPE);
    vec_push(&method_node->children, ast);

    TRY(BidParseResult, scan_type(idl_in, ast));

    // method name() -> type[;]

    skip_comment_and_space(&idl_in->scan);

    if (scan_curr(&idl_in->scan) != ';')
    {
        return ERR(BidParseResult, bid_error(BID_END_LINE, idl_in));
    }

    scan_next(&idl_in->scan); // ;

    return BID_SUCCESS;
}

// ----------------- error -----------------

static BidParseResult scan_error(struct bid *idl_in, struct bid_ast_node *errors_node)
{
    // errors {
    // err,
    // err2,
    //};

    skip_comment_and_space(&idl_in->scan);

    // errors [{]
    // err,
    // err2,
    //};

    if (scan_curr(&idl_in->scan) != '{')
    {
        return ERR(BidParseResult, bid_error(BID_OPENNING_BRACKETS, idl_in));
    }

    // errors {
    // [err,
    // err2,]
    //};

    scan_next(&idl_in->scan);
    skip_comment_and_space(&idl_in->scan);

    while (scan_curr(&idl_in->scan) != '}' && !scan_end(&idl_in->scan))
    {
        auto ast = create_ast_node(BID_AST_NODE_TYPE_ERROR_MEMBER);
        vec_push(&errors_node->children, ast);

        // [name],

        Str name = scan_skip_until((&idl_in->scan), is_identifier);

        if (name.len == 0)
        {
            return ERR(BidParseResult, bid_error(BID_ALNUM_STR, idl_in));
        }

        ast->errors.name = name;

        skip_comment_and_space(&idl_in->scan);

        if (scan_curr(&idl_in->scan) == ',')
        {
            // name[,]
            scan_next(&idl_in->scan);
            skip_comment_and_space(&idl_in->scan);
        }
        else if (scan_curr(&idl_in->scan) != '}')
        {
            // name[}]
            return ERR(BidParseResult, bid_error(BID_MEMBERS_BLOCK, idl_in));
        }

        skip_comment_and_space(&idl_in->scan);
    }

    if (scan_end(&idl_in->scan))
    {
        return ERR(BidParseResult, bid_error(BID_CLOSING_BRACKETS, idl_in));
    }

    // name[}];

    scan_next(&idl_in->scan); // '}'

    // name}[;]

    if (scan_curr(&idl_in->scan) == ';')
    {
        scan_next(&idl_in->scan); // ';'
        return BID_SUCCESS;
    }

    return ERR(BidParseResult, bid_error(BID_END_LINE, idl_in));
}

// ----------------- interface -----------------

static BidParseResult scan_interface_block(struct bid *idl_in, struct bid_ast_node *interface_node)
{
    // interface name [{]}
    if (scan_curr(&idl_in->scan) != '{')
    {
        return ERR(BidParseResult, bid_error(BID_OPENNING_BRACKETS, idl_in));
    }

    scan_next(&idl_in->scan);
    skip_comment_and_space(&idl_in->scan);

    // interface name  { [...] }
    while (scan_curr(&idl_in->scan) != '}' && !scan_end(&idl_in->scan))
    {
        Str result = scan_skip_until((&idl_in->scan), is_identifier);

        if (str_eq(result, str_cast("method")))
        {
            auto ast = create_ast_node(BID_AST_NODE_TYPE_METHOD);
            vec_push(&interface_node->children, ast);

            TRY(BidParseResult, scan_method(idl_in, ast));
        }
        else if (str_eq(result, str_cast("errors")))
        {
            auto ast = create_ast_node(BID_AST_NODE_TYPE_ERROR);
            vec_push(&interface_node->children, ast);

            TRY(BidParseResult, scan_error(idl_in, ast));
        }
        else if (str_eq(result, str_cast("type")))
        {
            auto ast = create_ast_node(BID_AST_NODE_TYPE_TYPEDEF);
            vec_push(&interface_node->children, ast);

            TRY(BidParseResult, scan_typedef(idl_in, ast));
        }
        else
        {
            return ERR(BidParseResult, bid_error(str_cast("method/struct/enum/type"), idl_in));
        }

        skip_comment_and_space(&idl_in->scan);
    }

    // interface name  { [}]

    if (scan_curr(&idl_in->scan) != '}')
    {
        return ERR(BidParseResult, bid_error(BID_CLOSING_BRACKETS, idl_in));
    }

    skip_comment_and_space(&idl_in->scan);

    return BID_SUCCESS;
}

BidParseResult bid_parse(struct bid *idl_in)
{

    skip_comment_and_space(&idl_in->scan);

    Str result = scan_skip_until((&idl_in->scan), is_identifier);

    // [interface] name { }
    if (str_eq(result, str_cast("interface")))
    {
        // interface [name] { }
        auto ast = create_ast_node(BID_AST_NODE_TYPE_INTERFACE);
        idl_in->root_ast = ast; // for the moment support 1 interface per file

        skip_comment_and_space(&idl_in->scan);

        Str interface_name = scan_skip_until((&idl_in->scan), is_identifier);

        if (interface_name.len == 0)
        {
            return ERR(BidParseResult, bid_error(BID_ALNUM_STR, idl_in));
        }

        ast->interface.name = interface_name;

        skip_comment_and_space(&idl_in->scan);
        TRY(BidParseResult, scan_interface_block(idl_in, ast));

        return BID_SUCCESS;
    }
    else
    {
        return ERR(BidParseResult, bid_error(BID_INTERFACE_STR, idl_in));
    }

    return BID_SUCCESS;
}
