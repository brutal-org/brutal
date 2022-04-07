#include <brutal/io.h>
#include <brutal/math.h>

int main(int, char const *[])
{
    io_print$("{#016x}\n", m_entropy64());
    return 0;
}
