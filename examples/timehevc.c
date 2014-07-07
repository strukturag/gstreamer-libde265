/*
 * Measure GStreamer HEVC/H.265 player performance.
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
#include <sys/time.h>

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

static void
on_pad_added (GstElement * element, GstPad * pad, gpointer data)
{
  GstPad *sinkpad;
  GstElement *decoder = (GstElement *) data;

  /* We can now link this pad with the decoder sink pad */
  g_print ("Dynamic pad created, linking demuxer/decoder\n");

  sinkpad = gst_element_get_static_pad (decoder, "sink");

  gst_pad_link (pad, sinkpad);

  gst_object_unref (sinkpad);
}

static void
global_get_cpu_stats (int64_t * user, int64_t * system)
{
  FILE *fp = fopen ("/proc/stat", "rb");
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  g_assert (fp != NULL);
  *user = *system = -1;
  while ((read = getline (&line, &len, fp)) != -1) {
    if (strstr (line, "cpu ") == line) {
      char *ptr = NULL;
      char *userinfo = strtok_r (line + 4, " ", &ptr);
      char *niceinfo = strtok_r (NULL, " ", &ptr);
      char *systeminfo = strtok_r (NULL, " ", &ptr);
      char *idleinfo = strtok_r (NULL, " ", &ptr);
      *user = atoll (userinfo) + atoll (niceinfo) + atoll (idleinfo);
      *system = atoll (systeminfo);
      break;
    }
  }
  free (line);
  fclose (fp);
}

static void
process_get_cpu_stats (int64_t * user, int64_t * system)
{
  char filename[8192];
  sprintf (filename, "/proc/%d/stat", getpid ());
  FILE *fp = fopen (filename, "rb");
  unsigned long userinfo;
  unsigned long systeminfo;

  g_assert (fp != NULL);
  fscanf (fp, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu",
      &userinfo, &systeminfo);
  *user = userinfo;
  *system = systeminfo;
  fclose (fp);
}

int
main (int argc, char *argv[])
{
  GMainLoop *loop;
  GstElement *source;
  GstElement *demuxer;
  GstElement *decoder;
  GstElement *sink;
  GstElement *pipeline;
  GstBus *bus;
  guint bus_watch_id;
  GOptionEntry options[] = {
    {NULL}
  };
  GOptionContext *ctx;
  GError *err = NULL;
  int64_t global_user_before, global_system_before;
  int64_t global_user_after, global_system_after;
  int64_t process_user_before, process_system_before;
  int64_t process_user_after, process_system_after;
  struct timeval time_before, time_after;

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
  demuxer =
      gst_element_factory_make ("matroskademux-libde265", "matroska demuxer");
  if (demuxer == NULL) {
    g_printerr ("Could not create demuxer element\n");
    return -1;
  }
  decoder = gst_element_factory_make ("libde265dec", "libde265 decoder");
  if (decoder == NULL) {
    g_printerr ("Could not create decoder element, please check your "
        "GStreamer plugin path.\n");
    return -1;
  }
  sink = gst_element_factory_make ("fakesink", "video-output");
  if (sink == NULL) {
    g_printerr ("Could not create sink element.\n");
    return -1;
  }

  g_object_set (G_OBJECT (source), "location", argv[1], NULL);
  g_object_set (G_OBJECT (sink), "sync", FALSE, NULL);

  loop = g_main_loop_new (NULL, FALSE);
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  bus_watch_id = gst_bus_add_watch (bus, bus_callback, loop);
#ifdef G_OS_UNIX
  g_unix_signal_add (SIGINT, (GSourceFunc) sigint_handler, pipeline);
#endif

  gst_bin_add_many (GST_BIN (pipeline), source, demuxer, decoder, sink, NULL);

  gst_element_link (source, demuxer);
  gst_element_link_many (decoder, sink, NULL);
  g_signal_connect (demuxer, "pad-added", G_CALLBACK (on_pad_added), decoder);

  global_get_cpu_stats (&global_user_before, &global_system_before);
  process_get_cpu_stats (&process_user_before, &process_system_before);
  gettimeofday (&time_before, NULL);
  gst_element_set_state (pipeline, GST_STATE_PLAYING);
  g_print ("Playing...\n");
  g_main_loop_run (loop);
  global_get_cpu_stats (&global_user_after, &global_system_after);
  process_get_cpu_stats (&process_user_after, &process_system_after);
  gettimeofday (&time_after, NULL);
  printf ("Global ticks:  user=%6ld system=%6ld\n",
      global_user_after - global_user_before,
      global_system_after - global_system_before);
  printf ("Process ticks: user=%6ld system=%6ld\n",
      process_user_after - process_user_before,
      process_system_after - process_system_before);
  float user_cpu =
      100 * (process_user_after -
      process_user_before) / ((float) global_user_after - global_user_before);
  float system_cpu =
      100 * (process_system_after -
      process_system_before) / ((float) global_system_after -
      global_system_before);
  printf ("CPU usage:     user=%.3f%% system=%.3f%%\n", user_cpu, system_cpu);

  int movie_fps = 24;           // TOOD: get this from pipeline/demuxer/decoder somehow
  gint64 pos;
#if GST_CHECK_VERSION(1,0,0)
  gst_element_query_position (pipeline, GST_FORMAT_TIME, &pos);
#else
  GstFormat query = GST_FORMAT_TIME;
  gst_element_query_position (pipeline, &query, &pos);
#endif
  gint64 frames = movie_fps * (pos / 1000000000.0);
  printf ("Played %.3f seconds (%ld frames)\n", pos / 1000000000.0, frames);

  struct timeval time_total;
  timersub (&time_after, &time_before, &time_total);
  float fps =
      frames / ((float) time_total.tv_sec + (time_total.tv_usec / 1000000.0));
  printf ("Playback performance %.3f fps @ %.3f%%\n", fps, user_cpu);
  printf ("Max performance      %.3f fps @ 100%%\n", fps * (100.0 / user_cpu));

  gst_element_set_state (pipeline, GST_STATE_NULL);
  gst_object_unref (GST_OBJECT (pipeline));
  g_source_remove (bus_watch_id);
  g_main_loop_unref (loop);
  gst_deinit ();
  return 0;
}
