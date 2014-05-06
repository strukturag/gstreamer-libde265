/* GStreamer base utils library codec-specific utility functions
 * Copyright (C) 2010 Arun Raghavan <arun.raghavan@collabora.co.uk>
 *               2013 Sreerenj Balachandran <sreerenj.balachandran@intel.com>
 *               2010 Collabora Multimedia
 *               2010 Nokia Corporation
 *               2013 Intel Corporation
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

/**
 * SECTION:gstpbutilscodecutils
 * @short_description: Miscellaneous codec-specific utility functions
 *
 * <refsect2>
 * <para>
 * Provides codec-specific ulility functions such as functions to provide the
 * codec profile and level in human-readable string form from header data.
 * </para>
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>

#include "codec-utils.h"

#define GST_SIMPLE_CAPS_HAS_NAME(caps,name) \
    gst_structure_has_name(gst_caps_get_structure((caps),0),(name))

static const gchar *
digit_to_string (guint digit)
{
  static const char itoa[][2] = {
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"
  };

  if (G_LIKELY (digit < 10))
    return itoa[digit];
  else
    return NULL;
}

/**
 * gst_codec_utils_h265_get_profile:
 * @profile_tier_level: Pointer to the profile_tier_level
 *   structure for the stream.
 * @len: Length of the data available in @profile_tier_level
 *
 * Converts the profile indication (general_profile_idc) in the stream's
 * profile_level_tier structure into a string. The profile_tier_level is
 * expected to have the following format, as defined in the H.265
 * specification. The profile_tier_level is viewed as a bitstream here,
 * with bit 0 being the most significant bit of the first byte.
 *
 * <itemizedlist>
 * <listitem><para>Bit 0:1   - general_profile_space</para></listitem>
 * <listitem><para>Bit 2     - general_tier_flag</para></listitem>
 * <listitem><para>Bit 3:7   - general_profile_idc</para></listitem>
 * <listitem><para>Bit 8:39  - gernal_profile_compatibility_flags</para></listitem>
 * <listitem><para>Bit 40    - general_progressive_source_flag</para></listitem>
 * <listitem><para>Bit 41    - general_interlaced_source_flag</para></listitem>
 * <listitem><para>Bit 42    - general_non_packed_constraint_flag</para></listitem>
 * <listitem><para>Bit 43    - general_frame_only_constraint_flag</para></listitem>
 * <listitem><para>Bit 44:87 - general_reserved_zero_44bits</para></listitem>
 * <listitem><para>Bit 88:95 - general_level_idc</para></listitem>
 * </itemizedlist>
 *
 * Returns: The profile as a const string, or %NULL if there is an error.
 *
 * Since 1.4
 */
const gchar *
gst_codec_utils_h265_get_profile (const guint8 * profile_tier_level, guint len)
{
  const gchar *profile = NULL;
  gint gpcf1 = 0, gpcf2 = 0, gpcf3 = 0;
  gint profile_idc;

  g_return_val_if_fail (profile_tier_level != NULL, NULL);

  if (len < 2)
    return NULL;

  GST_MEMDUMP ("ProfileTierLevel", profile_tier_level, len);

  profile_idc = (profile_tier_level[0] & 0x1f);

  gpcf1 = (profile_tier_level[1] & 0x40) >> 6;
  gpcf2 = (profile_tier_level[1] & 0x20) >> 5;
  gpcf3 = (profile_tier_level[1] & 0x10) >> 4;

  if (profile_idc == 1 || gpcf1)
    profile = "main";
  else if (profile_idc == 2 || gpcf2)
    profile = "main-10";
  else if (profile_idc == 3 || gpcf3)
    profile = "main-still-picture";
  else
    profile = NULL;

  return profile;
}

/**
 * gst_codec_utils_h265_get_tier:
 * @profile_tier_level: Pointer to the profile_tier_level structure
 *   for the stream.
 * @len: Length of the data available in @profile_tier_level.
 *
 * Converts the tier indication (general_tier_flag) in the stream's
 * profile_tier_level structure into a string. The profile_tier_level
 * is expected to have the same format as for gst_codec_utils_h264_get_profile().
 *
 * Returns: The tier as a const string, or %NULL if there is an error.
 *
 * Since 1.4
 */
const gchar *
gst_codec_utils_h265_get_tier (const guint8 * profile_tier_level, guint len)
{
  const gchar *tier = NULL;
  gint tier_flag = 0;

  g_return_val_if_fail (profile_tier_level != NULL, NULL);

  if (len < 1)
    return NULL;

  GST_MEMDUMP ("ProfileTierLevel", profile_tier_level, len);

  tier_flag = (profile_tier_level[0] & 0x20) >> 5;

  if (tier_flag)
    tier = "high";
  else
    tier = "main";

  return tier;
}

