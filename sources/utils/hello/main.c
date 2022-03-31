#include <brutal/io.h>

int main(int argc, char const *argv[])
{
    Str name = str$("world");

    if (argc > 1)
    {
        name = str$(argv[1]);
    }

    io_print$("Hello, {}!\n", name);
    return 0;
}
