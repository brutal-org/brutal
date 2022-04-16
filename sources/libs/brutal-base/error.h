#pragma once

#include "result.h"
#include "str.h"

typedef enum
{
    ERR_KIND_SUCCESS,

    ERR_KIND_TIMEOUT,
    ERR_KIND_NOT_FOUND,
    ERR_KIND_EXHAUSTION,
    ERR_KIND_INVALID_DATA,
    ERR_KIND_INVALID_PARAMS,
    ERR_KIND_PERMISSION,
    ERR_KIND_UNSUPPORTED,
    ERR_KIND_WOULD_BLOCK,
    ERR_KIND_FULL,
    ERR_KIND_EMPTY,

    ERR_KIND_UNDEFINED,
} ErrorKind;

typedef struct
{
    ErrorKind kind;
    Str message;
} Error;

#define make_error(KIND, MESSAGE) ((Error){(KIND), str$(MESSAGE)})

#define ERR_SUCCESS make_error(ERR_KIND_SUCCESS, "success")

#define ERR_BAD_ADDRESS make_error(ERR_KIND_INVALID_PARAMS, "bad-address")
#define ERR_BAD_ARGUMENTS make_error(ERR_KIND_INVALID_PARAMS, "bad-argument")
#define ERR_BAD_HANDLE make_error(ERR_KIND_INVALID_PARAMS, "bad-handle")
#define ERR_BAD_SYSCALL make_error(ERR_KIND_INVALID_PARAMS, "bad-syscall")
#define ERR_DENIED make_error(ERR_KIND_PERMISSION, "denied")
#define ERR_NOT_FOUND make_error(ERR_KIND_NOT_FOUND, "not-found")
#define ERR_NOT_IMPLEMENTED make_error(ERR_KIND_UNSUPPORTED, "not-implemented")
#define ERR_OUT_OF_MEMORY make_error(ERR_KIND_EXHAUSTION, "out-of-memory")
#define ERR_TIMEOUT make_error(ERR_KIND_TIMEOUT, "timeout")
#define ERR_WOULD_BLOCK make_error(ERR_KIND_WOULD_BLOCK, "would-block")
#define ERR_INVALID_DATA make_error(ERR_KIND_INVALID_DATA, "invalid-data")
#define ERR_CHECKSUM_MISMATCH make_error(ERR_KIND_INVALID_DATA, "checksum-mismatch")

#define ERR_UNDEFINED make_error(ERR_KIND_UNDEFINED, "undefined")

typedef struct
{
    char _dummy;
} Success;

#define ERROR(ERROR) ERR(MaybeError, ERROR)

#define SUCCESS OK(MaybeError, (Success){})

typedef Result(Error, Success) MaybeError;
