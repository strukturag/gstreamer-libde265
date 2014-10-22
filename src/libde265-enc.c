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

/**
 * SECTION:element-libde265enc
 *
 * Encodes HEVC/H.265 video.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-1.0 videotestsrc pattern=18 num-buffers=64 ! libde265enc ! matroskamux-libde265 ! filesink location=encoded.mkv
 * ]| The above pipeline encodes the bouncing ball video test pattern to a HEVC/H.265 bitstream and muxes it into an Matroska container.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "libde265-enc.h"

/* TODO(fancycode): add support for GStreamer 0.10 */
#if !GST_CHECK_VERSION(1,0,0)
#error "Only GStreamer 1.0 supported yet"
#endif

#define parent_class gst_libde265_enc_parent_class
G_DEFINE_TYPE (GstLibde265Enc, gst_libde265_enc, VIDEO_ENCODER_TYPE);

static char NAL_HEADER[] = { '\x00', '\x00', '\x01' };

static GstStaticPadTemplate sink_template = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (
#if GST_CHECK_VERSION(1,0,0)
        GST_VIDEO_CAPS_MAKE ("{ I420 }")
#else
        GST_VIDEO_CAPS_YUV ("{ I420 }")
#endif
    )
    );

static GstStaticPadTemplate src_template = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("video/x-h265")
    );

static void gst_libde265_enc_finalize (GObject * object);

static gboolean gst_libde265_enc_start (VIDEO_ENCODER_BASE * encoder);
static gboolean gst_libde265_enc_stop (VIDEO_ENCODER_BASE * encoder);
static gboolean gst_libde265_enc_set_format (VIDEO_ENCODER_BASE * encoder,
    VIDEO_STATE * state);
static GstFlowReturn gst_libde265_enc_handle_frame (VIDEO_ENCODER_BASE *
    encoder, VIDEO_FRAME * frame);
static GstFlowReturn gst_libde265_enc_finish (VIDEO_ENCODER_BASE * encoder);

static void
gst_libde265_enc_class_init (GstLibde265EncClass * klass)
{
  GObjectClass *gobject_class = (GObjectClass *) klass;
  VIDEO_ENCODER_CLASS *encoder_class = VIDEO_ENCODER_GET_CLASS (klass);
  GstElementClass *gstelement_class = GST_ELEMENT_CLASS (klass);

  gobject_class->finalize = gst_libde265_enc_finalize;

  encoder_class->start = GST_DEBUG_FUNCPTR (gst_libde265_enc_start);
  encoder_class->stop = GST_DEBUG_FUNCPTR (gst_libde265_enc_stop);
  encoder_class->set_format = GST_DEBUG_FUNCPTR (gst_libde265_enc_set_format);
  encoder_class->handle_frame =
      GST_DEBUG_FUNCPTR (gst_libde265_enc_handle_frame);
  encoder_class->finish = GST_DEBUG_FUNCPTR (gst_libde265_enc_finish);

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sink_template));
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_template));

  gst_element_class_set_details_simple (gstelement_class,
      "HEVC/H.265 encoder",
      "Codec/Encoder/Video",
      "Encodes HEVC/H.265 video streams using libde265",
      "struktur AG <opensource@struktur.de>");
}

static inline void
_gst_libde265_enc_free_encoder (GstLibde265Enc * enc)
{
  if (enc->ctx != NULL) {
    en265_free_encoder (enc->ctx);
    enc->ctx = NULL;
  }
  if (enc->codec_data != NULL) {
    gst_buffer_unref (enc->codec_data);
    enc->codec_data = NULL;
  }
  if (enc->output_buffer != NULL) {
    gst_buffer_unref (enc->output_buffer);
    enc->output_buffer = NULL;
  }
  if (enc->nal_header_memory != NULL) {
    gst_memory_unref (enc->nal_header_memory);
    enc->nal_header_memory = NULL;
  }
}

static inline gboolean
_gst_libde265_enc_reset_encoder (GstLibde265Enc * enc)
{
  _gst_libde265_enc_free_encoder (enc);
  enc->codec_data = gst_buffer_new ();
  if (enc->codec_data == NULL) {
    return FALSE;
  }

  if (enc->nal_header_memory == NULL) {
    enc->nal_header_memory =
        gst_memory_new_wrapped (GST_MEMORY_FLAG_READONLY, NAL_HEADER,
        sizeof (NAL_HEADER), 0, sizeof (NAL_HEADER), NULL, NULL);
    if (enc->nal_header_memory == NULL) {
      return FALSE;
    }
  }

  return TRUE;
}

static void
gst_libde265_enc_init (GstLibde265Enc * encoder)
{
  GstLibde265Enc *enc = GST_LIBDE265_ENC (encoder);

  enc->ctx = NULL;
  enc->codec_data = NULL;
  enc->output_buffer = NULL;
  enc->nal_header_memory = NULL;
}

