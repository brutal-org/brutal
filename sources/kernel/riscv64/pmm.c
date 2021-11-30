#include <brutal/base/range.h>
#include <brutal/debug.h>
#include <brutal/ds.h>
#include <brutal/mem.h>
#include <brutal/sync.h>
#include <hw/fdt/fdt.h>
#include "kernel/arch.h"
#include "kernel/mmap.h"
#include "kernel/pmm.h"

static Bits pmm_bitmap = {};
static PmmRange bitmap_range;
static PmmRange memory_range;
static Lock pmm_lock = {};
static size_t used_memory = 0;
static size_t best_bet_upper = 0;
static size_t best_bet_lower = (size_t)-1;

static PmmRange get_needed_memory_range(FdtHeader *header)
{
    FdtNode root_node = fdt_lookup_node(header, str$(""));
    FdtProp root_addr_size = fdt_lookup_props(root_node, str$("#address-cells"));
    FdtProp root_len_size = fdt_lookup_props(root_node, str$("#size-cells"));

    // TODO: for the moment we don't support 32bit address size
    int addr_size = load_be(*(be_uint32_t *)root_addr_size.value.buf);
    int len_size = load_be(*(be_uint32_t *)root_len_size.value.buf);

    // getting /memory@*/reg
    FdtProp memory_reg_prop = fdt_lookup_props(fdt_lookup_node(header, str$("/memory@")), str$("reg"));

    return range$(PmmRange, fdt_reg_get_range(memory_reg_prop, addr_size, len_size));
}

static void pmm_bitmap_initialize(void)
{
    log$("Allocating memory bitmap...");
    // we will use the bottom of the memory for the bitmap
    size_t bitmap_size = memory_range.size / (MEM_PAGE_SIZE * 8);

    log$("A bitmap {}kib long is needed", bitmap_size / 1024);

    bitmap_range = (PmmRange){
        .base = memory_range.base + memory_range.size - bitmap_size - MEM_PAGE_SIZE,
        .size = bitmap_size};

    bitmap_range = range_align_higher(bitmap_range, MEM_PAGE_SIZE);

    log$("allocated bitmap at: {#x}-{#x}", (uintptr_t)bitmap_range.base, (uintptr_t)bitmap_range.base + bitmap_range.size);

    bits_init(&pmm_bitmap, (void *)(bitmap_range.base), bitmap_range.size);
    bits_fill(&pmm_bitmap, PMM_UNUSED);
}

static Iter pmm_load_reserved_memory_iter(FdtNode *val, MAYBE_UNUSED void *ctx)
{
    if (str_count(val->name, str$("mmode_resv")) == -1)
    {
        return ITER_CONTINUE;
    }

    FdtProp memory_prop = fdt_lookup_props(*val, str$("reg"));

    // FIXME: we assume #address-cells and #size-cells is equal to 2
    PmmRange resv_mem = range$(PmmRange, fdt_reg_get_range(memory_prop, 2, 2));

    pmm_used(resv_mem);

    log$("reserved memory: {#x}-{#x}", resv_mem.base, resv_mem.size);
    return ITER_CONTINUE;
}

static void pmm_load_reserved_memory(FdtHeader *header)
{
    FdtNode reserved_memory_node = fdt_lookup_node(header, str$("/reserved-memory"));

    fdt_node_childs(reserved_memory_node, (IterFn *)pmm_load_reserved_memory_iter, NULL);
}

void pmm_initialize(FdtHeader *header, PmmRange kernel_physical_range)
{
    memory_range = get_needed_memory_range(header);
    pmm_bitmap_initialize();
    pmm_load_reserved_memory(header);

    pmm_used(range_align_higher(kernel_physical_range, MEM_PAGE_SIZE));
    pmm_used(bitmap_range);
}

PmmResult pmm_alloc(MAYBE_UNUSED size_t size, MAYBE_UNUSED bool upper)
{
    LOCK_RETAINER(&pmm_lock);

    assert_truth(mem_is_size_page_aligned(size));

    used_memory += size;

    size_t page_size = size / MEM_PAGE_SIZE;
    USizeRange page_range = bits_find_free(&pmm_bitmap, upper ? best_bet_upper : best_bet_lower, page_size, upper);

    if (range_empty(page_range))
    {
        if (upper)
        {
            best_bet_upper = -1;
        }
        else
        {
            best_bet_lower = 0;
        }

        page_range = bits_find_free(&pmm_bitmap, upper ? -1 : 0, page_size, upper);
    }

    if (!range_any(page_range))
    {
        log$("pmm_alloc({}): out of memory!", size);
        return ERR(PmmResult, BR_OUT_OF_MEMORY);
    }

    if (upper)
    {
        best_bet_upper = range_begin(page_range);
    }
    else
    {
        best_bet_lower = range_end(page_range);
    }

    bits_set_range(&pmm_bitmap, page_range, PMM_USED);

    PmmRange pmm_range = range$(PmmRange, page_range);

    pmm_range = range_mul(pmm_range, MEM_PAGE_SIZE);
    pmm_range.base += memory_range.base;

    return OK(PmmResult, pmm_range);
}

PmmResult pmm_used(PmmRange range)
{
    LOCK_RETAINER(&pmm_lock);

    assert_truth(mem_is_range_page_aligned(range));

    size_t page_base = range.base - memory_range.base;
    size_t page_size = range.size;

    USizeRange page_range = {page_base, page_size};
    page_range = range_div(page_range, MEM_PAGE_SIZE);

    bits_set_range(&pmm_bitmap, page_range, PMM_USED);

    return OK(PmmResult, range);
}

PmmResult pmm_unused(PmmRange range)
{
    LOCK_RETAINER(&pmm_lock);

    assert_truth(mem_is_range_page_aligned(range));

    used_memory -= range.size;

    if (range.base == 0)
    {
        return ERR(PmmResult, BR_BAD_ADDRESS);
    }

    size_t page_base = range.base - memory_range.base;
    size_t page_size = range.size;
    USizeRange page_range = {page_base, page_size};
    page_range = range_div(page_range, MEM_PAGE_SIZE);

    bits_set_range(&pmm_bitmap, page_range, PMM_UNUSED);

    if (best_bet_upper < range_end(page_range))
    {
        best_bet_upper = range_end(page_range);
    }

    if (best_bet_lower > range_begin(page_range))
    {
        best_bet_lower = range_begin(page_range);
    }

    return OK(PmmResult, range);
}