/**
 * gst_codec_utils_h265_get_level:
 * @profile_tier_level: Pointer to the profile_tier_level structure
 *   for the stream
 * @len: Length of the data available in @profile_tier_level.
 *
 * Converts the level indication (general_level_idc) in the stream's
 * profile_tier_level structure into a string. The profiel_tier_level is
 * expected to have the same format as for gst_codec_utils_h264_get_profile().
 *
 * Returns: The level as a const string, or %NULL if there is an error.
 *
 * Since 1.4
 */
const gchar *
gst_codec_utils_h265_get_level (const guint8 * profile_tier_level, guint len)
{
  g_return_val_if_fail (profile_tier_level != NULL, NULL);

  if (len < 12)
    return NULL;

  GST_MEMDUMP ("ProfileTierLevel", profile_tier_level, len);

  if (profile_tier_level[11] % 30 == 0)
    return digit_to_string (profile_tier_level[11] / 30);
  else {
    switch (profile_tier_level[11]) {
      case 63:
        return "2.1";
        break;
      case 93:
        return "3.1";
        break;
      case 123:
        return "4.1";
        break;
      case 153:
        return "5.1";
        break;
      case 156:
        return "5.2";
        break;
      case 183:
        return "6.1";
        break;
      case 186:
        return "6.2";
        break;
      default:
        return NULL;
    }
  }
}

/**
 * gst_codec_utils_h265_get_level_idc:
 * @level: A level string from caps
 *
 * Transform a level string from the caps into the level_idc
 *
 * Returns: the level_idc or 0 if the level is unknown
 *
 * Since 1.4
 */
guint8
gst_codec_utils_h265_get_level_idc (const gchar * level)
{
  g_return_val_if_fail (level != NULL, 0);

  if (!strcmp (level, "1"))
    return 30;
  else if (!strcmp (level, "2"))
    return 60;
  else if (!strcmp (level, "2.1"))
    return 63;
  else if (!strcmp (level, "3"))
    return 90;
  else if (!strcmp (level, "3.1"))
    return 93;
  else if (!strcmp (level, "4"))
    return 120;
  else if (!strcmp (level, "4.1"))
    return 123;
  else if (!strcmp (level, "5"))
    return 150;
  else if (!strcmp (level, "5.1"))
    return 153;
  else if (!strcmp (level, "5.2"))
    return 156;
  else if (!strcmp (level, "6"))
    return 180;
  else if (!strcmp (level, "6.1"))
    return 183;
  else if (!strcmp (level, "6.2"))
    return 186;

  GST_WARNING ("Invalid level %s", level);
  return 0;
}

/**
 * gst_codec_utils_h265_caps_set_level_tier_and_profile:
 * @caps: the #GstCaps to which the level, tier and profile are to be added
 * @profile_tier_level: Pointer to the profile_tier_level struct
 * @len: Length of the data available in @profile_tier_level.
 *
 * Sets the level, tier and profile in @caps if it can be determined from
 * @profile_tier_level. See gst_codec_utils_h265_get_level(),
 * gst_codec_utils_h265_get_tier() and gst_codec_utils_h265_get_profile()
 * for more details on the parameters.
 *
 * Returns: %TRUE if the level, tier, profile could be set, %FALSE otherwise.
 *
 * Since 1.4
 */
gboolean
gst_codec_utils_h265_caps_set_level_tier_and_profile (GstCaps * caps,
    const guint8 * profile_tier_level, guint len)
{
  const gchar *level, *tier, *profile;

  g_return_val_if_fail (GST_IS_CAPS (caps), FALSE);
  g_return_val_if_fail (GST_CAPS_IS_SIMPLE (caps), FALSE);
  g_return_val_if_fail (GST_SIMPLE_CAPS_HAS_NAME (caps, "video/x-h265"), FALSE);
  g_return_val_if_fail (profile_tier_level != NULL, FALSE);

  level = gst_codec_utils_h265_get_level (profile_tier_level, len);
  if (level != NULL)
    gst_caps_set_simple (caps, "level", G_TYPE_STRING, level, NULL);

  tier = gst_codec_utils_h265_get_tier (profile_tier_level, len);
  if (tier != NULL)
    gst_caps_set_simple (caps, "tier", G_TYPE_STRING, tier, NULL);

  profile = gst_codec_utils_h265_get_profile (profile_tier_level, len);
  if (profile != NULL)
    gst_caps_set_simple (caps, "profile", G_TYPE_STRING, profile, NULL);

  GST_LOG ("profile : %s", (profile) ? profile : "---");
  GST_LOG ("tier    : %s", (tier) ? tier : "---");
  GST_LOG ("level   : %s", (level) ? level : "---");

  return (level != NULL && tier != NULL && profile != NULL);
}
