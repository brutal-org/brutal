#pragma once

#include <bal/boot.h>
#include <efi/protos.h>
#include "loader/config.h"

Handover get_handover(const LoaderEntry *entry);
