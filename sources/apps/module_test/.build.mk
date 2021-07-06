APP_NAME := module_test 
APP_PATH := sources/apps/module_test

MODULE_TEST_BIN := $(BUILDDIR_CROSS)/apps/module_test.elf

APP_SRC =\
	$(wildcard $(APP_PATH)/*.c) \
	$(wildcard $(APP_PATH)/*/*.c)

APP_LIB_SRC = 

TARGETS += $(MODULE_TEST_BIN)

APP_OBJ= \
	$(patsubst sources/%.c, $(BUILDDIR_CROSS)/%.c.o, $(APP_SRC)) \
	$(patsubst sources/%.c, $(BUILDDIR_CROSS)/apps/module_test/%.c.o, $(APP_LIB_SRC)) \

$(BUILDDIR_CROSS)/apps/module_test/%.c.o: $(APP_PATH)/%.c
	$(MKCWD)
	$(CROSS_CC) -c -o $@ $< $(CROSS_ACFLAGS)

$(MODULE_TEST_BIN): $(APP_OBJ)
	$(MKCWD)
	$(CROSS_LD) -o $@ $^ $(CROSS_ALDFLAGS)

