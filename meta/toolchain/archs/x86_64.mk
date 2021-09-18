
ARCH_KCFLAGS = \
	-fsanitize=undefined \
	-fno-stack-protector \
	-mno-80387 \
	-mno-mmx \
	-mno-3dnow \
	-mno-sse \
	-mno-sse2 \
	-mno-red-zone \
	-mcmodel=kernel \

