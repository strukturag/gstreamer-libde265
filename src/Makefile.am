plugin_LTLIBRARIES = libgstlibde265.la

libgstlibde265_la_SOURCES = \
	gstlibde265.c \
	libde265-dec.c \
	libde265-dec.h \
	matroska/codec-utils.h \
	matroska/codec-utils.c \
	matroska/$(USE_GSTREAMER_VERSION)/ebml-ids.h \
	matroska/$(USE_GSTREAMER_VERSION)/ebml-read.c \
	matroska/$(USE_GSTREAMER_VERSION)/ebml-read.h \
	matroska/$(USE_GSTREAMER_VERSION)/lzo.c \
	matroska/$(USE_GSTREAMER_VERSION)/lzo.h \
	matroska/$(USE_GSTREAMER_VERSION)/matroska-demux.c \
	matroska/$(USE_GSTREAMER_VERSION)/matroska-demux.h \
	matroska/$(USE_GSTREAMER_VERSION)/matroska-ids.c \
	matroska/$(USE_GSTREAMER_VERSION)/matroska-ids.h \
	matroska/$(USE_GSTREAMER_VERSION)/matroska-parse.c \
	matroska/$(USE_GSTREAMER_VERSION)/matroska-parse.h \
	matroska/$(USE_GSTREAMER_VERSION)/matroska-read-common.c \
	matroska/$(USE_GSTREAMER_VERSION)/matroska-read-common.h

libgstlibde265_la_CFLAGS = \
	$(GST_CFLAGS) \
	$(GST_PLUGIN_CFLAGS) \
	$(libde265_CFLAGS) \
	$(zlib_CFLAGS) \
	$(bz2_CFLAGS) \
	-DHAVE_ZLIB \
	-DHAVE_BZ2
libgstlibde265_la_LIBADD = \
	$(GST_LIBS) \
	$(GST_PLUGIN_LIBS) \
	$(libde265_LIBS) \
	$(zlib_LIBS) \
	$(bz2_LIBS)
libgstlibde265_la_LDFLAGS = \
	$(GST_LDFLAGS) \
	$(GST_PLUGIN_LDFLAGS)
libgstlibde265_la_LIBTOOLFLAGS = \
	--tag=disable-static

noinst_HEADERS = \
	libde265-dec.h \
	matroska/codec-utils.h \
	matroska/$(USE_GSTREAMER_VERSION)/ebml-ids.h \
	matroska/$(USE_GSTREAMER_VERSION)/ebml-read.h \
	matroska/$(USE_GSTREAMER_VERSION)/lzo.h \
	matroska/$(USE_GSTREAMER_VERSION)/matroska-demux.h \
	matroska/$(USE_GSTREAMER_VERSION)/matroska-ids.h \
	matroska/$(USE_GSTREAMER_VERSION)/matroska-parse.h \
	matroska/$(USE_GSTREAMER_VERSION)/matroska-read-common.h

EXTRA_DIST = \
	matroska/*.c \
	matroska/*.h \
	matroska/COPYING \
	matroska/README.txt \
	matroska/*/*.c \
	matroska/*/*.h \
	matroska/*/COPYING \
	matroska/*/README.txt \
	matroska/*/*.diff
