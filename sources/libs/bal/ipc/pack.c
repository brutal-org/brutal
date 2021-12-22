#include <bal/ipc/pack.h>
#include <brutal/debug.h>
#include <brutal/mem.h>

void bal_pack_init(BalPack *self)
{
    *self = (BalPack){};
}

void bal_pack_deinit(BalPack *self)
{
    assert_br_success(bal_unmap(BR_SPACE_SELF, self->buf, self->len));
    assert_br_success(bal_close(self->obj));
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

    assert_br_success(br_create(&memobj));

    BrMapArgs memmap = {
        .space = BR_SPACE_SELF,
        .mem_obj = memobj.handle,
        .flags = BR_MEM_READABLE | BR_MEM_WRITABLE,
    };

    assert_br_success(br_map(&memmap));

    if (self->buf != nullptr)
    {
        mem_cpy((void *)memmap.vaddr, self->buf, self->curr);
        bal_unmap(BR_SPACE_SELF, self->buf, self->len);
        assert_br_success(bal_close(self->obj));
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

void bal_pack_enum(BalPack *self, int const *v)
{
    bal_pack(self, v, sizeof(*v));
}

void bal_pack_size(BalPack *self, size_t const *s)
{
    bal_pack(self, s, sizeof(*s));
}

void bal_pack_s8(BalPack *self, int8_t const *v)
{
    bal_pack(self, v, sizeof(*v));
}

void bal_pack_u8(BalPack *self, uint8_t const *v)
{
    bal_pack(self, v, sizeof(*v));
}

void bal_pack_s16(BalPack *self, int16_t const *v)
{
    bal_pack(self, v, sizeof(*v));
}

void bal_pack_u16(BalPack *self, uint16_t const *v)
{
    bal_pack(self, v, sizeof(*v));
}

void bal_pack_s32(BalPack *self, int32_t const *v)
{
    bal_pack(self, v, sizeof(*v));
}

void bal_pack_u32(BalPack *self, uint32_t const *v)
{
    bal_pack(self, v, sizeof(*v));
}

void bal_pack_s64(BalPack *self, int64_t const *v)
{
    bal_pack(self, v, sizeof(*v));
}

void bal_pack_u64(BalPack *self, uint64_t const *v)
{
    bal_pack(self, v, sizeof(*v));
}

void bal_pack_f32(BalPack *self, float const *v)
{
    bal_pack(self, v, sizeof(*v));
}

void bal_pack_f64(BalPack *self, double const *v)
{
    bal_pack(self, v, sizeof(*v));
}

void bal_pack_str(BalPack *self, Str const *v)
{
    bal_pack_size(self, &v->len);
    bal_pack(self, v->buf, v->len);
}

void bal_pack_slice_impl(BalPack *self, SliceImpl const *v, BalPackFn *el)
{
    bal_pack_size(self, &v->len);

    for (size_t i = 0; i < v->len; i++)
    {
        el(self, v->buf + v->size * i);
    }
}
