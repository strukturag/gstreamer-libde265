/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 * Copyright (C) <2003> David A. Schleef <ds@schleef.org>
 * Copyright (C) <2006> Wim Taymans <wim@fluendo.com>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "qtdemux.h"

#include <gst/pbutils/pbutils.h>

gboolean
gst_isomp4_plugin_init (GstPlugin * plugin)
{
#ifdef ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif /* ENABLE_NLS */

  gst_pb_utils_init ();

  /* ensure private tag is registered */
  gst_tag_register (GST_QT_DEMUX_PRIVATE_TAG, GST_TAG_FLAG_META,
      GST_TYPE_SAMPLE, "QT atom", "unparsed QT tag atom",
      gst_tag_merge_use_first);

  gst_tag_register (GST_QT_DEMUX_CLASSIFICATION_TAG, GST_TAG_FLAG_META,
      G_TYPE_STRING, GST_QT_DEMUX_CLASSIFICATION_TAG, "content classification",
      gst_tag_merge_use_first);

  if (!gst_element_register (plugin, "qtdemux",
          GST_RANK_PRIMARY + 1, GST_TYPE_QTDEMUX))
    return FALSE;
  if (!gst_element_register (plugin, "qtdemux-libde265",
          GST_RANK_PRIMARY + 1, GST_TYPE_QTDEMUX))
    return FALSE;

  return TRUE;
}
