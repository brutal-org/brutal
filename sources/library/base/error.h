#pragma once

enum error_kind
{
    ERR_KIND_SUCCESS,
    ERR_KIND_TIMEOUT,
    ERR_KIND_NOT_FOUND,
    ERR_KIND_EXHAUSTION,
    ERR_KIND_INVALID,
    ERR_KIND_UNDEFINED,
};

typedef struct
{
    enum error_kind kind;
    const char *message;
} error_t;

#define ERR_SUCCESS ((error_t){ERR_KIND_SUCCESS, "success"})
#define ERR_TIMEOUT ((error_t){ERR_KIND_TIMEOUT, "timeout"})
#define ERR_NOT_FOUND ((error_t){ERR_KIND_NOT_FOUND, "not-found"})
#define ERR_OUT_OF_MEMORY ((error_t){ERR_KIND_EXHAUSTION, "out-of-memory"})
#define ERR_BAD_ADDRESS ((error_t){ERR_KIND_INVALID, "bad-address"})
#define ERR_UNDEFINED ((error_t){ERR_KIND_UNDEFINED, "undefined"})
