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

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "libde265-dec.h"

// use two decoder threads if no information about
// available CPU cores can be retrieved
#define DEFAULT_THREAD_COUNT        2

#define READ_BE32(x)                                 \
    (((uint32_t)((const uint8_t*)(x))[0] << 24) |    \
               (((const uint8_t*)(x))[1] << 16) |    \
               (((const uint8_t*)(x))[2] <<  8) |    \
                ((const uint8_t*)(x))[3])

#define WRITE_BE32(p, darg) do {                \
        unsigned d = (darg);                    \
        ((uint8_t*)(p))[3] = (d);               \
        ((uint8_t*)(p))[2] = (d)>>8;            \
        ((uint8_t*)(p))[1] = (d)>>16;           \
        ((uint8_t*)(p))[0] = (d)>>24;           \
    } while(0)

#define parent_class gst_libde265_dec_parent_class
G_DEFINE_TYPE (GstLibde265Dec, gst_libde265_dec, VIDEO_DECODER_TYPE);

static GstStaticPadTemplate sink_template = GST_STATIC_PAD_TEMPLATE (
    "sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("video/x-h265")
    );

static GstStaticPadTemplate src_template = GST_STATIC_PAD_TEMPLATE (
    "src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (
#if GST_CHECK_VERSION(1,0,0)
        GST_VIDEO_CAPS_MAKE("{ I420 }")
#else
        GST_VIDEO_CAPS_YUV("{ I420 }")
#endif
    )
    );

static void gst_libde265_dec_finalize (GObject * object);

static gboolean gst_libde265_dec_start (VIDEO_DECODER_BASE * parse);
static gboolean gst_libde265_dec_stop (VIDEO_DECODER_BASE * parse);
#if GST_CHECK_VERSION(1,0,0)
static GstFlowReturn gst_libde265_dec_parse (VIDEO_DECODER_BASE * parse,
    VIDEO_FRAME *frame,
    GstAdapter *adapter,
    gboolean at_eos);
static gboolean gst_libde265_dec_set_format (VIDEO_DECODER_BASE * parse,
    GstVideoCodecState * state);
#else
static GstFlowReturn gst_libde265_dec_parse_data (VIDEO_DECODER_BASE * parse,
    gboolean at_eos);
#endif
#if GST_CHECK_VERSION(1,2,0)
static gboolean gst_libde265_dec_flush (VIDEO_DECODER_BASE * parse);
#elif GST_CHECK_VERSION(1,0,0)
static gboolean gst_libde265_dec_reset (VIDEO_DECODER_BASE * parse,
    gboolean hard);
#else
static gboolean gst_libde265_dec_reset (VIDEO_DECODER_BASE * parse);
#endif
static GstFlowReturn gst_libde265_dec_handle_frame (VIDEO_DECODER_BASE * parse,
    VIDEO_FRAME * frame);

static void
gst_libde265_dec_class_init (GstLibde265DecClass * klass)
{
    GObjectClass *gobject_class = (GObjectClass *) klass;
    VIDEO_DECODER_CLASS *decoder_class = VIDEO_DECODER_GET_CLASS (klass);
    GstElementClass *gstelement_class = GST_ELEMENT_CLASS (klass);

    gobject_class->finalize = gst_libde265_dec_finalize;

    decoder_class->start = GST_DEBUG_FUNCPTR (gst_libde265_dec_start);
    decoder_class->stop = GST_DEBUG_FUNCPTR (gst_libde265_dec_stop);
#if GST_CHECK_VERSION(1,0,0)
    decoder_class->parse = GST_DEBUG_FUNCPTR (gst_libde265_dec_parse);
    decoder_class->set_format = GST_DEBUG_FUNCPTR (gst_libde265_dec_set_format);
#else
    decoder_class->parse_data = GST_DEBUG_FUNCPTR (gst_libde265_dec_parse_data);
#endif
#if GST_CHECK_VERSION(1,2,0)
    decoder_class->flush = GST_DEBUG_FUNCPTR (gst_libde265_dec_flush);
#else
    decoder_class->reset = GST_DEBUG_FUNCPTR (gst_libde265_dec_reset);
#endif
    decoder_class->handle_frame = GST_DEBUG_FUNCPTR (gst_libde265_dec_handle_frame);

    gst_element_class_add_pad_template (gstelement_class,
        gst_static_pad_template_get (&sink_template));
    gst_element_class_add_pad_template (gstelement_class,
        gst_static_pad_template_get(&src_template));

    gst_element_class_set_details_simple (gstelement_class,
        "HEVC/H.265 parser",
        "Codec/Parser/Converter/Video",
        "Decodes HEVC/H.265 video streams using libde265",
        "struktur AG <opensource@struktur.de>");
}

