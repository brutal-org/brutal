#include <brutal/bid/bid.h>
#include "brutal/log/assert.h"
#include "test/test.h"

TEST(bid_creation_destruction)
{
    auto bid = UNWRAP(init_bid(str_cast(" interface test { }")));

    destroy_bid(&bid);
};

TEST(bid_creation_fail)
{
    auto bid_result = (init_bid(str_cast(" interface test {")));

    assert_falsity(bid_result.success);

    assert_equal(bid_result._error.type, (enum bid_error_type)BID_ERROR_TYPE_UNEXPECTED_TOKEN);
};

TEST(bid_comment)
{
    auto bid_result = (init_bid(str_cast(" /* uwu */ interface test { /* another comment */ } /* another one */")));

    assert_truth(bid_result.success);

    destroy_bid(&bid_result._ok);
};

TEST(bid_unexpected_token_interface_name)
{
    auto bid_result = (init_bid(str_cast(" interface @ {")));

    assert_falsity(bid_result.success);

    assert_equal(bid_result._error.type, (enum bid_error_type)BID_ERROR_TYPE_UNEXPECTED_TOKEN);
    assert_str_equal(bid_result._error.specific_information.expected_token.name, BID_ALNUM_STR);
};

TEST(bid_unexpected_token_closing_brackets)
{
    auto bid_result = (init_bid(str_cast(" interface test { ")));

    assert_falsity(bid_result.success);

    assert_equal(bid_result._error.type, (enum bid_error_type)BID_ERROR_TYPE_UNEXPECTED_TOKEN);
    assert_str_equal(bid_result._error.specific_information.expected_token.name, BID_CLOSING_BRACKETS);
};

TEST(bid_unexpected_token_openning_brackets)
{
    auto bid_result = (init_bid(str_cast(" interface test  }")));

    assert_falsity(bid_result.success);

    assert_equal(bid_result._error.type, (enum bid_error_type)BID_ERROR_TYPE_UNEXPECTED_TOKEN);
    assert_str_equal(bid_result._error.specific_information.expected_token.name, BID_OPENNING_BRACKETS);
};

TEST(bid_unexpected_token_interface_str)
{
    auto bid_result = (init_bid(str_cast(" uwuterface test { }")));

    assert_falsity(bid_result.success);

    assert_equal(bid_result._error.type, (enum bid_error_type)BID_ERROR_TYPE_UNEXPECTED_TOKEN);
    assert_str_equal(bid_result._error.specific_information.expected_token.name, BID_INTERFACE_STR);
};
