dnl required version of autoconf
AC_PREREQ([2.53])

AC_INIT([gstreamer-libde265],[0.1.12])

dnl required versions of gstreamer
GST_REQUIRED=1.0.0
GSTREAMER_VERSION=1.0

AC_CONFIG_SRCDIR([src/gstlibde265.c])
AC_CONFIG_HEADERS([config.h])

dnl required version of automake
AM_INIT_AUTOMAKE([1.10])

dnl enable mainainer mode by default
AM_MAINTAINER_MODE([enable])

dnl check for tools (compiler etc.)
AC_PROG_CC
AM_PROG_CC_C_O

dnl required version of libtool
LT_PREREQ([2.2.6])
LT_INIT

AC_TYPE_UINT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_INT8_T
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

AC_C_INLINE
AC_CHECK_HEADERS([sys/time.h])
AC_SEARCH_LIBS([floor], [m])
AC_CHECK_FUNCS([gettimeofday])

AC_CHECK_PROG(HAVE_PKGCONFIG, pkg-config, [ ], [
  AC_MSG_ERROR([You need to have pkg-config installed!])
])

dnl possibly modify pkg-config path
AC_ARG_WITH(pkg-config-path,
    AC_HELP_STRING([--with-pkg-config-path],
                   [colon-separated list of pkg-config(1) dirs]),
    [
        export PKG_CONFIG_PATH=${withval}
        AC_MSG_NOTICE(Set PKG_CONFIG_PATH to $PKG_CONFIG_PATH)
    ])

AC_ARG_ENABLE(gstreamer1.0,
              [AS_HELP_STRING([--enable-gstreamer1.0],
                              [build for GStreamer 1.0 (default=yes)])],
  [enable_gstreamer10=$enableval],
  [enable_gstreamer10=yes])
if eval "test $enable_gstreamer10 = yes"; then
    GST_REQUIRED=1.0.0
    GSTREAMER_VERSION=1.0
fi

AC_ARG_ENABLE(gstreamer0.10,
              [AS_HELP_STRING([--enable-gstreamer0.10],
                              [build for GStreamer 0.10 instead of 1.0 (default=no)])],
  [enable_gstreamer010=$enableval],
  [enable_gstreamer010=no])
if eval "test $enable_gstreamer010 = yes"; then
    GST_REQUIRED=0.10
    GSTREAMER_VERSION=0.10
fi

AM_CONDITIONAL([USE_GSTREAMER_010], [test x"$enable_gstreamer010" = x"yes"])

PKG_CHECK_MODULES(GST, [
  gstreamer-$GSTREAMER_VERSION >= $GST_REQUIRED
  gstreamer-video-$GSTREAMER_VERSION >= $GST_REQUIRED
  gstreamer-pbutils-$GSTREAMER_VERSION >= $GST_REQUIRED
  gstreamer-riff-$GSTREAMER_VERSION >= $GST_REQUIRED
], [
  AC_SUBST(GST_CFLAGS)
  AC_SUBST(GST_LIBS)
  AC_SUBST(GST_LDFLAGS)
], [
  AC_MSG_ERROR([
    Can't find the following GStreamer development packages:

      gstreamer-$GSTREAMER_VERSION >= $GST_REQUIRED
      gstreamer-video-$GSTREAMER_VERSION >= $GST_REQUIRED
      gstreamer-pbutils-$GSTREAMER_VERSION >= $GST_REQUIRED
      gstreamer-riff-$GSTREAMER_VERSION >= $GST_REQUIRED

    Please make sure you have the necessary GStreamer-$GSTREAMER_VERSION
    development headers installed.

    On debian/Ubuntu systems you will probably need to install the
    'libgstreamer$GSTREAMER_VERSION-dev' and 'libgstreamer-plugins-base$GSTREAMER_VERSION-dev' packages.

    On RPM-based systems you will probably need to install the
    'gstreamer-devel-$GSTREAMER_VERSION' package.
  ])
])

PKG_CHECK_MODULES(GST_PLUGIN, [
  gstreamer-plugins-base-$GSTREAMER_VERSION >= $GST_REQUIRED
], [
  AC_SUBST(GST_PLUGIN_CFLAGS)
  AC_SUBST(GST_PLUGIN_LIBS)
  AC_SUBST(GST_PLUGIN_LDFLAGS)
], [
  AC_MSG_ERROR([
    Can't find the following GStreamer development packages:

      gstreamer-plugins-base-$GSTREAMER_VERSION >= $GST_REQUIRED

    Please make sure you have the necessary GStreamer-$GSTREAMER_VERSION
    development headers installed.

    On debian/Ubuntu systems you will probably need to install the
    'libgstreamer$GSTREAMER_VERSION-dev' and 'libgstreamer-plugins-base$GSTREAMER_VERSION-dev' packages.

    On RPM-based systems you will probably need to install the
    'gstreamer-devel-$GSTREAMER_VERSION' package.
  ])
])

if eval "test $enable_gstreamer010 = yes"; then
    PKG_CHECK_MODULES(GST010, [
      gstreamer-basevideo-$GSTREAMER_VERSION >= $GST_REQUIRED
    ], [
      GST_CFLAGS="$GST_CFLAGS $GST010_CFLAGS"
      GST_LIBS="$GST_LIBS $GST010_LIBS"
      GST_LDFLAGS="$GST_LDFLAGS $GST010_LDFLAGS"
    ], [
      AC_MSG_ERROR([
        Can't find the following GStreamer development packages:

          gstreamer-basevideo-$GSTREAMER_VERSION >= $GST_REQUIRED

        Please make sure you have the necessary GStreamer-$GSTREAMER_VERSION
        development headers installed.

        On debian/Ubuntu systems you will probably need to install the
        'libgstreamer$GSTREAMER_VERSION-dev' and 'libgstreamer-plugins-base$GSTREAMER_VERSION-dev' packages.

        On RPM-based systems you will probably need to install the
        'gstreamer-devel-$GSTREAMER_VERSION' package.
      ])
    ])

    GST_CFLAGS="$GST_CFLAGS -DGST_USE_UNSTABLE_API"
