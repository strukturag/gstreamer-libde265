GSTREAMER_VERSION:=0.10

ifeq ($(origin CC), default)
	CC := gcc
endif

CFLAGS_GSTREAMER:=$(shell pkg-config --cflags gstreamer-$(GSTREAMER_VERSION))
LDFLAGS_GSTREAMER:=$(shell pkg-config --libs gstreamer-$(GSTREAMER_VERSION))

all: playhevc

playhevc: playhevc.c
	$(CC) -g -o $@ $< $(CFLAGS_GSTREAMER) $(LDFLAGS_GSTREAMER)

clean:
	rm -f playhevc
