/*
 * Simple GStreamer HEVC/H.265 video player.
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

#include <gst/gst.h>
#include <glib.h>
#ifdef G_OS_UNIX
#include <glib-unix.h>
#endif

static gboolean
bus_callback (GstBus * bus, GstMessage * msg, gpointer data)
{
  GMainLoop *loop = (GMainLoop *) data;

  switch (GST_MESSAGE_TYPE (msg)) {
    case GST_MESSAGE_EOS:
    {
      g_print ("End of stream\n");
      g_main_loop_quit (loop);
    }
      break;

    case GST_MESSAGE_ERROR:
    {
      gchar *debug;
      GError *error;

      gst_message_parse_error (msg, &error, &debug);
      g_free (debug);

      g_printerr ("Error: %s\n", error->message);
      g_error_free (error);

      g_main_loop_quit (loop);
    }
      break;

    case GST_MESSAGE_APPLICATION:
    {
      const GstStructure *s;

      s = gst_message_get_structure (msg);
      if (gst_structure_has_name (s, "SignalInterrupt")) {
        g_main_loop_quit (loop);
      }
    }
      break;

    default:
      break;
  }

  return TRUE;
}

#ifdef G_OS_UNIX
static gboolean
sigint_handler (gpointer data)
{
  GstElement *pipeline = (GstElement *) data;

  gst_element_post_message (GST_ELEMENT (pipeline),
      gst_message_new_application (GST_OBJECT (pipeline),
          gst_structure_new ("SignalInterrupt", "message", G_TYPE_STRING,
              "Pipeline interrupted", NULL)));

  return TRUE;
}
#endif

int
main (int argc, char *argv[])
{
  gint fps = 25;
  GMainLoop *loop;
  GstElement *source;
  GstElement *decoder;
  GstElement *sink;
  GstElement *pipeline;
  GstBus *bus;
  guint bus_watch_id;
  GOptionEntry options[] = {
    {"fps", 'f', 0, G_OPTION_ARG_INT, &fps,
        "Framerate to playback stream [default: 25]", "N"},
    {NULL}
  };
  GOptionContext *ctx;
  GError *err = NULL;

  ctx = g_option_context_new ("<filename>");
  g_option_context_add_main_entries (ctx, options, NULL);
  g_option_context_add_group (ctx, gst_init_get_option_group ());
  if (!g_option_context_parse (ctx, &argc, &argv, &err)) {
    if (err) {
      g_printerr ("Error initializing: %s\n", GST_STR_NULL (err->message));
    } else {
      g_printerr ("Error initializing: Unknown error!\n");
    }
    return -1;
  }
  g_option_context_free (ctx);

  if (argc != 2) {
    g_printerr ("Usage: %s filename\n", argv[0]);
    return -1;
  }

  pipeline = gst_pipeline_new ("example-player");
  source = gst_element_factory_make ("filesrc", "file-source");
  if (source == NULL) {
    g_printerr ("Could not create source element\n");
    return -1;
  }
  decoder = gst_element_factory_make ("libde265dec", "libde265 decoder");
  if (decoder == NULL) {
    g_printerr ("Could not create decoder element, please check your "
        "GStreamer plugin path.\n");
    return -1;
  }
  sink = gst_element_factory_make ("autovideosink", "video-output");
  if (sink == NULL) {
    g_printerr ("Could not create sink element.\n");
    return -1;
  }

  g_object_set (G_OBJECT (source), "location", argv[1], NULL);
  g_object_set (G_OBJECT (decoder), "mode", 1, NULL);
  g_object_set (G_OBJECT (decoder), "framerate", fps, 1, NULL);

  loop = g_main_loop_new (NULL, FALSE);
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  bus_watch_id = gst_bus_add_watch (bus, bus_callback, loop);
#ifdef G_OS_UNIX
  g_unix_signal_add (SIGINT, (GSourceFunc) sigint_handler, pipeline);
#endif

  gst_bin_add_many (GST_BIN (pipeline), source, decoder, sink, NULL);

  gst_element_link_many (source, decoder, sink, NULL);

  gst_element_set_state (pipeline, GST_STATE_PLAYING);
  g_print ("Playing...\n");
  g_main_loop_run (loop);

  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (GST_OBJECT (pipeline));
  g_source_remove (bus_watch_id);
  g_main_loop_unref (loop);
  gst_deinit ();
  return 0;
}
