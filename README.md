gstreamer-libde265
====================

Powered by libde265

![powered by libde265](doc/libde265.png)

Compile for gstreamer0.10:

    $ make

or

    $ make GSTREAMER_VERSION=0.10


Compile for gstreamer1.0:

    $ make GSTREAMER_VERSION=1.0


See the header of `Makefile` for dependencies.

The source build of `libde265` is expected at `../libde265` by default,
you can override this by passing `LIBDE265_ROOT=/path/to/libde265`.

To test, make sure the `libde265.so.0` is in your `LD_LIBRARY_PATH` and
execute:

    $ gst-launch-0.10 --gst-plugin-path=/path/to/gstreamer-libde265 playbin uri=file:///path/to/sample-hevc.mkv

You can also playback raw H.265/HEVC  bitstreams by switching the decoder
to raw-mode and passing the desired framerate:

    $ gst-launch-0.10 --gst-plugin-path=/path/to/gstreamer-libde265 \
        filesrc location=/path/to/sample-bitstream.hevc \
        ! libde265dec mode=raw framerate=25/1 ! xvimagesink

Prebuilt packages for Ubuntu are available at
https://launchpad.net/~strukturag/+archive/libde265

Approximated performance (measured using the totem video
player on a Intel(R) Core(TM) i7-2700K CPU @ 3.50GHz with
8 CPU cores on Ubuntu 12.04, 64bit):

| Resolution        | fps     | CPU usage @ 24 fps |
| ----------------- | ------- | ------------------ |
| 720p              | 685 fps | ~28 %              |
| 1080p             | 240 fps | ~80 %              |
| 4K                | 51 fps  | ~380 %             |

Copyright (c) 2014 struktur AG
