#include <bid/bid.h>
#include <bid/parser.h>
#include <brutal/log.h>
#include "tests/test.h"

TEST(bid_creation_destruction)
{
    auto bid = UNWRAP(bid_init(str_cast(" interface test { }")));

    bid_deinit(&bid);
};

TEST(bid_creation_fail)
{
    auto bid_result = (bid_init(str_cast(" interface test {")));

    assert_falsity(bid_result.success);

    assert_equal(bid_result._error.type, (enum bid_error_type)BID_ERROR_TYPE_UNEXPECTED_TOKEN);
};

TEST(bid_comment)
{
    auto bid_result = (bid_init(str_cast(" /* uwu */ interface test { /* another comment */ } /* another one */")));

    assert_truth(bid_result.success);

    bid_deinit(&bid_result._ok);
};

TEST(bid_unexpected_token_interface_name)
{
    auto bid_result = (bid_init(str_cast(" interface @ {")));

    assert_falsity(bid_result.success);

    assert_equal(bid_result._error.type, (enum bid_error_type)BID_ERROR_TYPE_UNEXPECTED_TOKEN);
    assert_str_equal(bid_result._error.specific_information.expected_token.name, BID_ALNUM_STR);
};

TEST(bid_unexpected_token_closing_brackets)
{
    auto bid_result = (bid_init(str_cast(" interface test { ")));

    assert_falsity(bid_result.success);

    assert_equal(bid_result._error.type, (enum bid_error_type)BID_ERROR_TYPE_UNEXPECTED_TOKEN);
    assert_str_equal(bid_result._error.specific_information.expected_token.name, BID_CLOSING_BRACKETS);
};

TEST(bid_unexpected_token_openning_brackets)
{
    auto bid_result = (bid_init(str_cast(" interface test  }")));

    assert_falsity(bid_result.success);

    assert_equal(bid_result._error.type, (enum bid_error_type)BID_ERROR_TYPE_UNEXPECTED_TOKEN);
    assert_str_equal(bid_result._error.specific_information.expected_token.name, BID_OPENNING_BRACKETS);
};

TEST(bid_unexpected_token_interface_str)
{
    auto bid_result = (bid_init(str_cast(" uwuterface test { }")));

    assert_falsity(bid_result.success);

    assert_equal(bid_result._error.type, (enum bid_error_type)BID_ERROR_TYPE_UNEXPECTED_TOKEN);
    assert_str_equal(bid_result._error.specific_information.expected_token.name, BID_INTERFACE_STR);
};
