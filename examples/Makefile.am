bin_PROGRAMS = \
	playhevc \
	timehevc \
	decode-to-png

playhevc_SOURCES = playhevc.c
playhevc_CFLAGS = \
	$(GST_CFLAGS)
playhevc_LDFLAGS = \
	$(GST_LDFLAGS) \
	$(GST_LIBS)

timehevc_SOURCES = timehevc.c
timehevc_CFLAGS = \
	$(GST_CFLAGS)
timehevc_LDFLAGS = \
	$(GST_LDFLAGS) \
	$(GST_LIBS)

decode_to_png_SOURCES = decode_to_png.c
decode_to_png_CFLAGS = \
	$(GST_CFLAGS)
decode_to_png_LDFLAGS = \
	$(GST_LDFLAGS) \
	$(GST_LIBS)

EXTRA_DIST = \
	spreedmovie.mkv
