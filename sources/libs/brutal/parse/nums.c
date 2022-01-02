#include <brutal/parse/nums.h>
#include <math.h>

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

bool scan_next_uint(Scan *self, unsigned long *value)
{
    while (!scan_ended(self))
    {
        char v = scan_peek(self, 0);
        if (v >= '0' && v <= '9')
        {
            *value *= 10;
            *value += v - '0';
            scan_next(self);
        }
        else
        {
            break;
        }
    }

    return true;
}

bool scan_next_int(Scan *self, long *value)
{
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
            *value *= 10;
            *value += v - '0';
            scan_next(self);
        }
        else
        {
            break;
        }
    }

    if (is_negative)
    {
        *value *= -1;
    }

    return true;
}

bool scan_next_float(Scan *self, float *value)
{
    long ipart = 0;

    if (!scan_next_int(self, &ipart))
    {
        return false;
    }

    double fpart = 0;

    if (scan_skip(self, '.'))
    {
        double multiplier = (1.0 / 10);

        char v = scan_peek(self, 0);
        while (v >= '0' && v <= '9')
        {
            fpart += multiplier * (v - '0');
            multiplier *= (1.0 / 10);
        }
    }

    long exp = 0;

    if (scan_curr_is_any(self, str$("eE")))
    {
        scan_next(self);
        scan_next_int(self, &exp);
    }

    *value = (ipart + fpart) * pow(10, exp);
    return true;
}

bool str_to_uint(Str string, unsigned long *value)
{
    Scan scan = {0};
    scan_init(&scan, string);
    return scan_next_uint(&scan, value);
}

bool str_to_int(Str string, long *value)
{
    Scan scan = {0};
    scan_init(&scan, string);
    return scan_next_int(&scan, value);
}

bool str_to_float(Str string, float *value)
{
    Scan scan = {0};
    scan_init(&scan, string);
    return scan_next_float(&scan, value);
}
