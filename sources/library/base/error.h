#pragma once

enum error_kind
{
    ERR_KIND_SUCCESS,
    ERR_KIND_TIMEOUT,
    ERR_KIND_NOT_FOUND,
};

typedef struct
{
    enum error_kind kind;
    const char *message;
} error_t;

#define ERR_SUCCESS ((error_t){ERR_KIND_SUCCESS, "success"})
#define ERR_TIMEOUT ((error_t){ERR_KIND_TIMEOUT, "timeout"})
#define ERR_NOT_FOUND ((error_t){ERR_KIND_NOT_FOUND, "not-found"})
