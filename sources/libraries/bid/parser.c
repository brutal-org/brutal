#include <bid/ast.h>
#include <bid/parser.h>

// ----------------- parsing utils -------

int bid_is_keyword(int chr)
{
    if (isalnum(chr))
    {
        return true;
    }
    return (chr == '_');
}

static bool skip_comment(struct bid *idl_in)
{

    char current = scan_curr(&idl_in->scanner);

    if (current != '/' || scan_peek(&idl_in->scanner, 1) != '*')
    {
        return false;
    }

    while (!scan_end(&idl_in->scanner))
    {
        char current = scan_curr(&idl_in->scanner);

        if (current == '*' && scan_peek(&idl_in->scanner, 1) == '/')
        {
            scan_next(&idl_in->scanner); // skip the '*'
            scan_next(&idl_in->scanner); // skip the '/'
            return true;
        }

        scan_next(&idl_in->scanner);
    }

    if (scan_end(&idl_in->scanner))
    {
        return false;
    }

    return false; // clang warns fix (unreachable code)
}

void skip_comment_and_space(struct bid *idl_in)
{
    // loop it until we don't hit a comment or a space

    while ((skip_space(&idl_in->scanner).len != 0 || skip_comment(idl_in)))
    {
        if (scan_end(&idl_in->scanner))
        {
            return;
        }
    }
}

// ----------------- type-----------------

BidParseResult scan_type(struct bid *idl_in, struct bid_ast_node *type_node)
{
    // [type]

    skip_comment_and_space(idl_in);
    Str name = scan_skip_until((&idl_in->scanner), bid_is_keyword);

    if (name.len == 0)
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_ALNUM_STR, idl_in));
    }

    type_node->ntype.name = name;
    skip_comment_and_space(idl_in);

    // type[<]

    if (scan_curr(&idl_in->scanner) == '<')
    {
        scan_next(&idl_in->scanner);

        // type<...>

        while (scan_curr(&idl_in->scanner) != '>' && !scan_end(&idl_in->scanner))
        {

            auto ast = create_ast_node(BID_AST_NODE_TYPE);
            vec_push(&type_node->children, ast);

            // type<[type]>

            scan_type(idl_in, ast);

            // type<type[,]type>

            if (scan_curr(&idl_in->scanner) == ',')
            {

                scan_next(&idl_in->scanner);
            }

            // type<type[>]
            else if (scan_curr(&idl_in->scanner) != '>')
            {

                return ERR(BidParseResult, bid_create_unexpected_token_error(BID_END_TYPE, idl_in));
            }
        }

        if (scan_end(&idl_in->scanner))
        {
            return ERR(BidParseResult, bid_create_unexpected_token_error(BID_GREATER_THAN, idl_in));
        }

        scan_next(&idl_in->scanner); // skip '>'
    }

    return BID_SUCCESS;
}

// ----------------- var -----------------

BidParseResult scan_var(struct bid *idl_in, struct bid_ast_node *arg_node)
{
    // [name] : type

    skip_comment_and_space(idl_in);

    Str name = scan_skip_until((&idl_in->scanner), bid_is_keyword);

    if (name.len == 0)
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_ALNUM_STR, idl_in));
    }

    arg_node->argument.name = name;

    skip_comment_and_space(idl_in);

    // name [:] type

    if (scan_curr(&idl_in->scanner) != ':')
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_DOUBLE_DOT, idl_in));
    }

    scan_next(&idl_in->scanner);
    skip_comment_and_space(idl_in);

    // name : [type]

    auto ast = create_ast_node(BID_AST_NODE_TYPE);
    vec_push(&arg_node->children, ast);

    TRY(BidParseResult, scan_type(idl_in, ast));

    return BID_SUCCESS;
}

// ----------------- typedef -----------------

