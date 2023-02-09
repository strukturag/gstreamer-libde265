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

#ifndef __GST_LIBDE265_DEC_H__
#define __GST_LIBDE265_DEC_H__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gst/gst.h>
#include <gst/video/gstvideodecoder.h>

#include <libde265/de265.h>

G_BEGIN_DECLS

#define GST_TYPE_LIBDE265_DEC \
    (gst_libde265_dec_get_type())
#define GST_LIBDE265_DEC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_LIBDE265_DEC,GstLibde265Dec))
#define GST_LIBDE265_DEC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_LIBDE265_DEC,GstLibde265DecClass))


typedef enum {
  GST_TYPE_LIBDE265_DEC_PACKETIZED,
  GST_TYPE_LIBDE265_DEC_RAW
} GstLibde265DecMode;

typedef struct _GstLibde265Dec {
    GstVideoDecoder      parent;

    /* private */
    de265_decoder_context   *ctx;
    int                     width;
    int                     height;
    GstLibde265DecMode      mode;
    int                     length_size;
    int                     fps_n;
    int                     fps_d;
    int                     max_threads;
    int                     buffer_full;
    void                    *codec_data;
    int                     codec_data_size;

    int                     frame_number;
    GstVideoCodecState      *input_state;
    GstVideoCodecState      *output_state;
} GstLibde265Dec;

typedef struct _GstLibde265DecClass {
  GstVideoDecoderClass     parent;
} GstLibde265DecClass;

G_END_DECLS

gboolean gst_libde265_dec_plugin_init (GstPlugin *plugin);

#endif  // __GST_LIBDE265_DEC_H__
