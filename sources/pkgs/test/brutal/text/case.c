#include <brutal/alloc.h>
#include <brutal/text.h>
#include "test/test.h"

static void test_case(Case c, char const *input, char const *expected)
{
    Buf transformed = case_change(c, str$(input), test_alloc());
    assert_str_equal(buf_str(&transformed), str$(expected));
    buf_deinit(&transformed);
}

TEST(change_case)
{
    test_case(CASE_NO, "noCase", "no case");
    test_case(CASE_NO, "no_case", "no case");
    test_case(CASE_NO, "NO_CASE", "no case");
    test_case(CASE_NO, "no/case", "no case");
    test_case(CASE_NO, "no.case", "no case");
    test_case(CASE_NO, "No Case", "no case");

    test_case(CASE_DEFAULT, "Default Case", "Default Case");
    test_case(CASE_DEFAULT, "this is unchanged", "this is unchanged");
    test_case(CASE_DEFAULT, "this.is/unchanged", "this.is/unchanged");

    test_case(CASE_CAMEL, "camel case", "camelCase");
    test_case(CASE_CAPITAL, "capitalCase", "Capital Case");
    test_case(CASE_CONSTANT, "hello world", "HELLO_WORLD");
    test_case(CASE_DOT, "hello world", "hello.world");
    test_case(CASE_HEADER, "HelloWorld", "Hello-World");
    test_case(CASE_PARAM, "hello world", "hello-world");
    test_case(CASE_PASCAL, "hello world", "HelloWorld");
    test_case(CASE_PATH, "hello world", "hello/world");
    test_case(CASE_SENTENCE, "Hello World", "Hello world");
    test_case(CASE_SNAKE, "Hello World", "hello_world");
    test_case(CASE_TITLE, "hello world", "Hello World");
    test_case(CASE_SWAP, "Hello World", "hELLO wORLD");
    test_case(CASE_LOWER, "HELLO WORLD", "hello world");
    test_case(CASE_LOWER_FIRST, "HELLO WORLD", "hELLO WORLD");
    test_case(CASE_UPPER, "helloWorld", "HELLOWORLD");
    test_case(CASE_UPPER, "hello world", "HELLO WORLD");
    test_case(CASE_UPPER_FIRST, "hello world", "Hello world");
    test_case(CASE_SPONGE, "hello world", "HeLlO WoRlD");
    test_case(CASE_SPONGE, "NFTs are the future of the web", "NfTs aRe tHe fUtUrE Of tHe wEb");
}