fi

AC_MSG_CHECKING(for GStreamer version)
save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS $GST_CFLAGS"
gst_version=unknown
# The order of these tests is important.
AC_TRY_COMPILE([#include <gst/gst.h>],
[#if !GST_CHECK_VERSION(1,4,0)
#error "not 1.4.0"
#endif], USE_GSTREAMER_VERSION=1.4 gst_version=1.4.0)
if test $gst_version = unknown; then
AC_TRY_COMPILE([#include <gst/gst.h>],
[#if !GST_CHECK_VERSION(1,2,0)
#error "not 1.2.0"
#endif], USE_GSTREAMER_VERSION=1.2 gst_version=1.2.0)
fi
if test $gst_version = unknown; then
AC_TRY_COMPILE([#include <gst/gst.h>],
[#if !GST_CHECK_VERSION(1,0,0)
#error "not 1.0.0"
#endif], USE_GSTREAMER_VERSION=1.0 gst_version=1.0.0)
fi
if test $gst_version = unknown; then
USE_GSTREAMER_VERSION=0.10
gst_version=0.10.0
fi
AC_SUBST(USE_GSTREAMER_VERSION)
AC_MSG_RESULT($gst_version)
CFLAGS="$save_CFLAGS"

AM_CONDITIONAL([INCLUDE_MATROSKA_DEMUXER], [test "$USE_GSTREAMER_VERSION" != "1.4"])
AM_CONDITIONAL([INCLUDE_MP4_DEMUXER], [test "$USE_GSTREAMER_VERSION" != "1.4"])

if eval "test $USE_GSTREAMER_VERSION != 1.4" ; then
  PKG_CHECK_MODULES(GST_AUDIO_TAG, [
    gstreamer-audio-$GSTREAMER_VERSION >= $GST_REQUIRED
    gstreamer-tag-$GSTREAMER_VERSION >= $GST_REQUIRED
  ], [
    GST_CFLAGS="$GST_CFLAGS $GST_AUDIO_TAG_CFLAGS"
    GST_LIBS="$GST_LIBS $GST_AUDIO_TAG_LIBS"
    GST_LDFLAGS="$GST_LDFLAGS $GST_AUDIO_TAG_LDFLAGS"
  ], [
    AC_MSG_ERROR([
      Can't find the following GStreamer development packages:

        gstreamer-audio-$GSTREAMER_VERSION >= $GST_REQUIRED
        gstreamer-tag-$GSTREAMER_VERSION >= $GST_REQUIRED

      Please make sure you have the necessary GStreamer-$GSTREAMER_VERSION
      development headers installed.

      On debian/Ubuntu systems you will probably need to install the
      'libgstreamer$GSTREAMER_VERSION-dev' and 'libgstreamer-plugins-base$GSTREAMER_VERSION-dev' packages.

     On RPM-based systems you will probably need to install the
      'gstreamer-devel-$GSTREAMER_VERSION' package.
    ])
  ])

  PKG_CHECK_MODULES(zlib, [zlib], [
    AC_SUBST(zlib_CFLAGS)
    AC_SUBST(zlib_LIBS)
  ], [
    AC_MSG_ERROR([You need to install the zlib development package.])
  ])

  save_LIBS="$LIBS"
  AC_SEARCH_LIBS([BZ2_bzDecompressInit], [bz2], [
    bz2_CFLAGS=""
    bz2_LIBS="-lbz2"
    AC_SUBST(bz2_CFLAGS)
    AC_SUBST(bz2_LIBS)
  ], [
    AC_MSG_ERROR([You need to install the bz2 development package.])
  ])
  LIBS="$save_LIBS"
fi

PKG_CHECK_MODULES(libde265, [libde265 >= 0.7], [
  AC_SUBST(libde265_CFLAGS)
  AC_SUBST(libde265_LIBS)
], [
  AC_MSG_ERROR([You need to install the libde265 development package.])
])

dnl check if compiler understands -Wall (if yes, add -Wall to GST_CFLAGS)
AC_MSG_CHECKING([to see if compiler understands -Wall])
save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -Wall"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([ ], [ ])], [
  GST_CFLAGS="$GST_CFLAGS -Wall"
  AC_MSG_RESULT([yes])
], [
  AC_MSG_RESULT([no])
])
CFLAGS="$save_CFLAGS"

dnl set the plugindir where plugins should be installed (for src/Makefile.am)
if test "x${prefix}" = "x$HOME"; then
  plugindir="$HOME/.gstreamer-$GSTREAMER_VERSION/plugins"
else
  plugindir="\$(libdir)/gstreamer-$GSTREAMER_VERSION"
fi
AC_SUBST(plugindir)

dnl set proper LDFLAGS for plugins
GST_PLUGIN_LDFLAGS='-module -avoid-version -export-symbols-regex [_]*\(gst_\|Gst\|GST_\).*'
AC_SUBST(GST_PLUGIN_LDFLAGS)

AC_CONFIG_FILES([Makefile common/Makefile common/m4/Makefile src/Makefile examples/Makefile])
AC_OUTPUT
