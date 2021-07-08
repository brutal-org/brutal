#pragma once

#include <brutal/base/result.h>

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
    char const *message;
} Error;

#define ERR_SUCCESS ((Error){ERR_KIND_SUCCESS, "success"})
#define ERR_TIMEOUT ((Error){ERR_KIND_TIMEOUT, "timeout"})
#define ERR_NOT_FOUND ((Error){ERR_KIND_NOT_FOUND, "not-found"})
#define ERR_OUT_OF_MEMORY ((Error){ERR_KIND_EXHAUSTION, "out-of-memory"})
#define ERR_BAD_ADDRESS ((Error){ERR_KIND_INVALID, "bad-address"})
#define ERR_UNDEFINED ((Error){ERR_KIND_UNDEFINED, "undefined"})

typedef struct
{
} Success;

#define SUCCESS OK(MaybeError, (Success){})

typedef Result(Error, Success) MaybeError;
