plugin_LTLIBRARIES = libgstlibde265.la

libgstlibde265_la_SOURCES = \
	gstlibde265.c \
	libde265-dec.c \
	libde265-dec.h \
	common/codec-utils.h \
	common/codec-utils.c \
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
	matroska/$(USE_GSTREAMER_VERSION)/matroska-read-common.h \
	isomp4/$(USE_GSTREAMER_VERSION)/qtatomparser.h   \
	isomp4/$(USE_GSTREAMER_VERSION)/qtdemux.h        \
	isomp4/$(USE_GSTREAMER_VERSION)/qtdemux_types.h  \
	isomp4/$(USE_GSTREAMER_VERSION)/qtdemux_dump.h   \
	isomp4/$(USE_GSTREAMER_VERSION)/qtdemux_fourcc.h \
	isomp4/$(USE_GSTREAMER_VERSION)/qtdemux_lang.h   \
	isomp4/$(USE_GSTREAMER_VERSION)/qtpalette.h      \
	isomp4/$(USE_GSTREAMER_VERSION)/atoms.h          \
	isomp4/$(USE_GSTREAMER_VERSION)/atomsrecovery.h  \
	isomp4/$(USE_GSTREAMER_VERSION)/descriptors.h    \
	isomp4/$(USE_GSTREAMER_VERSION)/properties.h     \
	isomp4/$(USE_GSTREAMER_VERSION)/fourcc.h         \
	isomp4/$(USE_GSTREAMER_VERSION)/ftypcc.h         \
	isomp4/$(USE_GSTREAMER_VERSION)/isomp4-plugin.c  \
	isomp4/$(USE_GSTREAMER_VERSION)/qtdemux.c        \
	isomp4/$(USE_GSTREAMER_VERSION)/qtdemux_types.c  \
	isomp4/$(USE_GSTREAMER_VERSION)/qtdemux_dump.c   \
	isomp4/$(USE_GSTREAMER_VERSION)/qtdemux_lang.c   \
	isomp4/$(USE_GSTREAMER_VERSION)/atoms.c          \
	isomp4/$(USE_GSTREAMER_VERSION)/atomsrecovery.c  \
	isomp4/$(USE_GSTREAMER_VERSION)/descriptors.c    \
	isomp4/$(USE_GSTREAMER_VERSION)/properties.c

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
	common/codec-utils.h \
	matroska/$(USE_GSTREAMER_VERSION)/ebml-ids.h \
	matroska/$(USE_GSTREAMER_VERSION)/ebml-read.h \
	matroska/$(USE_GSTREAMER_VERSION)/lzo.h \
	matroska/$(USE_GSTREAMER_VERSION)/matroska-demux.h \
	matroska/$(USE_GSTREAMER_VERSION)/matroska-ids.h \
	matroska/$(USE_GSTREAMER_VERSION)/matroska-parse.h \
	matroska/$(USE_GSTREAMER_VERSION)/matroska-read-common.h \
	isomp4/$(USE_GSTREAMER_VERSION)/qtatomparser.h   \
	isomp4/$(USE_GSTREAMER_VERSION)/qtdemux.h        \
	isomp4/$(USE_GSTREAMER_VERSION)/qtdemux_types.h  \
	isomp4/$(USE_GSTREAMER_VERSION)/qtdemux_dump.h   \
	isomp4/$(USE_GSTREAMER_VERSION)/qtdemux_fourcc.h \
	isomp4/$(USE_GSTREAMER_VERSION)/qtdemux_lang.h   \
	isomp4/$(USE_GSTREAMER_VERSION)/qtpalette.h      \
	isomp4/$(USE_GSTREAMER_VERSION)/atoms.h          \
	isomp4/$(USE_GSTREAMER_VERSION)/atomsrecovery.h  \
	isomp4/$(USE_GSTREAMER_VERSION)/descriptors.h    \
	isomp4/$(USE_GSTREAMER_VERSION)/properties.h     \
	isomp4/$(USE_GSTREAMER_VERSION)/fourcc.h         \
	isomp4/$(USE_GSTREAMER_VERSION)/ftypcc.h

EXTRA_DIST = \
	common/*.c \
	common/*.h \
	common/COPYING \
	common/README.txt \
	matroska/*/*.c \
	matroska/*/*.h \
	matroska/*/COPYING \
	matroska/*/README.txt \
	matroska/*/*.diff \
	isomp4/*/*.c \
	isomp4/*/*.h \
	isomp4/*/COPYING \
	isomp4/*/README.txt \
	isomp4/*/*.diff
