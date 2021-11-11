#pragma once

#include <bal/abi.h>
#include <brutal/ds.h>

typedef struct
{
    void *buf;
    size_t len;
    size_t curr;
    BrMemObj obj;
} BalPack;

typedef void BalPackFn(BalPack *self, void *buf);

void bal_pack_init(BalPack *self);

void bal_pack_deinit(BalPack *self);

void bal_pack_ensure(BalPack *self, size_t cap);

void bal_pack(BalPack *self, void const *buf, size_t len);

void bal_pack_enum(BalPack *self, const int *s);

void bal_pack_s8(BalPack *self, const int8_t *s);

void bal_pack_u8(BalPack *self, const uint8_t *u);

void bal_pack_s16(BalPack *self, const int16_t *s);

void bal_pack_u16(BalPack *self, const uint16_t *u);

void bal_pack_s32(BalPack *self, const int32_t *s);

void bal_pack_u32(BalPack *self, const uint32_t *u);

void bal_pack_s64(BalPack *self, const int64_t *s);

void bal_pack_u64(BalPack *self, const uint64_t *u);

void bal_pack_f32(BalPack *self, const float *f);

void bal_pack_f64(BalPack *self, const double *f);

void bal_pack_str(BalPack *self, const Str *str);

void bal_pack_vec_impl(BalPack *self, VecImpl *iml, BalPackFn *el);
