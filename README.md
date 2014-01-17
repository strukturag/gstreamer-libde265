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

Prebuilt packages for Ubuntu are available at
https://launchpad.net/~strukturag/+archive/libde265

Copyright (c) 2014 struktur AG
