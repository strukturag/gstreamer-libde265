/*
 * GStreamer HEVC/H.265 video codec.
 *
 * Copyright (c) 2014 struktur AG, Joachim Bauch <bauch@struktur.de>
 *
 * This file is part of gstreamer-libde265.
 *
 * libde265 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libde265 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libde265.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gst/pbutils/pbutils.h>

#include <gst/gst.h>
#include <libde265/de265.h>

#include "libde265-dec.h"

GST_DEBUG_CATEGORY_EXTERN(matroskareadcommon_debug);

void gst_matroska_register_tags (void);
gboolean gst_matroska_demux_plugin_init (GstPlugin *plugin);
gboolean gst_matroska_parse_plugin_init (GstPlugin *plugin);

static gboolean
plugin_init(GstPlugin *plugin)
{
    gboolean ret;

    de265_init();
    gst_pb_utils_init();
    gst_matroska_register_tags();

    GST_DEBUG_CATEGORY_INIT (matroskareadcommon_debug, "matroskareadcommon", 0,
        "Matroska demuxer/parser shared debug");

    ret = gst_matroska_demux_plugin_init (plugin);
    ret &= gst_matroska_parse_plugin_init (plugin);
    ret &= gst_libde265_dec_plugin_init (plugin);
    return ret;
}

#ifndef VERSION
#define VERSION     "0.1.4"
#endif

#ifndef PACKAGE
#define PACKAGE     "gstreamer-libde265"
#endif

GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
#if GST_CHECK_VERSION(1,0,0)
    gstlibde265,
#else
    "gstlibde265",
#endif
    "HEVC/H.265 decoder using libde265",
    plugin_init,
    VERSION,
    "LGPL",
#if GST_CHECK_VERSION(1,0,0)
    "gstreamer1.0-libde265",
#else
    "gstreamer0.10-libde265",
#endif
    "https://github.com/strukturag/gstreamer-libde265/"
)