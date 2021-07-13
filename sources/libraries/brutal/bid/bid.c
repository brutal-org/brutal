#include <brutal/bid/bid.h>
#include <brutal/base.h>
#include <brutal/log.h>
#include <brutal/alloc.h>
#include "brutal/io/scan.h"
static BidResult create_bid(Str data_in)
{
    struct bid result = {
        .in_data = data_in,
    };

    scan_init(&result.scanner, data_in);
    vec_init(&result.tokens, alloc_global());

    return OK(BidResult,result);
}

enum bid_token_type token_from_char(char from)
{
    switch (from)
    {
    case '{':
        return BID_CURLY_START;
    case '}':
        return BID_CURLY_END;
    case '(':
        return BID_PARENTHESIS_START;
    case ')':
        return BID_PARENTHESIS_END;
    case '[':
        return BID_SQUARE_BRACKETS_START;
    case ']':
        return BID_SQUARE_BRACKETS_END;
    case ':':
        return BID_COLON;
    case ';':
        return BID_SEMICOLON;
    case '\n':
        return BID_END_LINE;
    case ',':
        return BID_COMMA;
    case '*':
        return BID_ASTERIX;
    default:
        return BID_TOKEN_NULL;
    }
}

bool skip_comment(struct bid* idl_in)
{
    while(!scan_end(&idl_in->scanner))
    {
        char current = scan_curr(&idl_in->scanner);
        if(current == '*' && scan_peek(&idl_in->scanner, 1) == '/')
        {
            scan_next(&idl_in->scanner); // skip the '*'
            scan_next(&idl_in->scanner); // skip the '/'
            return true;
        }
        scan_next(&idl_in->scanner);
    }
    if(scan_end(&idl_in->scanner))
    {
        return false;
    }
    return true;
}

BidScanResult scan_line(struct bid* idl_in, int current_line)
{

    while(scan_curr(&idl_in->scanner) != ';' && !scan_end(&idl_in->scanner))
    {
        skip_space(&idl_in->scanner);
        struct bid_token token;
        token.line = current_line;
        token.offset = idl_in->scanner.head;
        char current = scan_curr(&idl_in->scanner);
        // numbers
        if(isdigit(current))
        {
            long value = scan_next_decimal(&idl_in->scanner);
            token.type = BID_DIGIT;
            token.int_value = value;
            token.raw_value = str_cast( "number");

        }
        // word
        else if(isalnum(current))
        {
            Str result = scan_alnum(&idl_in->scanner);
            token.type = BID_TOKEN_KEYWORD;
            token.raw_value = result;
        }
        // arrow
        else if(current == '-' && scan_peek(&idl_in->scanner, 1) == '>') // arrow
        {
            Str result = str_cast("->");
            scan_next(&idl_in->scanner);
            token.raw_value = result;
            token.type = BID_ARROW;
        }
        // comment
        else if(current == '/' && scan_peek(&idl_in->scanner, 1) == '*')
        {
            skip_comment(idl_in);
            continue; // don't add the current token
        }
        else if (current == ';')
        {

            continue;
        }
        // for everything else (: * = + ...)
        else if(ispunct(current))
        {
            enum bid_token_type type = token_from_char(current);
            if(type == BID_TOKEN_NULL)
            {
                struct bid_error error;
                error.type = BID_INVALID_TOKEN;
                log("invalid token");
                error.message = str_cast("bid-error: invalid token");
                error.offset =  idl_in->scanner.head;


                return ERR(BidScanResult, error );
            }
            token.type = type;
            token.int_value = current;
            token.raw_value = str_cast("uwu");

        }
        vec_push(&idl_in->tokens, token);

        scan_next(&idl_in->scanner);
    }

    scan_next(&idl_in->scanner);
    return OK(BidScanResult, (MonoState) {});
}

BidScanResult scan_bid(struct bid* idl_in)
{

    int line = 0;
    while(!scan_end(&idl_in->scanner))
    {
        TRY(BidScanResult, scan_line(idl_in, line ++));
    }

    return OK(BidScanResult, (MonoState) {});
}


BidResult init_bid(Str idl_in)
{
    struct bid bid = TRY(BidResult,create_bid(idl_in));

    TRY(BidResult, scan_bid(&bid));

    return OK(BidResult, bid);
}
