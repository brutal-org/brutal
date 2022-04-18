#include <brutal-fmt>

int main(int argc, char const *argv[])
{
    Str name = str$("world");

    if (argc > 1)
    {
        name = str$(argv[1]);
    }

    fmt_print$("Hello, {}!\n", name);
    return 0;
}
