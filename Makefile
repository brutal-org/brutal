.SUFFIXES:
.DEFAULT_GOAL := all

CC=gcc
CFLAGS=-std=gnu2x -Wall -Wextra -Werror -Isrc/lib

AR=ar
ARFLAGS=rcsv

MKCWD=mkdir -p $(@D)

include src/lib/.build.mk

all: $(TARGETS)

clean:
	rm -rf build/
