UD_NAME = ud

USE_READLINE = yes

HOST_PKGS += UD

ifeq ($(USE_READLINE), yes)
	HOST_LDFLAGS += -lreadline
	HOST_CFLAGS += -DUSE_READLINE
endif
