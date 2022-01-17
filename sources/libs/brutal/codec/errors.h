#pragma once

#define ERR_INVALID_DATA make_error(ERR_KIND_INVALID_DATA, "invalid-data")
#define ERR_CHECKSUM_MISMATCH make_error(ERR_KIND_INVALID_DATA, "checksum-mismatch")
