#include <brutal-glob>
#include <brutal-test>

static void expect_match(char const *pattern, char const *input)
{
    test_case$(pattern)
    {
        if (!glob_match_str(str$(pattern), str$(input)))
        {
            test_fail$("String '{}' was expected to match '{}'", input, pattern);
        }
    }
}

static void expect_mismatch(char const *pattern, char const *input)
{
    test_case$(pattern)
    {
        if (glob_match_str(str$(pattern), str$(input)))
        {
            test_fail$("String '{}' was expected to mismatch '{}'", input, pattern);
        }
    }
}

test$(text_glob_match)
{
    expect_match("", "");
    expect_match("a", "a");
    expect_mismatch("a", "");
    expect_mismatch("", "a");

    expect_match("abcABC123", "abcABC123");
    expect_mismatch("ABCabc123", "abcABC123");

    expect_match("?", "a");
    expect_match("?", "b");
    expect_match("?", "1");
    expect_match("?", "8");
    expect_mismatch("?", "");

    expect_match("abcABC*", "abcABC123");
    expect_match("abc*123", "abcABC123");
    expect_match("abc*123", "abc123");
    expect_match("*ABC123", "abcABC123");

    expect_mismatch("abcABC*", "ABCabc123");
    expect_mismatch("abc*123", "ABCabc123");
    expect_mismatch("abc*123", "abcABCXYZ");
    expect_mismatch("*ABC123", "ABCabc123");

    expect_match("[abc]", "a");
    expect_match("[abc]", "b");
    expect_match("[abc]", "c");
    expect_mismatch("[abc]", "1");
    expect_mismatch("[abc]", "2");
    expect_mismatch("[abc]", "3");

    expect_match("[a-z]", "a");
    expect_match("[a-z]", "z");
    expect_mismatch("[a-z]", "1");
    expect_mismatch("[a-z]", "9");

    expect_mismatch("[^a-z]", "a");
    expect_mismatch("[^a-z]", "z");
    expect_match("[^a-z]", "1");
    expect_match("[^a-z]", "9");

    expect_match("[a-z0-9]", "a");
    expect_match("[a-z0-9]", "k");
    expect_match("[a-z0-9]", "1");
    expect_match("[a-z0-9]", "7");
    expect_match("[a-z0-9]", "a");
    expect_mismatch("[a-z0-9]", "A");
    expect_mismatch("[a-z0-9]", "K");
}