BidParseResult scan_typedef(struct bid *idl_in, struct bid_ast_node *typedef_node)
{
    // type type1 : type2;

    skip_comment_and_space(idl_in);

    // type [type1] : type2;

    auto ast = create_ast_node(BID_AST_NODE_TYPE);
    vec_push(&typedef_node->children, ast);
    TRY(BidParseResult, scan_type(idl_in, ast));

    // type type1 [:] type2;

    skip_comment_and_space(idl_in);

    if (scan_curr(&idl_in->scanner) != ':')
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_DOUBLE_DOT, idl_in));
    }

    scan_next(&idl_in->scanner);

    // type type1 : [type2];

    skip_comment_and_space(idl_in);

    auto ast2 = create_ast_node(BID_AST_NODE_TYPE);
    vec_push(&typedef_node->children, ast2);

    TRY(BidParseResult, scan_type(idl_in, ast2));

    // type type1 : [type2][;]

    skip_comment_and_space(idl_in);

    if (scan_curr(&idl_in->scanner) != ';')
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_END_LINE, idl_in));
    }

    scan_next(&idl_in->scanner);

    return BID_SUCCESS;
}

// ----------------- method -----------------

BidParseResult scan_method(struct bid *idl_in, struct bid_ast_node *method_node)
{
    // method [name](..)->...;
    skip_comment_and_space(idl_in);

    Str name = scan_skip_until((&idl_in->scanner), bid_is_keyword);

    if (name.len == 0)
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_ALNUM_STR, idl_in));
    }

    method_node->method.name = name;

    // method name[(]...)->...;

    skip_comment_and_space(idl_in);

    if (scan_curr(&idl_in->scanner) != '(')
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_OPENNING_PARENTHESIS, idl_in));
    }

    // method name([...])->...;

    scan_next(&idl_in->scanner);

    while (scan_curr(&idl_in->scanner) != ')' && !scan_end(&idl_in->scanner))
    {

        // method name(arg)->...;

        skip_comment_and_space(idl_in);

        auto ast = create_ast_node(BID_AST_NODE_TYPE_VAR);
        vec_push(&method_node->children, ast);

        TRY(BidParseResult, scan_var(idl_in, ast));

        // method name(arg[,] )->...;

        skip_comment_and_space(idl_in);

        if (scan_curr(&idl_in->scanner) == ',')
        {

            scan_next(&idl_in->scanner);
            skip_comment_and_space(idl_in);
        }

        // method name(arg[)]->...;

        else if (scan_curr(&idl_in->scanner) != ')')
        {

            return ERR(BidParseResult, bid_create_unexpected_token_error(BID_END_ARGUMENT, idl_in));
        }
    }

    if (scan_end(&idl_in->scanner))
    {

        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_CLOSING_PARENTHESIS, idl_in));
    }

    scan_next(&idl_in->scanner);
    skip_comment_and_space(idl_in);

    // method name()[;]

    if (scan_curr(&idl_in->scanner) == ';')
    {
        scan_next(&idl_in->scanner); // ;
        return BID_SUCCESS;
    }

    // method name() [->] type;

    if (scan_curr(&idl_in->scanner) != '-' || scan_peek(&idl_in->scanner, 1) != '>')
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_ARROW, idl_in));
    }

    scan_next(&idl_in->scanner); // skip [-]>
    scan_next(&idl_in->scanner); // skip -[>]

    skip_comment_and_space(idl_in);

    auto ast = create_ast_node(BID_AST_NODE_TYPE_METHOD_RETURN_TYPE);
    vec_push(&method_node->children, ast);

    TRY(BidParseResult, scan_type(idl_in, ast));

    // method name() -> type[;]

    skip_comment_and_space(idl_in);

    if (scan_curr(&idl_in->scanner) != ';')
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_END_LINE, idl_in));
    }

    scan_next(&idl_in->scanner); // ;

    return BID_SUCCESS;
}

// ----------------- error -----------------