static inline void
_gst_libde265_dec_reset_decoder (GstLibde265Dec * dec)
{
    dec->ctx = NULL;
    dec->width = -1;
    dec->height = -1;
#if GST_CHECK_VERSION(1,0,0)
    dec->input_state = NULL;
#endif
}

static void
gst_libde265_dec_init (GstLibde265Dec * dec)
{
    _gst_libde265_dec_reset_decoder (dec);
#if GST_CHECK_VERSION(1,0,0)
    gst_video_decoder_set_packetized(GST_VIDEO_DECODER(dec), FALSE);
#endif
}

static inline void
_gst_libde265_dec_free_decoder (GstLibde265Dec *dec)
{
    if (dec->ctx != NULL) {
        de265_free_decoder(dec->ctx);
    }
#if GST_CHECK_VERSION(1,0,0)
    if (dec->input_state != NULL) {
        gst_video_codec_state_unref(dec->input_state);
    }
#endif
    _gst_libde265_dec_reset_decoder (dec);
}

static void
gst_libde265_dec_finalize (GObject * object)
{
    GstLibde265Dec *dec = GST_LIBDE265_DEC (object);
    
    _gst_libde265_dec_free_decoder(dec);
    
    G_OBJECT_CLASS (parent_class)->finalize (object);
}

static gboolean
gst_libde265_dec_start (VIDEO_DECODER_BASE * parse)
{
    GstLibde265Dec *dec = GST_LIBDE265_DEC (parse);
    
    _gst_libde265_dec_free_decoder(dec);
    dec->ctx = de265_new_decoder();
    if (dec->ctx == NULL) {
        return FALSE;
    }
    
    int threads;
#if defined(_SC_NPROC_ONLN)
    threads = sysconf(_SC_NPROC_ONLN);
#elif defined(_SC_NPROCESSORS_ONLN)
    threads = sysconf(_SC_NPROCESSORS_ONLN);
#else
    #warning "Don't know how to get number of CPU cores, will use the default thread count"
    threads = DEFAULT_THREAD_COUNT;
#endif
    if (threads <= 0) {
        threads = DEFAULT_THREAD_COUNT;
    }
    
    de265_start_worker_threads(dec->ctx, threads);
    GST_INFO ("Starting %d worker threads\n", threads);
    
    return TRUE;
}

static gboolean
gst_libde265_dec_stop (VIDEO_DECODER_BASE * parse)
{
    GstLibde265Dec *dec = GST_LIBDE265_DEC (parse);
    
    _gst_libde265_dec_free_decoder(dec);
    
    return TRUE;
}

#if GST_CHECK_VERSION(1,2,0)
static gboolean gst_libde265_dec_flush (VIDEO_DECODER_BASE * parse)
#elif GST_CHECK_VERSION(1,0,0)
static gboolean gst_libde265_dec_reset (VIDEO_DECODER_BASE * parse,
    gboolean hard)
#else
static gboolean gst_libde265_dec_reset (VIDEO_DECODER_BASE * parse)
#endif
{
    GstLibde265Dec *dec = GST_LIBDE265_DEC (parse);

    // flush any pending decoded images
    const struct de265_image *img;
    do {
        img = de265_get_next_picture(dec->ctx);
    } while (img != NULL);

    return TRUE;
}

#if GST_CHECK_VERSION(1,0,0)
static GstFlowReturn gst_libde265_dec_parse (VIDEO_DECODER_BASE * parse,
    VIDEO_FRAME *frame,
    GstAdapter *adapter,
    gboolean at_eos)
#else
static GstFlowReturn gst_libde265_dec_parse_data (VIDEO_DECODER_BASE * parse,
    gboolean at_eos)
