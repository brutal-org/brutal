#pragma once

#define ERR_UNKNOWN_BLOCK make_error(ERR_KIND_INVALID_DATA, "unknown-blocktype")
#define ERR_TOO_MANY_SYMBOLS make_error(ERR_KIND_INVALID_DATA, "too-many-symbols")
#define ERR_OVER_SUBSCRIBED make_error(ERR_KIND_INVALID_DATA, "over-subscribed")
#define ERR_INCOMPLETE_SET make_error(ERR_KIND_INVALID_DATA, "incomplete-set")
#define ERR_NOT_ENOUGH_TABLE_SPACE make_error(ERR_KIND_INVALID_DATA, "not-enough-table-space")
#define ERR_INVALID_CODE make_error(ERR_KIND_INVALID_DATA, "invalid-code")
#define ERR_BAD_DATA make_error(ERR_KIND_INVALID_DATA, "bad-data")