BidParseResult scan_error(struct bid *idl_in, struct bid_ast_node *errors_node)
{
    // errors {
    // err,
    // err2,
    //};

    skip_comment_and_space(idl_in);

    // errors [{]
    // err,
    // err2,
    //};

    if (scan_curr(&idl_in->scanner) != '{')
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_OPENNING_BRACKETS, idl_in));
    }

    // errors {
    // [err,
    // err2,]
    //};

    scan_next(&idl_in->scanner);
    skip_comment_and_space(idl_in);

    while (scan_curr(&idl_in->scanner) != '}' && !scan_end(&idl_in->scanner))
    {
        skip_comment_and_space(idl_in);
        auto ast = create_ast_node(BID_AST_NODE_TYPE_ERROR_MEMBER);
        vec_push(&errors_node->children, ast);

        // [name],

        Str name = scan_skip_until((&idl_in->scanner), bid_is_keyword);

        if (name.len == 0)
        {
            return ERR(BidParseResult, bid_create_unexpected_token_error(BID_ALNUM_STR, idl_in));
        }

        ast->errors.name = name;

        skip_comment_and_space(idl_in);

        // name[,]

        if (scan_curr(&idl_in->scanner) == ',')
        {
            scan_next(&idl_in->scanner);
            skip_comment_and_space(idl_in);
        }

        // name[}]

        else if (scan_curr(&idl_in->scanner) != '}')
        {
            return ERR(BidParseResult, bid_create_unexpected_token_error(BID_MEMBERS_BLOCK, idl_in));
        }
    }

    skip_comment_and_space(idl_in);

    if (scan_end(&idl_in->scanner))
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_CLOSING_BRACKETS, idl_in));
    }

    // name[}];

    scan_next(&idl_in->scanner); // '}'

    // name}[;]

    if (scan_curr(&idl_in->scanner) == ';')
    {
        scan_next(&idl_in->scanner); // ';'
        return BID_SUCCESS;
    }

    return ERR(BidParseResult, bid_create_unexpected_token_error(BID_END_LINE, idl_in));
}

// ----------------- interface -----------------

BidParseResult scan_interface_block(struct bid *idl_in, struct bid_ast_node *interface_node)
{
    // interface name [{]}

    if (scan_curr(&idl_in->scanner) != '{')
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_OPENNING_BRACKETS, idl_in));
    }

    scan_next(&idl_in->scanner);
    skip_comment_and_space(idl_in);

    // interface name  { [...] }

    while (scan_curr(&idl_in->scanner) != '}' && !scan_end(&idl_in->scanner))
    {

        skip_comment_and_space(idl_in);

        Str result = scan_skip_until((&idl_in->scanner), bid_is_keyword);

        if (result.len == 0 && scan_curr(&idl_in->scanner) == '}')
        {
            break;
        }
        else if (result.len == 0 && scan_curr(&idl_in->scanner) != '}')
        {
            return ERR(BidParseResult, bid_create_unexpected_token_error(BID_ALNUM_STR, idl_in));
        }
        else if (str_eq(result, str_cast("method")))
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
            return ERR(BidParseResult, bid_create_unexpected_token_error(str_cast("method/struct/enum/type"), idl_in));
        }
    }

    // interface name  { [}]

    if (scan_curr(&idl_in->scanner) != '}')
    {

        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_CLOSING_BRACKETS, idl_in));
    }

    skip_comment_and_space(idl_in);

    return BID_SUCCESS;
}

BidParseResult scan_interface_definition(struct bid *idl_in)
{

    skip_comment_and_space(idl_in);

    Str result = scan_skip_until((&idl_in->scanner), bid_is_keyword);

    // [interface] name { }

    if (str_eq(result, str_cast("interface")))
    {
        // interface [name] { }
        auto ast = create_ast_node(BID_AST_NODE_TYPE_INTERFACE);
        idl_in->root_ast = ast; // for the moment support 1 interface per file

        skip_comment_and_space(idl_in);

        Str interface_name = scan_skip_until((&idl_in->scanner), bid_is_keyword);

        if (interface_name.len == 0)
        {
            return ERR(BidParseResult, bid_create_unexpected_token_error(BID_ALNUM_STR, idl_in));
        }

        ast->interface.name = interface_name;

        skip_comment_and_space(idl_in);
        TRY(BidParseResult, scan_interface_block(idl_in, ast));

        return BID_SUCCESS;
    }
    else
    {
        return ERR(BidParseResult, bid_create_unexpected_token_error(BID_INTERFACE_STR, idl_in));
    }

    return BID_SUCCESS;
}
