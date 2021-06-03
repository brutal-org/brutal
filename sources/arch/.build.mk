KERNEL_SRC+=$(wildcard sources/arch/*.c)

build/arch/%.c.o: sources/arch/%.c
	$(MKCWD)
	$(CC) $(KCFLAGS) -c -o $@ $^