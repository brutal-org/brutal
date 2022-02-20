#pragma once

#include <bal/boot.h>
#include <efi/protos.h>
#include "loader/config.h"

void loader_populate_handover(LoaderEntry const *entry, Handover *ho);
