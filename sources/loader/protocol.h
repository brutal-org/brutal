#pragma once

#include <bal/boot.h>
#include <efi/protos.h>
#include "loader/config.h"

void efi_populate_handover(LoaderEntry const *entry, Handover *ho);
