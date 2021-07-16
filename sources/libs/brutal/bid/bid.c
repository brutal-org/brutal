#include <brutal/alloc.h>
#include <brutal/base.h>
#include <brutal/bid/bid.h>
#include <brutal/bid/bid_parse.h>
#include <brutal/log.h>
#include <ctype.h>
#include "brutal/base/attributes.h"
#include "brutal/base/types.h"
#include "brutal/io/scan.h"

static BidResult create_bid(Str data_in)
{
    struct bid result =
        {
            .in_data = data_in,
            ._current_scanned_token_cursor = 0,
        };

    scan_init(&result.scanner, data_in);

    return OK(BidResult, result);
}


bool skip_comment(struct bid *idl_in)
{
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
    return true;
}

int is_keyword(int chr)
{
    if (isalnum(chr))
    {
        return true;
    }
    return (chr == '_');
}

static BidParseResult scan_line(struct bid *idl_in, int current_line)
{
    (void)current_line;

    while (!scan_end(&idl_in->scanner))
    {
        log("current char: {}", scan_curr(&idl_in->scanner));
        skip_space(&idl_in->scanner);
        char current = scan_curr(&idl_in->scanner);
        // numbers
        if (isdigit(current))
        {
            long value = scan_next_decimal(&idl_in->scanner);
            (void)value;
        }

        // word
        else if (isalnum(current))
        {
            Str result = scan_skip_until((&idl_in->scanner), is_keyword);
            (void)result;
            continue;
        }

        // arrow
        else if (current == '-' && scan_peek(&idl_in->scanner, 1) == '>') // arrow
        {
            Str result = str_cast("->");
            (void)result;
            scan_next(&idl_in->scanner);
        }

        // comment
        else if (current == '/' && scan_peek(&idl_in->scanner, 1) == '*')
        {
            skip_comment(idl_in);
            continue; // don't add the current token
        }

        // for everything else (: * = + ...)
        else if (ispunct(current))
        {
        }


        if (scan_curr(&idl_in->scanner) == ';')
        {
            scan_next(&idl_in->scanner);
            break;
        }

        scan_next(&idl_in->scanner);
    }

    return OK(BidParseResult, (MonoState){});
}

static BidParseResult scan_bid(struct bid *idl_in)
{

    int line = 0;
    while (!scan_end(&idl_in->scanner))
    {
        TRY(BidParseResult, scan_line(idl_in, line++));
    }

    return OK(BidParseResult, (MonoState){});
}
BidResult init_bid(Str idl_in)
{
    struct bid mbid = TRY(BidResult, create_bid(idl_in));

    TRY(BidResult, scan_bid(&mbid));

    return OK(BidResult, mbid);
}

Str bid_to_c(MAYBE_UNUSED struct bid from)
{
    return str_cast("love u");
}
