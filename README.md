# gstreamer-libde265

Powered by libde265

![powered by libde265](doc/libde265.png)

## Building

[![Build Status](https://travis-ci.org/strukturag/gstreamer-libde265.png?branch=master)](https://travis-ci.org/strukturag/gstreamer-libde265)

If you got gstreamer-libde265 from the git repository, you will first
need to run the included `autogen.sh` script to generate the `configure`
script.

Compile for gstreamer0.10:

    $ ./configure --enable-gstreamer0.10
    $ make

Compile for gstreamer1.0:

    $ ./configure
    $ make

## Dependencies

Various libraries are required to build gstreamer-libde265:
- `libde265-dev` (>= 0.7),
- `libz-dev`
- `libbz2-dev`
- `libgstreamer0.10-dev` (only for gstreamer0.10)
- `libgstreamer-plugins-base0.10-dev` (only for gstreamer0.10)
- `libgstreamer-plugins-bad0.10-dev` (only for gstreamer0.10)
- `libgstreamer1.0-dev` (only for gstreamer1.0)
- `libgstreamer-plugins-base1.0-dev` (only for gstreamer1.0)
- `libgstreamer-plugins-good1.0-dev` (only for gstreamer1.0)
- `libgstreamer-plugins-bad1.0-dev` (only for gstreamer1.0)

Prebuilt packages for Ubuntu are available at
https://launchpad.net/~strukturag/+archive/libde265

## Running

To test, make sure the `libde265.so.0` is in your `LD_LIBRARY_PATH` and
execute:

    $ gst-launch-0.10 \
        --gst-plugin-path=/path/to/gstreamer-libde265/src/.libs/ \
        playbin uri=file:///path/to/sample-hevc.mkv

You can also playback raw H.265/HEVC  bitstreams by switching the decoder
to raw-mode and passing the desired framerate:

    $ gst-launch-0.10 \
        --gst-plugin-path=/path/to/gstreamer-libde265/src/.libs/ \
        filesrc location=/path/to/sample-bitstream.hevc \
        ! libde265dec mode=raw framerate=25/1 ! xvimagesink

The `examples` folder contains a sample raw bitstream player which can
be used instead of passing the various options to `gst-launch` (assuming
you have all necessary plugins in the GStreamer plugin path):

    $ ./playhevc --fps=25 /path/to/sample-bitstream.hevc

Other commandline switches are available from

    $ ./playhevc --help-all

## Performance

Decoder performance was measured using the `timehevc` tool from the `examples`
folder. The tool plays a Matroska movie to the GStreamer fakesink and measures
the average framerate.

| Resolution        | avg. fps | CPU usage |
| ----------------- | -------- | --------- |
| [720p][1]         |  298 fps |      36 % |
| [1080p][2]        |  161 fps |      43 % |
| [4K][3]           |   40 fps |      55 % |

Environment:
- Intel(R) Core(TM) i7-2700K CPU @ 3.50GHz (4 physical CPU cores)
- Ubuntu 12.04, 64bit
- GStreamer 0.10.36
- libde265 0.7

[1]: http://trailers.divx.com/hevc/TearsOfSteel_720p_24fps_27qp_831kbps_720p_GPSNR_41.65_HM11_2aud_7subs.mkv
[2]: http://trailers.divx.com/hevc/TearsOfSteel_1080p_24fps_27qp_1474kbps_GPSNR_42.29_HM11_2aud_7subs.mkv
[3]: http://trailers.divx.com/hevc/TearsOfSteel_4K_24fps_9500kbps_2aud_9subs.mkv

Copyright (c) 2014 struktur AG
