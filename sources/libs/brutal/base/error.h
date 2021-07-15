#pragma once

#include <brutal/base/result.h>
#include <brutal/text/str.h>

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
    Str message;
} Error;

#define make_error(KIND, MESSAGE) ((Error){(KIND), str_cast(MESSAGE)})

#define ERR_SUCCESS make_error(ERR_KIND_SUCCESS, "success")
#define ERR_TIMEOUT make_error(ERR_KIND_TIMEOUT, "timeout")
#define ERR_NOT_FOUND make_error(ERR_KIND_NOT_FOUND, "not-found")
#define ERR_OUT_OF_MEMORY make_error(ERR_KIND_EXHAUSTION, "out-of-memory")
#define ERR_BAD_ADDRESS make_error(ERR_KIND_INVALID, "bad-address")
#define ERR_UNDEFINED make_error(ERR_KIND_UNDEFINED, "undefined")

typedef struct
{
} Success;

#define SUCCESS OK(MaybeError, (Success){})

typedef Result(Error, Success) MaybeError;
