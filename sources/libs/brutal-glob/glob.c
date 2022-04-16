#include <brutal-debug>

#include "glob.h"

bool glob_match_group(Scan *pattern, char current)
{
    bool neg = scan_skip(pattern, '^');

    while (!scan_ended(pattern) &&
           !scan_skip(pattern, ']'))
    {
        char range_start = scan_next(pattern);
        char range_end = range_start;

        if (scan_peek(pattern, 0) == '-' && scan_peek(pattern, 1) != ']')
        {
            range_end = scan_peek(pattern, 1);
            scan_next_n(pattern, 2);
        }

        if (range_end < range_start)
        {
            swap$(&range_start, &range_end);
        }

        if (range_start <= current && current <= range_end)
        {
            while (!scan_ended(pattern) &&
                   !scan_skip(pattern, ']'))
            {
                scan_next(pattern);
            }

            return !neg;
        }
    }

    return neg;
}

bool glob_match_wildcard(Scan *pattern, Scan *input)
{
    int input_start = scan_tell(input);
    int pattern_start = scan_tell(pattern);

    scan_seek(input, scan_size(input));

    while (scan_tell(input) >= input_start)
    {
        int input_here = scan_tell(input);

        if (glob_match(pattern, input))
        {
            return true;
        }

        scan_seek(pattern, pattern_start);
        scan_seek(input, input_here - 1);
    }

    return false;
}

bool glob_match(Scan *pattern, Scan *input)
{
    while (!scan_ended(pattern) && !scan_ended(input))
    {
        char operator= scan_next(pattern);

        switch (operator)
        {
        case '?':
            if (scan_ended(input))
            {
                return false;
            }

            scan_next(input);
            break;

        case '[':
            if (!glob_match_group(pattern, scan_next(input)))
            {
                return false;
            }

            break;

        case '*':
            if (!glob_match_wildcard(pattern, input))
            {
                return false;
            }
            break;

        case '\\':
            if (scan_ended(input))
            {
                return false;
            }

            if (operator!= scan_next(input))
            {
                return false;
            }
            break;

        default:
            if (scan_ended(input))
            {
                return false;
            }

            if (operator!= scan_next(input))
            {
                return false;
            }
            break;
        }
    }

    return scan_ended(pattern) && scan_ended(input);
}

bool glob_match_str(Str pattern, Str input)
{
    Scan pattern_scan;
    scan_init(&pattern_scan, pattern);

    Scan input_scan;
    scan_init(&input_scan, input);

    return glob_match(&pattern_scan, &input_scan);
}
