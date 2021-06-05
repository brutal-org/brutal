KERNEL_C_SRC+=$(wildcard sources/kernel/*.c) \
			  $(wildcard sources/kernel/*/*.c)

KERNEL_LIB_SRC = \
			  $(wildcard sources/library/io/*.c) \
			  $(wildcard sources/library/mem/*.c) \
			  $(wildcard sources/library/text/*.c) \
			  $(wildcard sources/library/ds/*.c) \

KERNEL_OBJ= \
	$(patsubst sources/%.c, build/%.c.o, $(KERNEL_C_SRC)) \
	$(patsubst sources/%.c, build/kernel/%.c.o, $(KERNEL_LIB_SRC)) \
	$(patsubst sources/%.s, build/%.s.o, $(KERNEL_S_SRC))

KERNEL_BIN=build/kernel.elf

TARGETS += $(KERNEL_BIN)

build/kernel/%.c.o: sources/kernel/%.c
	$(MKCWD)
	$(CROSS_CC) $(CROSS_KCFLAGS) -c -o $@ $^

build/kernel/library/%.c.o: sources/library/%.c
	$(MKCWD)
	$(CROSS_CC) $(CROSS_KCFLAGS) -c -o $@ $^

$(KERNEL_BIN): $(KERNEL_OBJ)
	$(MKCWD)
	$(CROSS_LD) $(CROSS_KLDFLAGS) $^ -o $@
