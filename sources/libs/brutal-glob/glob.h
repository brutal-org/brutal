#pragma once

#include <brutal-parse>

/**
    @brief Match a stream of charaters using glob patterns

    This is a really simple glob pattern implementation
    supporting the following operators:
        - ?        : match only one ch
        - *        : match 0 or more character
        - [xzX-Z]  : matche x, z, or any characters between X and Z
        - [^xzX-Z] : matche anything except x, z, or any characters between X and Z
        - \x       : match x

    @param pattern used to match the input against
    @param input matched with the pattern
    @return true if the input match the pattern
    @return false if the doesn't match the pattern
*/
bool glob_match(Scan *pattern, Scan *input);

/**
    @brief Match a string using a glob pattern

    This function is just a wrapper around glob_match()
    to reduce boilerplate

    @param pattern used to match the input against
    @param str matched with the pattern
    @return true
    @return false
*/
bool glob_match_str(Str pattern, Str str);
