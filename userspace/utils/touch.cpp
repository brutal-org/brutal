#include <libio/File.h>

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        IO::File file{argv[i], HJ_OPEN_CREATE};
    }

    return PROCESS_SUCCESS;
}
