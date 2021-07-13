#pragma once
#include <brutal/base.h>
#include <brutal/ds.h>
#include <brutal/io/scan.h>

enum bid_token_type
{
    BID_TOKEN_NULL,
    BID_TOKEN_STR,
    BID_TOKEN_KEYWORD,
    BID_CURLY_START,
    BID_CURLY_END,
    BID_PARENTHESIS_START,
    BID_PARENTHESIS_END,
    BID_SQUARE_BRACKETS_START,
    BID_SQUARE_BRACKETS_END,
    BID_END_LINE,
    BID_COLON,
    BID_SEMICOLON,
    BID_COMMA,
    BID_ARROW,
    BID_COMMENT_START,
    BID_COMMENT_END,
    BID_ASTERIX,
    BID_DIGIT,
};

enum bid_keywords
{
    BID_STRUCT,
    BID_VAR,
    BID_METHOD,
    BID_ENUM,
    BID_INTERFACE,
    BID_INCLUDE,
};

enum bid_error_type
{
    BID_NO_ERROR,
    BID_SCANNER_ERROR,
    BID_TOKENIZATION_ERROR,
    BID_INVALID_TOKEN,
    BID_INVALID_KEYWORD,
    BID_INVALID_INT,
    BID_OTHER_ERROR,
};

struct bid_error
{
    enum bid_error_type type;
    int line;
    int offset;
    Str message;
};

struct bid_token
{
    enum bid_token_type type;
    int line;
    int offset;
    long int_value;
    Str raw_value;
};

struct bid
{
    Str in_data;
    struct scan scanner;
    Vec(struct bid_token) tokens;
};

typedef Result(struct bid_error, struct bid) BidResult;
typedef Result(struct bid_error, MonoState) BidScanResult;

BidResult init_bid(Str idl_in);

Str bid_to_c(struct bid from);
