#include <brutal-fmt>
#include <brutal-test>

static void TEST_CASE(Case c, char const *input, char const *expected)
{
    test_case$(expected)
    {
        Buf transformed = case_change(c, str$(input), test_use_alloc());
        expect_str_equal$(buf_str(&transformed), str$(expected));
        buf_deinit(&transformed);
    }
}

test$(fmt_case)
{
    TEST_CASE(CASE_NO, "noCase", "no case");
    TEST_CASE(CASE_NO, "no_case", "no case");
    TEST_CASE(CASE_NO, "NO_CASE", "no case");
    TEST_CASE(CASE_NO, "no/case", "no case");
    TEST_CASE(CASE_NO, "no.case", "no case");
    TEST_CASE(CASE_NO, "No Case", "no case");

    TEST_CASE(CASE_DEFAULT, "Default Case", "Default Case");
    TEST_CASE(CASE_DEFAULT, "this is unchanged", "this is unchanged");
    TEST_CASE(CASE_DEFAULT, "this.is/unchanged", "this.is/unchanged");

    TEST_CASE(CASE_CAMEL, "camel case", "camelCase");
    TEST_CASE(CASE_CAPITAL, "capitalCase", "Capital Case");
    TEST_CASE(CASE_CONSTANT, "hello world", "HELLO_WORLD");
    TEST_CASE(CASE_DOT, "hello world", "hello.world");
    TEST_CASE(CASE_HEADER, "HelloWorld", "Hello-World");
    TEST_CASE(CASE_PARAM, "hello world", "hello-world");
    TEST_CASE(CASE_PASCAL, "hello world", "HelloWorld");
    TEST_CASE(CASE_PATH, "hello world", "hello/world");
    TEST_CASE(CASE_SENTENCE, "Hello World", "Hello world");
    TEST_CASE(CASE_SNAKE, "Hello World", "hello_world");
    TEST_CASE(CASE_TITLE, "hello world", "Hello World");
    TEST_CASE(CASE_SWAP, "Hello World", "hELLO wORLD");
    TEST_CASE(CASE_LOWER, "HELLO WORLD", "hello world");
    TEST_CASE(CASE_LOWER_FIRST, "HELLO WORLD", "hELLO WORLD");
    TEST_CASE(CASE_UPPER, "helloWorld", "HELLOWORLD");
    TEST_CASE(CASE_UPPER, "hello world", "HELLO WORLD");
    TEST_CASE(CASE_UPPER_FIRST, "hello world", "Hello world");
    TEST_CASE(CASE_SPONGE, "hello world", "HeLlO WoRlD");
    TEST_CASE(CASE_SPONGE, "NFTs are the future of the web", "NfTs aRe tHe fUtUrE Of tHe wEb");
}
