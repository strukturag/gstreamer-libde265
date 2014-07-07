/* GStreamer base utils library codec-specific utility functions
 * Copyright (C) 2010 Arun Raghavan <arun.raghavan@collabora.co.uk>
 *               2010 Collabora Multimedia
 *               2010 Nokia Corporation
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef __GST_MATROSKA_CODEC_UTILS_H__
#define __GST_MATROSKA_CODEC_UTILS_H__

#include <gst/gst.h>

G_BEGIN_DECLS

/* H.265 */

const gchar * gst_codec_utils_h265_get_profile                     (const guint8 * profile_tier_level,
                                                                    guint len);

const gchar * gst_codec_utils_h265_get_tier                        (const guint8 * profile_tier_level,
                                                                    guint len);

const gchar * gst_codec_utils_h265_get_level                       (const guint8 * profile_tier_level,
                                                                    guint len);

guint8        gst_codec_utils_h265_get_level_idc                   (const gchar  * level);

gboolean      gst_codec_utils_h265_caps_set_level_tier_and_profile (GstCaps      * caps,
                                                                    const guint8 * profile_tier_level,
                                                                    guint          len);

G_END_DECLS

#endif /* __GST_MATROSKA_CODEC_UTILS_H__ */
