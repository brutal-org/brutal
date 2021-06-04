#pragma once

enum error_kind
{
    ERR_SUCCESS,
};

struct error
{
    enum error_kind kind;
    const char *message;
};
