plugin_LTLIBRARIES = libgstlibde265.la

libgstlibde265_la_SOURCES = \
	gstlibde265.c \
	libde265-dec.c \
	libde265-dec.h \
	common/codec-utils.h \
	common/codec-utils.c

libgstlibde265_la_CFLAGS = \
	$(GST_CFLAGS) \
	$(GST_PLUGIN_CFLAGS) \
	$(libde265_CFLAGS)

libgstlibde265_la_LIBADD = \
	$(GST_LIBS) \
	$(GST_PLUGIN_LIBS) \
	$(libde265_LIBS)

libgstlibde265_la_LDFLAGS = \
	$(GST_LDFLAGS) \
	$(GST_PLUGIN_LDFLAGS)

libgstlibde265_la_LIBTOOLFLAGS = \
	--tag=disable-static

noinst_HEADERS = \
	libde265-dec.h \
	common/codec-utils.h

EXTRA_DIST = \
	common/*.c \
	common/*.h \
	common/COPYING \
	common/README.txt
