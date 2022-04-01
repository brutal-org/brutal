#pragma once

#include <libutils/Prelude.h>

namespace Utils
{

using TypeId = uintptr_t;

template <typename T>
static TypeId GetTypeId()
{
    static uint32_t placeHolder;
    return (reinterpret_cast<TypeId>(&placeHolder));
}

constexpr TypeId INVALID_TYPE_ID = -1;

} // namespace Utils
