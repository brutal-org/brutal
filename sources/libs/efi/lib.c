#include <brutal/io.h>
#include <brutal/log.h>
#include <efi/lib.h>

static bool _init = false;
static EFIHandle _handle;
static EFISystemTable *_st;

void efi_init(EFIHandle handle, EFISystemTable *st)
{
    assert_truth(!_init);
    _handle = handle;
    _st = st;

    _init = true;
}

EFIHandle efi_handle(void)
{
    assert_truth(_init);
    return _handle;
}

EFISystemTable *efi_st(void)
{
    assert_truth(_init);
    return _st;
}
