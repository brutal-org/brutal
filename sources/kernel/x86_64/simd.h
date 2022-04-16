#pragma once

#include <brutal-base>

void simd_initialize(void);

size_t simd_context_size(void);

void simd_context_init(void *ptr);

void simd_context_save(void *ptr);

void simd_context_load(void *ptr);
