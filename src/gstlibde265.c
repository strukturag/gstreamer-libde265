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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gst/pbutils/pbutils.h>

#include <gst/gst.h>
#include <libde265/de265.h>

#include "libde265-dec.h"


static gboolean
plugin_init(GstPlugin* plugin)
{
  gboolean ret = TRUE;

  gst_pb_utils_init();

  ret &= gst_libde265_dec_plugin_init(plugin);
  return ret;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR, GST_VERSION_MINOR,
    libde265,
"HEVC/H.265 decoder using libde265", plugin_init, VERSION, "LGPL",
"gstreamer1.0-libde265",
"https://github.com/strukturag/gstreamer-libde265/")
