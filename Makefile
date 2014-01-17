GSTREAMER_VERSION:=0.10

# Dependencies for gstreamer0.10:
# apt-get install libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev libgstreamer-plugins-bad0.10-dev

# Dependencies for gstreamer1.0:
# apt-get install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libgstreamer-plugins-good1.0-dev libgstreamer-plugins-bad1.0-dev

LIBDE265_ROOT:=../libde265

SOURCES:=  \
	src/gstlibde265.c\
	src/libde265-dec.c

MATROSKA_SOURCES:= \
	src/matroska/ebml-read.c \
	src/matroska/matroska-demux.c \
	src/matroska/matroska-ids.c \
	src/matroska/matroska-parse.c \
	src/matroska/matroska-read-common.c \
	src/matroska/lzo.c

CC:=gcc

LDFLAGS_GSTREAMER:=$(shell pkg-config --libs gstreamer-video-$(GSTREAMER_VERSION))
CFLAGS_GSTREAMER:=$(shell pkg-config --cflags gstreamer-video-$(GSTREAMER_VERSION))
LDFLAGS_GSTREAMER+=$(shell pkg-config --libs gstreamer-pbutils-$(GSTREAMER_VERSION))
CFLAGS_GSTREAMER+=$(shell pkg-config --cflags gstreamer-pbutils-$(GSTREAMER_VERSION))
LDFLAGS_GSTREAMER+=$(shell pkg-config --libs gstreamer-riff-$(GSTREAMER_VERSION))
CFLAGS_GSTREAMER+=$(shell pkg-config --cflags gstreamer-riff-$(GSTREAMER_VERSION))

ifneq ($(GSTREAMER_VERSION), 1.0)
LDFLAGS_GSTREAMER+=$(shell pkg-config --libs gstreamer-basevideo-$(GSTREAMER_VERSION))
CFLAGS_GSTREAMER+=$(shell pkg-config --cflags gstreamer-basevideo-$(GSTREAMER_VERSION))
endif

CFLAGS:= \
	-fPIC \
	-Wall \
	-I$(LIBDE265_ROOT) \
	-DGST_USE_UNSTABLE_API \
	$(CFLAGS_GSTREAMER)

LDFLAGS:= \
	-L$(LIBDE265_ROOT)/libde265/.libs \
	-lde265 \
	$(LDFLAGS_GSTREAMER)

OBJS:=$(patsubst %.c,%.o,$(SOURCES))
MATROSKA_OBJS:=$(patsubst %.c,%.o,$(MATROSKA_SOURCES))

all: libgstlibde265.so

%.o : %.c
	$(CC) -o $@ -c $< -g $(CFLAGS)

libgstlibde265.so: $(OBJS) $(MATROSKA_OBJS)
	$(CC) -shared -o $@ $^ -g $(LDFLAGS)

clean:
	rm -f libgstlibde265.so $(OBJS) $(MATROSKA_OBJS)
