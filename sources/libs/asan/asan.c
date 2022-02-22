void asan_shadow_check_load(unsigned long address, size_t size, void *return_address)
{
    (void)address;
    (void)size;
    (void)return_address;
}

void asan_shadow_check_store(unsigned long address, size_t size, void *return_address)
{
    (void)address;
    (void)size;
    (void)return_address;
}

#define ASAN_LOAD_STORE(size)                                                \
    void __asan_load##size(unsigned long);                                   \
    void __asan_load##size(unsigned long address)                            \
    {                                                                        \
        asan_shadow_check_load(address, size, __builtin_return_address(0));  \
    }                                                                        \
    void __asan_load##size##_noabort(unsigned long);                         \
    void __asan_load##size##_noabort(unsigned long address)                  \
    {                                                                        \
        asan_shadow_check_load(address, size, __builtin_return_address(0));  \
    }                                                                        \
    void __asan_store##size(unsigned long);                                  \
    void __asan_store##size(unsigned long address)                           \
    {                                                                        \
        asan_shadow_check_store(address, size, __builtin_return_address(0)); \
    }                                                                        \
    void __asan_store##size##_noabort(unsigned long);                        \
    void __asan_store##size##_noabort(unsigned long address)                 \
    {                                                                        \
        asan_shadow_check_store(address, size, __builtin_return_address(0)); \
    }

ASAN_LOAD_STORE(1);
ASAN_LOAD_STORE(2);
ASAN_LOAD_STORE(4);
ASAN_LOAD_STORE(8);
ASAN_LOAD_STORE(16);

void __asan_loadN(unsigned long address, size_t size)
{
    asan_shadow_check_load(address, size, __builtin_return_address(0));
}

void __asan_loadN_noabort(unsigned long address, size_t size)
{
    asan_shadow_check_load(address, size, __builtin_return_address(0));
}

void __asan_storeN(unsigned long address, size_t size)
{
    asan_shadow_check_store(address, size, __builtin_return_address(0));
}

void __asan_storeN_noabort(unsigned long address, size_t size)
{
    asan_shadow_check_store(address, size, __builtin_return_address(0));
}

void __asan_handle_no_return(void)
{
}

void __asan_before_dynamic_init(const char *)
{
}

void __asan_after_dynamic_init()
{
}
