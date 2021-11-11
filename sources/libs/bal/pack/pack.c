#include <bal/helpers.h>
#include <bal/pack/pack.h>
#include <brutal/mem.h>

void bal_pack_init(BalPack *self)
{
    *self = (BalPack){};
}

void bal_pack_deinit(BalPack *self)
{
    brh_unmap(BR_SPACE_SELF, self->buf, self->len);
    brh_close(self->obj);
}

void bal_pack_ensure(BalPack *self, size_t cap)
{
    if (self->len > cap)
    {
        return;
    }

    BrCreateArgs memobj = {
        .type = BR_OBJECT_MEMORY,
        .mem_obj = {
            .size = ALIGN_UP(cap, MEM_PAGE_SIZE),
            .flags = BR_MEM_OBJ_NONE,
        },
    };

    br_create(&memobj);

    BrMapArgs memmap = {
        .mem_obj = memobj.handle,
        .flags = BR_MEM_READABLE | BR_MEM_WRITABLE,
    };

    br_map(&memmap);

    if (self->buf != nullptr)
    {
        mem_cpy((void *)memmap.vaddr, self->buf, self->curr);
        brh_unmap(BR_SPACE_SELF, self->buf, self->len);
        brh_close(self->obj);
    }

    self->obj = memobj.handle;
    self->buf = (void *)memmap.vaddr;
    self->len = ALIGN_UP(cap, MEM_PAGE_SIZE);
}

void bal_pack(BalPack *self, void const *buf, size_t len)
{
    bal_pack_ensure(self, self->curr + len);

    mem_cpy(self->buf + self->curr, buf, len);
    self->curr += len;
}

void bal_pack_enum(BalPack *self, const int *v)
{
    bal_pack(self, v, sizeof(*v));
}

void bal_pack_s8(BalPack *self, const int8_t *v)
{
    bal_pack(self, v, sizeof(*v));
}

void bal_pack_u8(BalPack *self, const uint8_t *v)
{
    bal_pack(self, v, sizeof(*v));
}

void bal_pack_s16(BalPack *self, const int16_t *v)
{
    bal_pack(self, v, sizeof(*v));
}

void bal_pack_u16(BalPack *self, const uint16_t *v)
{
    bal_pack(self, v, sizeof(*v));
}

void bal_pack_s32(BalPack *self, const int32_t *v)
{
    bal_pack(self, v, sizeof(*v));
}

void bal_pack_u32(BalPack *self, const uint32_t *v)
{
    bal_pack(self, v, sizeof(*v));
}

void bal_pack_s64(BalPack *self, const int64_t *v)
{
    bal_pack(self, v, sizeof(*v));
}

void bal_pack_u64(BalPack *self, const uint64_t *v)
{
    bal_pack(self, v, sizeof(*v));
}

void bal_pack_f32(BalPack *self, const float *v)
{
    bal_pack(self, v, sizeof(*v));
}

void bal_pack_f64(BalPack *self, const double *v)
{
    bal_pack(self, v, sizeof(*v));
}

void bal_pack_str(BalPack *self, const Str *v)
{
    bal_pack_u64(self, &v->len);
    bal_pack(self, v->buf, v->len);
}

void bal_pack_vec_impl(BalPack *self, VecImpl *v, BalPackFn *el)
{
    bal_pack_enum(self, &v->len);

    for (int i = 0; i < v->len; i++)
    {
        el(self, v->data + v->data_size * i);
    }
}