#endif
{
    GstLibde265Dec *dec = GST_LIBDE265_DEC (parse);
#if !GST_CHECK_VERSION(1,0,0)
    GstAdapter *adapter = parse->input_adapter;
#endif
    gsize size = gst_adapter_available (adapter);
    if (size == 0) {
        return NEED_DATA_RESULT;
    }
    
    GstBuffer *buf = gst_adapter_take_buffer(adapter, size);
    uint8_t *frame_data;
    uint8_t *start_data;
    uint8_t *end_data;
#if GST_CHECK_VERSION(1,0,0)
    GstMapInfo info;
    if (!gst_buffer_map(buf, &info, GST_MAP_READWRITE)) {
        return GST_FLOW_ERROR;
    }
    
    frame_data = info.data;
#else
    frame_data = GST_BUFFER_DATA(buf);
#endif
    end_data = frame_data + size;
    
    // replace 4-byte length fields with NAL start codes
    start_data = frame_data;
    while (start_data + 4 <= end_data) {
        int nal_size = READ_BE32(start_data);
        WRITE_BE32(start_data, 0x00000001);
        start_data += 4 + nal_size;
    }
    
    de265_error ret = de265_decode_data(dec->ctx, frame_data, end_data - frame_data);
#if GST_CHECK_VERSION(1,0,0)
    gst_buffer_unmap(buf, &info);
#endif
    gst_buffer_unref(buf);
    if (ret != DE265_OK) {
        GST_ELEMENT_ERROR (parse, STREAM, DECODE,
            ("Error while decoding: %s (code=%d)", de265_get_error_text(ret), ret),
            (NULL));
        return GST_FLOW_ERROR;
    }
    
    while ((ret = de265_get_warning(dec->ctx)) != DE265_OK) {
        GST_ELEMENT_WARNING (parse, STREAM, DECODE,
            ("%s (code=%d)", de265_get_error_text(ret), ret),
            (NULL));
    }
    
    const struct de265_image *img = de265_peek_next_picture(dec->ctx);
    if (img == NULL) {
        // need more data
        return NEED_DATA_RESULT;
    }
    
    int width = de265_get_image_width(img, 0);
    int height = de265_get_image_height(img, 0);

    if (G_UNLIKELY(width != dec->width || height != dec->height)) {
#if GST_CHECK_VERSION(1,0,0)
        gst_video_decoder_set_output_state (parse, GST_VIDEO_FORMAT_I420, width, height, dec->input_state);
        gst_video_decoder_negotiate(parse);
#else
        GstVideoState *state = gst_base_video_decoder_get_state (parse);
        state->format = GST_VIDEO_FORMAT_I420;
        state->width = de265_get_image_width(img, 0);
        state->height = de265_get_image_height(img, 0);
        gst_base_video_decoder_set_src_caps (parse);
#endif
        GST_DEBUG ("Frame dimensions are %d x %d\n", width, height);
        dec->width = width;
        dec->height = height;
    }
    
    return HAVE_FRAME (parse);
}

#if GST_CHECK_VERSION(1,0,0)
static gboolean gst_libde265_dec_set_format (VIDEO_DECODER_BASE * parse,
    GstVideoCodecState * state)
{
    GstLibde265Dec *dec = GST_LIBDE265_DEC (parse);

    if (dec->input_state != NULL) {
        gst_video_codec_state_unref(dec->input_state);
    }
    dec->input_state = state;
    if (state != NULL) {
        gst_video_codec_state_ref(state);
    }
    
    return TRUE;
}
#endif

static GstFlowReturn gst_libde265_dec_handle_frame (VIDEO_DECODER_BASE * parse,
    VIDEO_FRAME * frame)
{
    GstFlowReturn ret = GST_FLOW_OK;
    GstLibde265Dec *dec = GST_LIBDE265_DEC (parse);
    
    const struct de265_image *img = de265_get_next_picture(dec->ctx);
    if (img == NULL) {
        // need more data
        return GST_FLOW_OK;
    }
    
    ret = ALLOC_OUTPUT_FRAME(parse, frame);
    if (ret != GST_FLOW_OK) {
        return ret;
    }
    
    uint8_t *dest;
#if GST_CHECK_VERSION(1,0,0)
    GstMapInfo info;
    if (!gst_buffer_map(frame->output_buffer, &info, GST_MAP_WRITE)) {
        return GST_FLOW_ERROR;
    }
    
    dest = info.data;
#else
    dest = GST_BUFFER_DATA(frame->src_buffer);
#endif
    
    int plane;
    for (plane=0; plane<3; plane++) {
        int stride;
        int height = de265_get_image_height(img, plane);
        const uint8_t *src = de265_get_image_plane(img, plane, &stride);
        memcpy(dest, src, height * stride);
        dest += (height * stride);
    }
#if GST_CHECK_VERSION(1,0,0)
    gst_buffer_unmap(frame->output_buffer, &info);
#endif
    
    return FINISH_FRAME (parse, frame);
}

gboolean
gst_libde265_dec_plugin_init (GstPlugin * plugin)
{
    /* create an elementfactory for the libde265 decoder element */
    if (!gst_element_register (plugin, "libde265dec",
            GST_RANK_PRIMARY, GST_TYPE_LIBDE265_DEC))
        return FALSE;

    return TRUE;
}
