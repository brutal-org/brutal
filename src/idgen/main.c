#include <brutal-fmt>
#include <brutal-math>

int main(int, char const *[])
{
    fmt_print$("{#016x}\n", m_entropy64());
    return 0;
}