static void
gst_libde265_enc_finalize (GObject * object)
{
  GstLibde265Enc *enc = GST_LIBDE265_ENC (object);

  _gst_libde265_enc_free_encoder (enc);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static gboolean
gst_libde265_enc_start (VIDEO_ENCODER_BASE * encoder)
{
  GstLibde265Enc *enc = GST_LIBDE265_ENC (encoder);

  if (!_gst_libde265_enc_reset_encoder (enc)) {
    GST_ERROR_OBJECT (encoder, "Failed to reset encoder context");
    return FALSE;
  }

  enc->ctx = en265_new_encoder ();
  if (enc->ctx == NULL) {
    GST_ERROR_OBJECT (encoder, "Failed to allocate encoder context");
    return FALSE;
  }

  en265_show_params (enc->ctx);
  return TRUE;
}

static gboolean
gst_libde265_enc_stop (VIDEO_ENCODER_BASE * encoder)
{
  GstLibde265Enc *enc = GST_LIBDE265_ENC (encoder);

  _gst_libde265_enc_free_encoder (enc);

  return TRUE;
}

static gboolean
gst_libde265_enc_set_format (VIDEO_ENCODER_BASE * encoder, VIDEO_STATE * state)
{
  GstLibde265Enc *enc = GST_LIBDE265_ENC (encoder);
  int width;
  int height;
  GstCaps *out_caps;

#if GST_CHECK_VERSION(1,0,0)
  if (enc->input_state != NULL) {
    gst_video_codec_state_unref (enc->input_state);
  }
  enc->input_state = state;
  if (state != NULL) {
    gst_video_codec_state_ref (state);
  }
  width = state->info.width;
  height = state->info.height;
#else
  width = enc->width = state->width;
  height = enc->height = state->height;
#endif
  en265_get_image_spec (enc->ctx, width, height, de265_chroma_420, &enc->spec);

  out_caps = gst_caps_new_simple ("video/x-h265",
      "stream-format", G_TYPE_STRING, "hvc1",
      "alignment", G_TYPE_STRING, "au", NULL);

  enc->output_state =
      gst_video_encoder_set_output_state (encoder, out_caps, state);

  return TRUE;
}

static void
_gst_libde265_enc_plugin_release_packet (void *user_data)
{
  struct en265_packet *packet = (struct en265_packet *) user_data;

  //en265_free_packet(packet->encoder_context, packet);
}

static GstFlowReturn
_gst_libde265_enc_return_packets (VIDEO_ENCODER_BASE * encoder,
    VIDEO_FRAME * frame)
{
  GstLibde265Enc *enc = GST_LIBDE265_ENC (encoder);
  GstMemory *mem;
  GstFlowReturn result = GST_FLOW_OK;
  VIDEO_FRAME *out_frame;
  int frame_number;

  while (result == GST_FLOW_OK) {
    struct en265_packet *packet = en265_get_packet (enc->ctx, 0);
    if (packet == NULL) {
      break;
    }

    if (packet->content_type == EN265_PACKET_SKIPPED_IMAGE) {
      /* Ignore packets for images that were not encoded. */
      en265_free_packet (enc->ctx, packet);
      continue;
    }

    if (enc->codec_data != NULL) {
      if (packet->content_type < EN265_PACKET_SLICE) {
        /* Generate codec data. */
        gst_buffer_append_memory (enc->codec_data,
            gst_memory_ref (enc->nal_header_memory));

        mem =
            gst_memory_new_wrapped (GST_MEMORY_FLAG_READONLY,
            (gpointer) packet->data, packet->length, 0, packet->length, packet,
            _gst_libde265_enc_plugin_release_packet);
        if (mem == NULL) {
          en265_free_packet (enc->ctx, packet);
          return GST_FLOW_ERROR;
        }

        gst_buffer_append_memory (enc->codec_data, mem);
        continue;
      }

      /* Got first image, pass codec data to output state. */
      GValue value = { 0 };
      g_value_init (&value, GST_TYPE_BUFFER);

      gst_value_set_buffer (&value, enc->codec_data);
      gst_buffer_unref (enc->codec_data);
      enc->codec_data = NULL;
      gst_structure_set_value (gst_caps_get_structure (enc->output_state->caps,
              0), "codec_data", &value);
      g_value_unset (&value);
    }

    if (enc->output_buffer == NULL) {
      enc->output_buffer = gst_buffer_new ();
      if (enc->output_buffer == NULL) {
        GST_ERROR_OBJECT (encoder, "Failed to allocate output buffer");
        return GST_FLOW_ERROR;
      }
    }

    gst_buffer_append_memory (enc->output_buffer,
        gst_memory_ref (enc->nal_header_memory));

    mem =
        gst_memory_new_wrapped (GST_MEMORY_FLAG_READONLY,
        (gpointer) packet->data, packet->length, 0, packet->length, packet,
        _gst_libde265_enc_plugin_release_packet);
    if (mem == NULL) {
      GST_ERROR_OBJECT (encoder, "Failed to wrap memory for packet data");
      return GST_FLOW_ERROR;
    }
    gst_buffer_append_memory (enc->output_buffer, mem);

    /* TODO(fancycode): this should actually check flag "final_slice". */
    if (packet->content_type != EN265_PACKET_SLICE) {
      continue;
    }

    /* Associate encoded packet with correct input frame. */
    g_assert (packet->input_image != NULL);
    frame_number =
        (uintptr_t) de265_get_image_user_data (packet->input_image) - 1;
    if (frame_number != -1) {
      out_frame = gst_video_encoder_get_frame (encoder, frame_number);
    } else {
      out_frame = NULL;
    }

    if (frame != NULL) {
      gst_video_codec_frame_unref (frame);
    }

    if (out_frame == NULL) {
      GST_ERROR_OBJECT (encoder, "No frame available to return");
      return GST_FLOW_ERROR;
    }

    out_frame->output_buffer = enc->output_buffer;
    enc->output_buffer = NULL;

    result = ENCODER_FINISH_FRAME (encoder, out_frame);
  }

  return result;
}

static GstFlowReturn
gst_libde265_enc_handle_frame (VIDEO_ENCODER_BASE * encoder,
    VIDEO_FRAME * frame)
{
  GstLibde265Enc *enc = GST_LIBDE265_ENC (encoder);
  de265_error err;
  struct de265_image *image;
  int width;
  int height;
  int i;
  GstClockTime pts;
  GstVideoFrame vframe;

#if GST_CHECK_VERSION(1,0,0)
  width = enc->input_state->info.width;
  height = enc->input_state->info.height;
#else
  width = enc->width;
  height = enc->height;
#endif
  pts = frame->pts;
  if (pts == GST_CLOCK_TIME_NONE) {
    pts = frame->dts;
  }
  image =
      en265_allocate_image (enc->ctx, width, height, de265_chroma_420,
      (de265_PTS) pts, (void *) (uintptr_t) (frame->system_frame_number + 1));

  if (!gst_video_frame_map (&vframe, &enc->input_state->info,
          frame->input_buffer, GST_MAP_READ)) {
    GST_ERROR_OBJECT (enc, "Failed to map frame input buffer");
    goto error;
  }

  /* TODO(fancycode): store input memory directly in image. */
  for (i = 0; i < 3; i++) {
    int stride = GST_VIDEO_FRAME_PLANE_STRIDE (&vframe, i);
    uint8_t *data = GST_VIDEO_FRAME_PLANE_DATA (&vframe, i);
    uint8_t *tmpdata =
        (uint8_t *) malloc (stride * GST_VIDEO_FRAME_COMP_HEIGHT (&vframe, i));
    memcpy (tmpdata, data, stride * GST_VIDEO_FRAME_COMP_HEIGHT (&vframe, i));
    de265_set_image_plane (image, i, tmpdata, stride, NULL);
  }
  gst_video_frame_unmap (&vframe);

  en265_push_image (enc->ctx, image);

  err = en265_encode (enc->ctx);
  if (!de265_isOK (err)) {
    GST_ELEMENT_ERROR (encoder, STREAM, ENCODE,
        ("Failed to encode: %s (code=%d)",
            de265_get_error_text (err), err), (NULL));
    return GST_FLOW_ERROR;
  }

  return _gst_libde265_enc_return_packets (encoder, frame);

error:
  return GST_FLOW_ERROR;
}

static GstFlowReturn
gst_libde265_enc_finish (VIDEO_ENCODER_BASE * encoder)
{
  GstLibde265Enc *enc = GST_LIBDE265_ENC (encoder);
  de265_error err;

  err = en265_push_eof (enc->ctx);
  if (!de265_isOK (err)) {
    GST_ELEMENT_ERROR (encoder, STREAM, ENCODE,
        ("Failed to push EOS to encoder: %s (code=%d)",
            de265_get_error_text (err), err), (NULL));
    return GST_FLOW_ERROR;
  }

  /* TODO(fancycode): In the future this might return early if the output queue is full, so the encoder state should get checked */
  err = en265_encode (enc->ctx);
  if (!de265_isOK (err)) {
    GST_ELEMENT_ERROR (encoder, STREAM, ENCODE,
        ("Failed to encode: %s (code=%d)",
            de265_get_error_text (err), err), (NULL));
    return GST_FLOW_ERROR;
  }

  return _gst_libde265_enc_return_packets (encoder, NULL);
}

gboolean
gst_libde265_enc_plugin_init (GstPlugin * plugin)
{
  /* create an elementfactory for the libde265 encoder element */
  if (!gst_element_register (plugin, "libde265enc",
          GST_RANK_PRIMARY, GST_TYPE_LIBDE265_ENC))
    return FALSE;

  return TRUE;
}
