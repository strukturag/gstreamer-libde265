GSTREAMER_VERSION:=0.10

CC:=gcc

CFLAGS_GSTREAMER:=$(shell pkg-config --cflags gstreamer-$(GSTREAMER_VERSION))
LDFLAGS_GSTREAMER:=$(shell pkg-config --libs gstreamer-$(GSTREAMER_VERSION))

all: playhevc

playhevc: playhevc.c
	gcc -g -o $@ $< $(CFLAGS_GSTREAMER) $(LDFLAGS_GSTREAMER)

clean:
	rm -f playhevc
