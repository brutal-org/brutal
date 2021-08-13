#pragma once
#include <handover/handover.h>
#include <efi/base.h>
#include "efi/st.h"
#include <efi/gop.h>

Handover get_handover(EFISystemTable* st);
