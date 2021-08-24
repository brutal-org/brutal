#include <brutal/ds/range.h>
#include <brutal/log/locked.h>

void range_alloc_init(RangeAlloc *self, Alloc *alloc)
{
    vec_init(self, alloc);
}

void range_alloc_deinit(RangeAlloc *self)
{
    vec_deinit(self);
}

void range_alloc_used(RangeAlloc *self, USizeRange range)
{
    for (int i = 0; i < self->length; i++)
    {
        auto curr = self->data[i];

        if (range_eq(curr, range))
        {
            vec_splice(self, i, 1);
            break;
        }

        if (range_colide(curr, range))
        {
            auto lower_half = range_half_under(curr, range);
            auto upper_half = range_half_over(curr, range);

            if (lower_half.size != 0)
            {
                self->data[i] = lower_half;

                if (upper_half.size != 0)
                {
                    range_alloc_unused(self, upper_half);
                }
            }
            else if (upper_half.size != 0)
            {
                self->data[i] = upper_half;
            }
        }
    }
}

USizeRange range_alloc_alloc(RangeAlloc *self, size_t size)
{
    for (int i = 0; i < self->length; i++)
    {
        if (self->data[i].size == size)
        {
            auto result = self->data[i];

            vec_splice(self, i, 1);

            return result;
        }

        if (self->data[i].size > size)
        {
            auto result = (USizeRange){self->data[i].base, size};

            self->data[i].base += size;
            self->data[i].size -= size;

            return result;
        }
    }

    return (USizeRange){};
}

static void range_alloc_compress(RangeAlloc *self, int start)
{
    while (start + 1 < self->length &&
           range_contiguous(self->data[start], self->data[start + 1]))
    {
        self->data[start] = range_merge(self->data[start], self->data[start + 1]);
        vec_splice(self, start + 1, 1);
    }

    while (start > 0 &&
           range_contiguous(self->data[start], self->data[start - 1]))
    {
        self->data[start] = range_merge(self->data[start], self->data[start - 1]);
        vec_splice(self, start - 1, 1);
    }
}

void range_alloc_unused(RangeAlloc *self, USizeRange range)
{
    for (int i = 0; i < self->length; i++)
    {
        if (range_contiguous(self->data[i], range))
        {
            self->data[i] = range_merge(self->data[i], range);
            range_alloc_compress(self, i);
            return;
        }

        if (i + 1 < self->length &&
            (self->data[i].base < range.base) &&
            (range.base < self->data[i + 1].base))
        {
            vec_insert(self, i + 1, range);
            return;
        }
    }

    vec_push(self, range);
}

void range_alloc_dump(RangeAlloc *self)
{
    vec_foreach(range, self)
    {
        log$("- {p} {p}", range_begin(range), range_end(range));
    }
}
