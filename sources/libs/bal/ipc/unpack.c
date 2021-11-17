#include <bal/ipc/unpack.h>

void bal_unpack_init(BalUnpack *self, void *buf, size_t len, Alloc *alloc)
{
    self->buf = buf;
    self->len = len;
    self->curr = 0;
    self->alloc = alloc;
}

void bal_unpack(BalUnpack *self, void *buf, size_t len)
{
    mem_cpy(buf, self->buf + self->curr, len);
    self->curr += len;
}

void bal_unpack_enum(BalUnpack *self, int *v)
{
    bal_unpack(self, v, sizeof(*v));
}

void bal_unpack_s8(BalUnpack *self, int8_t *v)
{
    bal_unpack(self, v, sizeof(*v));
}

void bal_unpack_u8(BalUnpack *self, uint8_t *v)
{
    bal_unpack(self, v, sizeof(*v));
}

void bal_unpack_s16(BalUnpack *self, int16_t *v)
{
    bal_unpack(self, v, sizeof(*v));
}

void bal_unpack_u16(BalUnpack *self, uint16_t *v)
{
    bal_unpack(self, v, sizeof(*v));
}

void bal_unpack_s32(BalUnpack *self, int32_t *v)
{
    bal_unpack(self, v, sizeof(*v));
}

void bal_unpack_u32(BalUnpack *self, uint32_t *v)
{
    bal_unpack(self, v, sizeof(*v));
}

void bal_unpack_s64(BalUnpack *self, int64_t *v)
{
    bal_unpack(self, v, sizeof(*v));
}

void bal_unpack_u64(BalUnpack *self, uint64_t *v)
{
    bal_unpack(self, v, sizeof(*v));
}

void bal_unpack_f32(BalUnpack *self, float *v)
{
    bal_unpack(self, v, sizeof(*v));
}

void bal_unpack_f64(BalUnpack *self, double *v)
{
    bal_unpack(self, v, sizeof(*v));
}

void bal_unpack_str(BalUnpack *self, Str *str)
{
    uint64_t len;
    bal_unpack_u64(self, &len);
    char *buf = alloc_make_array(self->alloc, char, len);
    bal_unpack(self, buf, len);
    *str = str_n$(len, buf);
}

void bal_unpack_slice_impl(BalUnpack *self, VoidSlice *slice, BalUnpackFn *el, size_t el_size)
{
    uint64_t len;
    bal_unpack_u64(self, &len);
    void *buf = alloc_calloc(self->alloc, el_size, len);

    for (uint64_t i = 0; i < len; i++)
    {
        el(self, buf + el_size * i);
    }

    slice->len = len;
    slice->buf = buf;
}
