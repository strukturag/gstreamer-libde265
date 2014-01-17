#include <gst/gst.h>

#if GST_CHECK_VERSION(1,2,0)
    #include "1.2/matroska-demux.c"
#elif GST_CHECK_VERSION(1,0,0)
    #include "1.0/matroska-demux.c"
#else
    #include "0.10/matroska-demux.c"
#endif
