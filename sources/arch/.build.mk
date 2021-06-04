KERNEL_C_SRC+=$(wildcard sources/arch/*.c)
KERNEL_S_SRC+=$(wildcard sources/arch/*.s)

build/arch/%.c.o: sources/arch/%.c
	$(MKCWD)
	$(CC) $(KCFLAGS) -c -o $@ $^

build/arch/%.s.o: sources/arch/%.s
	$(MKCWD)
	$(AS) $(ASFLAGS) -o $@ $^