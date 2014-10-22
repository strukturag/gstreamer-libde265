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

#ifndef __GST_LIBDE265_ENC_H__
#define __GST_LIBDE265_ENC_H__

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gst/gst.h>
#if GST_CHECK_VERSION(1,0,0)
    #include <gst/video/gstvideoencoder.h>

    #define VIDEO_ENCODER_BASE      GstVideoEncoder
    #define VIDEO_ENCODER_CLASS     GstVideoEncoderClass
    #define VIDEO_ENCODER_TYPE      GST_TYPE_VIDEO_ENCODER
    #define VIDEO_ENCODER_GET_CLASS GST_VIDEO_ENCODER_CLASS
    #define VIDEO_FRAME             GstVideoCodecFrame
    #define VIDEO_STATE             GstVideoCodecState
    #define ENCODER_FINISH_FRAME    gst_video_encoder_finish_frame
#if GST_CHECK_VERSION(1,2,0)
    #define ENCODER_ALLOC_OUTPUT_FRAME(a,b) gst_video_encoder_allocate_output_frame(a, b, 0)
#else
    #define ENCODER_ALLOC_OUTPUT_FRAME gst_video_encoder_allocate_output_frame
#endif
#else
    #include <gst/video/gstbasevideoencoder.h>

    #define VIDEO_ENCODER_BASE      GstBaseVideoEncoder
    #define VIDEO_ENCODER_CLASS     GstBaseVideoEncoderClass
    #define VIDEO_ENCODER_TYPE      GST_TYPE_BASE_VIDEO_ENCODER
    #define VIDEO_ENCODER_GET_CLASS GST_BASE_VIDEO_ENCODER_CLASS
    #define VIDEO_FRAME             GstVideoFrame
    #define VIDEO_STATE             GstVideoState
    #define ENCODER_FINISH_FRAME    gst_base_video_encoder_finish_frame
    #define ENCODER_ALLOC_OUTPUT_FRAME gst_base_video_encoder_alloc_src_frame
#endif

#include <libde265/en265.h>

G_BEGIN_DECLS

#define GST_TYPE_LIBDE265_ENC \
    (gst_libde265_enc_get_type())
#define GST_LIBDE265_ENC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_LIBDE265_ENC,GstLibde265Enc))
#define GST_LIBDE265_ENC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_LIBDE265_ENC,GstLibde265EncClass))

typedef struct _GstLibde265Enc {
    VIDEO_ENCODER_BASE      parent;

    /* private */
    en265_encoder_context   *ctx;
    struct de265_image_spec spec;
#if GST_CHECK_VERSION(1,0,0)
    GstVideoCodecState      *input_state;
#else
    int                     width;
    int                     height;
#endif
    const VIDEO_STATE       *output_state;
    GstBuffer               *codec_data;
    GstBuffer               *output_buffer;
    GstMemory               *nal_header_memory;
} GstLibde265Enc;

typedef struct _GstLibde265EncClass {
    VIDEO_ENCODER_CLASS     parent;
} GstLibde265EncClass;

G_END_DECLS

gboolean gst_libde265_enc_plugin_init (GstPlugin *plugin);

#endif  // __GST_LIBDE265_ENC_H__
