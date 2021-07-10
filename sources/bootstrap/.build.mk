BOOTSTRAP_BIN = $(BUILDDIR_CROSS)/bootstrap.elf

BOOSTRAP_SRC = $(wildcard sources/bootstrap/*.c)

TARGETS += $(BOOTSTRAP_BIN)

BOOSTRAP_OBJ= \
	$(patsubst sources/%.c, $(BUILDDIR_CROSS)/%.c.o, $(BOOSTRAP_SRC)) \
	$(patsubst sources/%.c, $(BUILDDIR_CROSS)/bootstrap/%.c.o, $(BOOSTRAP_LIB_SRC)) \

$(BUILDDIR_CROSS)/bootstrap/%.c.o: sources/bootstrap/%.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $< $(CROSS_ACFLAGS)

$(BOOTSTRAP_BIN): $(BOOSTRAP_OBJ)
	$(MKCWD)
	$(CROSS_LD) -o $@ $^ $(CROSS_ALDFLAGS)

