#pragma once

#include <brutal/base.h>

uint8_t br_in8(uint16_t port);

uint16_t br_in16(uint16_t port);

uint32_t br_in32(uint16_t port);

void br_out8(uint16_t port, uint8_t data);

void br_out16(uint16_t port, uint16_t data);

void br_out32(uint16_t port, uint32_t data);
