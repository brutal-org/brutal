#include <brutal/parse/nums.h>

long scan_next_digit(Scan *self)
{
    char c = tolower(scan_next(self));

    if (isalpha(c))
    {
        return c - 'a' + 10;
    }
    else
    {
        return c - '0';
    }
}

long scan_next_number(Scan *self)
{
    long result = 0;
    bool is_negative = false;
    char sign = scan_peek(self, 0);

    if (sign == '-')
    {
        is_negative = true;
        scan_next(self);
    }

    while (!scan_ended(self))
    {
        char v = scan_peek(self, 0);
        if (v >= '0' && v <= '9')
        {
            result *= 10;
            result += v - '0';
        }
        else
        {
            if (is_negative)
            {
                result *= -1;
            }
            return result;
        }
        scan_next(self);
    }

    if (is_negative)
    {
        result *= -1;
    }

    return result;
}

long str_to_number(Str string)
{
    Scan scan = {0};
    scan_init(&scan, string);
    return scan_next_number(&scan);
}
